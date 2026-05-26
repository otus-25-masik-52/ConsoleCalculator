# ConsoleCalculatorServer
A Linux TCP JSON service for executing safe integer calculations through `ConsoleCalculatorProtocol` and `ConsoleCalculatorMathLib`.

![Static Badge](https://img.shields.io/badge/otus--25--masik--52-ConsoleCalculatorServer-darkblue)
![GitHub top language](https://img.shields.io/github/languages/top/otus-25-masik-52/ConsoleCalculatorServer)
![GitHub](https://img.shields.io/github/license/otus-25-masik-52/ConsoleCalculatorServer)
![GitHub Repo stars](https://img.shields.io/github/stars/otus-25-masik-52/ConsoleCalculatorServer)

## Documentation

### Service overview

`ConsoleCalculatorServer` is a long-running Linux service.
It accepts calculation requests over TCP, parses them through the shared protocol library, executes calculations through the calculator core and sends protocol-compatible responses back to the client.

| Component | Responsibility |
|---|---|
| `calculator_server` | Long-running server executable |
| `ConsoleCalculatorProtocol` | Shared request/response models, status codes and JSON codec |
| `ConsoleCalculatorMathLib` | Safe arithmetic implementation |
| `Boost.Asio` | TCP networking |
| `StorageWorker` | Dedicated storage thread for PostgreSQL/cache access |
| `systemd` | Service lifecycle management |

This repository version is treated as a clean server application: there is one runtime executable, `calculator_server`.

### Runtime model

The service is split into separate execution areas:

| Thread area | Responsibility |
|---|---|
| Signal thread | Waits for `SIGINT` / `SIGTERM` and initiates graceful shutdown |
| Network worker threads | Accept TCP clients, read newline-delimited JSON and write responses |
| Storage worker thread | Owns cache and PostgreSQL-backed operation storage |

This model keeps network processing responsive and avoids unsynchronized concurrent access to the existing storage layer.

### Protocol

The server does not define its own JSON contract.
It uses `ConsoleCalculatorProtocol` as the single source of truth:

- `JsonCodec::parse_request`
- `JsonCodec::serialize_response`
- `CalculationRequest`
- `CalculationResponse`
- `OperationCode`
- `StatusCode`

Transport format:

```text
one JSON request + '\n'
one JSON response + '\n'
```

Example request:

```json
{"protocol_version":1,"request_id":"req-1","operation":"addition","operands":[1,2]}
```

Example response:

```json
{"protocol_version":1,"request_id":"req-1","status":{"code":0,"name":"OK"},"result":{"value":3},"error":null,"source":"calculated","timing":{"total_duration_ms":0}}
```

### systemd commands

Start service:

```bash
sudo systemctl start console-calculator-server
```

Restart service:

```bash
sudo systemctl restart console-calculator-server
```

Stop service:

```bash
sudo systemctl stop console-calculator-server
```

Check status:

```bash
sudo systemctl status console-calculator-server
```

### Configuration

Default config path:

```text
/etc/console-calculator/server.conf
```

Development config example:

```ini
host=0.0.0.0
port=5555
worker_threads=4
max_message_size=65536
log_file=/var/log/console-calculator/server.log

database_host=localhost
database_port=5432
database_name=console_calculator
database_user=admin
database_password_file=/run/secrets/cc_db_password
```

The server builds a `libpq` connection string from database fields.
A full explicit connection string may still be supplied through `database_connection_string` or `CC_DB_CONNECTION_STRING`, but the default development flow uses Docker secrets instead of committing a password.

Supported environment overrides:

| Variable | Meaning |
|---|---|
| `CC_SERVER_HOST` | Listen host |
| `CC_SERVER_PORT` | Listen port |
| `CC_SERVER_WORKER_THREADS` | Number of network worker threads |
| `CC_SERVER_MAX_MESSAGE_SIZE` | Maximum newline-delimited JSON message size |
| `CC_SERVER_LOG_FILE` | Log file path |
| `CC_DB_HOST` | PostgreSQL host |
| `CC_DB_PORT` | PostgreSQL port |
| `CC_DB_NAME` | PostgreSQL database name |
| `CC_DB_USER` | PostgreSQL user |
| `CC_DB_PASSWORD` | PostgreSQL password value |
| `CC_DB_PASSWORD_FILE` | Path to a file containing PostgreSQL password |
| `CC_DB_CONNECTION_STRING` | Full libpq connection string override |

### Development container

Docker files are kept under:

```text
.devcontainer/default/
```

The devcontainer Compose setup uses PostgreSQL secrets:

```yaml
POSTGRES_USER: admin
POSTGRES_PASSWORD_FILE: /run/secrets/cc_db_password
```

The root `.dockerignore` is still intentional because the Docker build context is the repository root:

```yaml
build:
  context: ../..
  dockerfile: .devcontainer/default/Dockerfile
```

Docker reads `.dockerignore` from the build context root, not from the directory where the Dockerfile lives.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## ThreadSanitizer

The service has an explicit TSan build option:

```bash
cmake -S . -B build-tsan \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCALCULATOR_SERVER_ENABLE_TSAN=ON

cmake --build build-tsan -j
ctest --test-dir build-tsan --output-on-failure
```

Signal handling itself is not unit-tested. The network request/response path is tested through a test TCP client.

## Manual service installation

Package generation is intentionally not enabled yet.
For local manual testing, build the binary and install the service files directly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
sudo cmake --install build --prefix /
sudo systemctl daemon-reload
```

Then start the service:

```bash
sudo systemctl start console-calculator-server
```

## Dependencies

The project is implemented in C++20 and uses:

- CMake
- Boost.Asio
- PostgreSQL `libpq`
- spdlog
- GoogleTest
- ConsoleCalculatorProtocol
- ConsoleCalculatorMathLib

## Support

If you have difficulties or questions about using this service, create an issue in this repository.

## Description of commits

All commits are expected to follow [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).
