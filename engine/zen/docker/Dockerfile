# Dockerfile para Zen sobre Zen — compilador self-hosted
# Evita que procesos colgados tiren la PC completa
FROM ubuntu:22.04

# Evitar prompts interactivos
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependencias de build
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    llvm-14 \
    libllvm14 \
    llvm-14-dev \
    liblld-14-dev \
    lld-14 \
    git \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Directorio de trabajo
WORKDIR /zen

# Script de entrypoint
COPY run_zen.sh /run_zen.sh
RUN chmod +x /run_zen.sh

ENTRYPOINT ["/run_zen.sh"]
