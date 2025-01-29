# qtp
Quantum Transfer Protocol POC-A


# Full-Duplex WebSocket Server for SudoCoinXRPL

## Overview

This project provides a high-performance full-duplex WebSocket server implemented in C++ using Boost.Asio and Boost.Beast. It is designed to support real-time bidirectional communication, making it ideal for use cases such as blockchain interactions, decentralized applications, and financial data streaming.

## Features

- Full-duplex WebSocket communication
- Thread-safe client management
- Supports multiple concurrent clients
- Verbose debugging mode for enhanced logging
- Configurable port through command-line arguments
- Uses Boost.Asio for efficient asynchronous networking
- Implements JSON support using `nlohmann/json`

## Dependencies

Ensure that the following dependencies are installed before compiling the project:

- **Boost Libraries** (Boost.Asio, Boost.Beast)
- **nlohmann/json** (for JSON parsing)
- **C++17 or later** (for standard language features)

## Installation

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y libboost-all-dev
```

### macOS

```bash
brew install boost
```

### Windows

Download and install Boost from: [Boost Official Site](https://www.boost.org/)

## Compilation

Use `g++` or `clang++` to compile the server:

```bash
g++ -std=c++17 -pthread -o websocket_server main.cpp -lboost_system -lboost_thread -lboost_beast
```

## Usage

### Running the Server

```bash
./websocket_server --port 1111 --verbose
```

- `--port <PORT>`: Set the port number (default is `1111`).
- `--verbose`: Enable verbose logging mode for debugging.

### Example WebSocket Client (JavaScript)

```javascript
const ws = new WebSocket('ws://localhost:1111');

ws.onopen = () => {
    console.log('Connected to server');
    ws.send('Hello, Server!');
};

ws.onmessage = (event) => {
    console.log('Received:', event.data);
};

ws.onclose = () => {
    console.log('Connection closed');
};
```

## Code Structure

- **`FullDuplexServer`**** Class**: Handles WebSocket connections, message passing, and client management.
- **`handleConnection()`**: Accepts new client connections.
- **`startReadLoop()`**: Reads and processes incoming messages.
- **`sendMessage()`**: Sends messages back to clients asynchronously.
- **`log()`**: Handles verbose debugging output.

## Contributions

We welcome contributions! Please fork the repository, make changes, and submit a pull request.

## License

This project is licensed under the MIT License. See `LICENSE` for details.

fs
