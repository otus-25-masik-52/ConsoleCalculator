FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    ca-certificates \
    curl \
    unzip \
    pkg-config \
    libpq-dev \
    postgresql-client \
    libboost-all-dev \
    valgrind \
    gdb \
    openssh-server \
    openjdk-21-jre-headless \
    iproute2 \
    procps \
    sudo \
    && rm -rf /var/lib/apt/lists/*

ARG DEV_UID=1000
ARG DEV_GID=1000
ARG DEV_USER=ccsu

RUN set -eux; \
    OLD_USER="$(getent passwd "${DEV_UID}" | cut -d: -f1 || true)"; \
    OLD_GROUP="$(getent group "${DEV_GID}" | cut -d: -f1 || true)"; \
    \
    if [ -n "${OLD_GROUP}" ] && [ "${OLD_GROUP}" != "${DEV_USER}" ]; then \
        groupmod -n "${DEV_USER}" "${OLD_GROUP}"; \
    elif [ -z "${OLD_GROUP}" ]; then \
        groupadd --gid "${DEV_GID}" "${DEV_USER}"; \
    fi; \
    \
    if [ -n "${OLD_USER}" ] && [ "${OLD_USER}" != "${DEV_USER}" ]; then \
        usermod -l "${DEV_USER}" "${OLD_USER}"; \
        usermod -d "/home/${DEV_USER}" -m "${DEV_USER}"; \
        usermod -g "${DEV_GID}" "${DEV_USER}"; \
    elif [ -z "${OLD_USER}" ]; then \
        useradd \
            --uid "${DEV_UID}" \
            --gid "${DEV_GID}" \
            --create-home \
            --shell /bin/bash \
            "${DEV_USER}"; \
    fi; \
    \
    echo "${DEV_USER}:dev_container_password_disabled" | chpasswd; \
    mkdir -p \
        "/home/${DEV_USER}/.ssh" \
        /ccs_app \
        /ccs_build \
        /run/sshd \
        /var/log/console-calculator \
        /var/lib/console-calculator; \
    chown -R "${DEV_UID}:${DEV_GID}" \
        "/home/${DEV_USER}" \
        /ccs_app \
        /ccs_build \
        /var/log/console-calculator \
        /var/lib/console-calculator; \
    chmod 700 "/home/${DEV_USER}/.ssh"; \
    chmod 0755 /run/sshd; \
    chmod 0755 /var/log/console-calculator; \
    chmod 0755 /var/lib/console-calculator


RUN sed -i 's/^#\?PasswordAuthentication .*/PasswordAuthentication no/' /etc/ssh/sshd_config \
    && sed -i 's/^#\?PermitRootLogin .*/PermitRootLogin no/' /etc/ssh/sshd_config \
    && sed -i 's/^#\?PubkeyAuthentication .*/PubkeyAuthentication yes/' /etc/ssh/sshd_config \
    && sed -i 's/^#\?AuthorizedKeysFile .*/AuthorizedKeysFile .ssh\/authorized_keys/' /etc/ssh/sshd_config \
    && sed -i 's/^#\?UsePAM .*/UsePAM no/' /etc/ssh/sshd_config

WORKDIR /ccs_app

EXPOSE 22
