import os, time, json, uuid
from datetime import datetime, timedelta, timezone
from typing import Optional
from fastapi import FastAPI, Depends, HTTPException
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from pydantic import BaseModel, EmailStr
import jwt
import psycopg
import redis
from argon2 import PasswordHasher
from dotenv import load_dotenv

load_dotenv()
app = FastAPI(title="Wuxia GM Backend", version="0.1")

DATABASE_URL = os.getenv("DATABASE_URL", "postgresql://wuxia:wuxia_pw@127.0.0.1:5432/wuxia_db")
REDIS_URL = os.getenv("REDIS_URL", "redis://127.0.0.1:6379/0")
JWT_SECRET = os.getenv("ADMIN_JWT_SECRET", "dev_only_secret")
JWT_TTL = int(os.getenv("ADMIN_JWT_TTL_SECONDS", "3600"))

rds = redis.from_url(REDIS_URL, decode_responses=True)
ph = PasswordHasher()
auth_scheme = HTTPBearer()

class LoginBody(BaseModel):
    email: EmailStr
    password: str

class CreateAccountBody(BaseModel):
    email: EmailStr
    password_hash: str  # client should send argon2id hash, or accept plaintext and hash here (not recommended)
    role: str = "player"

class BanBody(BaseModel):
    account_id: str
    reason: str
    hours: Optional[int] = None

class MuteBody(BaseModel):
    account_id: str
    reason: str
    hours: Optional[int] = None

class EventBody(BaseModel):
    event: str  # "BloodMoon", "QiRain", "BlueMoon"
    region_id: Optional[str] = None
    duration_seconds: Optional[int] = 600

def require_jwt(token: HTTPAuthorizationCredentials = Depends(auth_scheme)):
    try:
        payload = jwt.decode(token.credentials, JWT_SECRET, algorithms=["HS256"])
        return payload
    except Exception:
        raise HTTPException(status_code=401, detail="Invalid token")

@app.post("/login")
def login(body: LoginBody):
    with psycopg.connect(DATABASE_URL) as conn:
        with conn.cursor() as cur:
            cur.execute("SELECT id, pwd_hash, role FROM account WHERE email = %s", (body.email,))
            row = cur.fetchone()
            if not row:
                raise HTTPException(status_code=401, detail="No such account")
            acc_id, pwd_hash, role = row
            # For admin panel, store argon2id in pwd_hash; verify here if plaintext is used.
            # If pwd_hash contains argon2id string, this path expects body.password to be plaintext:
            try:
                ph.verify(pwd_hash, body.password)
            except Exception:
                raise HTTPException(status_code=401, detail="Invalid credentials")
            claims = {"sub": str(acc_id), "role": role, "exp": int(time.time()) + JWT_TTL}
            token = jwt.encode(claims, JWT_SECRET, algorithm="HS256")
            return {"token": token, "role": role}

@app.post("/accounts/create")
def create_account(body: CreateAccountBody, claims=Depends(require_jwt)):
    if claims.get("role") not in ("admin", "gm"):
        raise HTTPException(status_code=403, detail="Insufficient role")
    with psycopg.connect(DATABASE_URL) as conn:
        with conn.cursor() as cur:
            cur.execute(
                "INSERT INTO account (email, pwd_hash, role) VALUES (%s,%s,%s) RETURNING id",
                (body.email, body.password_hash, body.role)
            )
            acc_id = cur.fetchone()[0]
            conn.commit()
            cur.execute(
                "INSERT INTO audit_log (actor_account, action, payload) VALUES (%s,%s,%s)",
                (uuid.UUID(claims["sub"]), "create_account", json.dumps({"new": acc_id}))
            )
            conn.commit()
            return {"id": str(acc_id)}

