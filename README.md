# c-bindings-examples
Some examples on how to call C functions with Go and Python

## Go

### Debian
docker build -t go-status-fetcher .
docker run --rm go-status-fetcher

### Alpine
docker build -f Dockerfile.alpine -t go-status-fetcher-alpine .
docker run --rm go-status-fetcher-alpine

## Py

### Alpine
docker build -f Dockerfile.python.alpine -t py-status-fetcher-alpine .
docker run --rm py-status-fetcher-alpine
