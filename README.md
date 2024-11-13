# Simple Chat Server in C

A basic multi-client chat server built using socket programming in C. The server listens for incoming client connections, handles each client in a separate thread, and broadcasts messages from any client to all other connected clients.

## Features

- Supports multiple clients (up to 10 by default).
- Each client’s messages are broadcasted to all other connected clients.
- Uses pthreads for concurrency, handling each client connection in a separate thread.
- Demonstrates the basics of socket programming and threading in C.

## Prerequisites

- A C compiler (e.g., `gcc`)
- POSIX-compliant environment (Linux, macOS)
- Basic knowledge of networking and C programming

## Installation

1. Clone this repository (if stored on a VCS) or copy the `chat_server.c` file.
2. Compile the code with the following command:

   ```sh
   gcc main.c -o chat_server -lpthread
