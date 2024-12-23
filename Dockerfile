# Use a base image with Go and necessary build tools
FROM golang:1.20-buster AS build

# Install dependencies for building the C code
RUN apt-get update && apt-get install -y \
    curl \
    gcc \
    libc6-dev \
    libcurl4-openssl-dev \
    libpthread-stubs0-dev

# Set the working directory
WORKDIR /app

# Copy the C and Go code into the container
COPY status_fetcher.c .
COPY main.go .

# Compile the C code into a shared library
RUN gcc -shared -o libstatus_fetcher.so -fPIC status_fetcher.c -lcurl -lpthread

# Build the Go application
RUN CGO_ENABLED=1 go build -o status_fetcher main.go

# Use a smaller base image for the final stage
FROM debian:buster-slim

# Install runtime dependencies for the application
RUN apt-get update && apt-get install -y \
    libcurl4 \
    libc6

# Set the working directory
WORKDIR /app

# Copy the compiled shared library and binary from the build stage
COPY --from=build /app/libstatus_fetcher.so .
COPY --from=build /app/status_fetcher .

# Set the environment variable for the shared library
ENV LD_LIBRARY_PATH=/app

# Run the Go application
CMD ["./status_fetcher"]
