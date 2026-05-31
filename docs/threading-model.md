# Threading model

`ConsoleCalculatorServer` uses three independent execution areas:

1. Signal thread waits for `SIGINT` and `SIGTERM` with `sigwait`.
2. Boost.Asio worker threads handle TCP sessions and protocol request/response exchange.
3. Storage worker thread owns PostgreSQL/cache access and serializes storage jobs.

The storage layer from the original CLI project was not designed as a concurrent repository API. For this reason the service does not share `StorageService` directly between network threads. Network threads submit storage jobs to `StorageWorker`; the worker executes them sequentially on its own thread.
