FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
    curl \
    unzip \
    pkg-config \
    libpq-dev \
    postgresql-client \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd --gid 10001 ccu \
    && useradd --uid 10001 --gid 10001 --create-home --shell /bin/bash ccu

WORKDIR /cc_app

RUN rm -rf /cc_app/build /cc_app/cmake-build-debug /cc_app/cmake-build-release \
    && mkdir -p /cc_app/cc_build \
    && chown -R ccu:ccu /cc_app /cc_app/cc_build

COPY --chown=ccu:ccu . .

USER ccu:ccu

RUN cmake -S /cc_app -B /cc_app/cc_build -DCMAKE_BUILD_TYPE=Debug \
    && cmake --build /cc_app/cc_build -j

CMD ["bash"]