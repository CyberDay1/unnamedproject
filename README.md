# unnamedproject
## Persistence subsystem

- HTTP endpoints default to `https://example.com/api/saveProfile` and `https://example.com/api/loadProfile`. Update these URLs for your deployment or override them at runtime before issuing requests.
- Inject a production-ready 32-byte AES key via `UPersistenceSubsystem::SetAESKey` on startup. The built-in default is zeroed and intended only for local testing.
- Cooldown snapshots older than one hour are discarded automatically during load to avoid reviving expired action locks.
