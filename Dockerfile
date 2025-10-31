# ===============================================================
# Moonlit River (月照江流) — Dedicated Server Runtime (prebuilt)
# Unreal Engine 5.6.1 | Ubuntu 22.04 base
# ===============================================================

FROM ubuntu:22.04 AS runtime
LABEL maintainer="Connor Murphy <connor.murphy@umarexusa.com>"
LABEL project="Moonlit River (月照江流)"

# Environment setup
ENV DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8 \
    UE_PROJECT_NAME=MoonlitRiver \
    UE_BINARY_PATH=/app/Binaries/Linux/MoonlitRiverServer \
    WUXIA_AES_KEY_32_BASE64="" \
    DB_HOST=postgres \
    DB_PORT=5432 \
    DB_NAME=wuxia_db \
    DB_USER=wuxia \
    DB_PASS=wuxia_pw \
    REDIS_HOST=redis \
    REDIS_PORT=6379

# Install minimal runtime dependencies for UE server
RUN apt-get update && apt-get install -y --no-install-recommends \
    libicu70 \
    libssl3 \
    libpq5 \
    libxi6 \
    libxrandr2 \
    libxss1 \
    libxkbcommon0 \
    libxcomposite1 \
    libxdamage1 \
    libxinerama1 \
    libglu1-mesa \
    libnss3 \
    ca-certificates \
    tzdata && \
    rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy only needed assets
COPY Binaries/Linux/MoonlitRiverServer /app/Binaries/Linux/MoonlitRiverServer
COPY Config /app/Config
COPY Content/Localization /app/Content/Localization
COPY Saved/Snapshots /app/Saved/Snapshots

# Permissions
RUN chmod +x /app/Binaries/Linux/MoonlitRiverServer

# Volumes for persistence
VOLUME ["/app/Logs", "/app/Saved", "/app/Saved/Snapshots"]

# Expose game port and optional admin API port
EXPOSE 7777/udp
EXPOSE 8000

# Health check (responds if log file updated recently)
HEALTHCHECK --interval=60s --timeout=5s --start-period=60s CMD \
    find /app/Logs -type f -mmin -2 | grep .log >/dev/null || exit 1

# Entry point
ENTRYPOINT ["/app/Binaries/Linux/MoonlitRiverServer", "-log"]