@app.post("/ban")
def ban(body: BanBody, claims=Depends(require_jwt)):
    if claims.get("role") not in ("admin", "gm", "mod"):
        raise HTTPException(status_code=403, detail="Insufficient role")
    expires = None
    if body.hours:
        expires = datetime.now(timezone.utc) + timedelta(hours=body.hours)
    with psycopg.connect(DATABASE_URL) as conn:
        with conn.cursor() as cur:
            cur.execute(
                "INSERT INTO ban_list (account_id, reason, expires_at) VALUES (%s,%s,%s) RETURNING id",
                (uuid.UUID(body.account_id), body.reason, expires)
            )
            ban_id = cur.fetchone()[0]
            conn.commit()
            cur.execute(
                "INSERT INTO audit_log (actor_account, action, payload) VALUES (%s,%s,%s)",
                (uuid.UUID(claims["sub"]), "ban", json.dumps({"ban_id": str(ban_id), "target": body.account_id}))
            )
            conn.commit()
            return {"ban_id": str(ban_id), "expires_at": expires.isoformat() if expires else None}

@app.post("/unban/{account_id}")
def unban(account_id: str, claims=Depends(require_jwt)):
    if claims.get("role") not in ("admin", "gm", "mod"):
        raise HTTPException(status_code=403, detail="Insufficient role")
    with psycopg.connect(DATABASE_URL) as conn:
        with conn.cursor() as cur:
            cur.execute("DELETE FROM ban_list WHERE account_id = %s", (uuid.UUID(account_id),))
            conn.commit()
            cur.execute(
                "INSERT INTO audit_log (actor_account, action, payload) VALUES (%s,%s,%s)",
                (uuid.UUID(claims["sub"]), "unban", json.dumps({"target": account_id}))
            )
            conn.commit()
            return {"ok": True}

@app.post("/mute")
def mute(body: MuteBody, claims=Depends(require_jwt)):
    if claims.get("role") not in ("admin", "gm", "mod"):
        raise HTTPException(status_code=403, detail="Insufficient role")
    expires = None
    if body.hours:
        expires = datetime.now(timezone.utc) + timedelta(hours=body.hours)
    with psycopg.connect(DATABASE_URL) as conn:
        with conn.cursor() as cur:
            cur.execute(
                "INSERT INTO mute_list (account_id, reason, expires_at) VALUES (%s,%s,%s) RETURNING id",
                (uuid.UUID(body.account_id), body.reason, expires)
            )
            mute_id = cur.fetchone()[0]
            conn.commit()
            cur.execute(
                "INSERT INTO audit_log (actor_account, action, payload) VALUES (%s,%s,%s)",
                (uuid.UUID(claims["sub"]), "mute", json.dumps({"mute_id": str(mute_id), "target": body.account_id}))
            )
            conn.commit()
            return {"mute_id": str(mute_id), "expires_at": expires.isoformat() if expires else None}

@app.post("/events/trigger")
def trigger_event(body: EventBody, claims=Depends(require_jwt)):
    if claims.get("role") not in ("admin", "gm"):
        raise HTTPException(status_code=403, detail="Insufficient role")
    # Write to Redis so the UE server can poll or subscribe
    evt = {"event": body.event, "region_id": body.region_id, "duration": body.duration_seconds}
    rds.publish("wuxia_events", json.dumps(evt))
    with psycopg.connect(DATABASE_URL) as conn:
        with conn.cursor() as cur:
            cur.execute(
                "INSERT INTO audit_log (actor_account, action, payload) VALUES (%s,%s,%s)",
                (uuid.UUID(claims["sub"]), "trigger_event", json.dumps(evt))
            )
            conn.commit()
    return {"ok": True}

@app.get("/logs/audit")
def audit_logs(offset:int=0, limit:int=50, claims=Depends(require_jwt)):
    if claims.get("role") not in ("admin", "gm", "mod", "viewer"):
        raise HTTPException(status_code=403, detail="Insufficient role")
    with psycopg.connect(DATABASE_URL) as conn:
        with conn.cursor() as cur:
            cur.execute("SELECT occurred_at, actor_account, action, payload FROM audit_log ORDER BY id DESC OFFSET %s LIMIT %s", (offset, limit))
            rows = cur.fetchall()
            data = []
            for (ts, actor, action, payload) in rows:
                data.append({"occurred_at": ts.isoformat(), "actor": str(actor) if actor else None, "action": action, "payload": payload})
            return {"data": data, "next_offset": offset + len(data)}

# Run: uvicorn main:app --host 0.0.0.0 --port 8000
# For local testing, set ADMIN_JWT_SECRET and create an admin account in Postgres manually or via /accounts/create after first admin login.
