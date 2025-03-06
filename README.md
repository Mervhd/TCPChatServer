# TCP Chat Server

A robust TCP-based chat server built in C++ using Winsock, developed as an assignment for my Bachelor’s Degree in Computer Science at Full Sail University. This project showcases my skills in network programming, socket communication, and user management, enabling multiple clients to connect, authenticate, and chat in real-time.

## Features
- **Multi-Client Support:** Handles multiple simultaneous connections with a configurable capacity.
- **User Authentication:** Register and login with `~register <username> <password>` and `~login <username> <password>`.
- **Direct Messaging:** Send private messages via `~send <username> <message>`.
- **Broadcasting:** Periodic UDP announcements of server availability.
- **Command System:** Intuitive commands like `~help`, `~getlist` (active users), `~getlog` (chat history), and `~logout`.
- **Message Logging:** Public messages are logged to `message_log.txt`.

## Technologies
- **C++:** Core language with object-oriented design.
- **Winsock2:** Windows-specific socket API for TCP and UDP networking.
- **STL:** Uses `std::unordered_map`, `std::vector`, and `std::string` for efficient data handling.
- **Multithreading:** Employs `std::thread` for UDP broadcasting.

## Prerequisites
- **Operating System:** Windows (due to Winsock dependency).
- **Compiler:** C++ compiler (e.g., MSVC via Visual Studio, MinGW).
- **Library:** Winsock library (`Ws2_32.lib`).

## Installation
1. **Clone the Repository:**
   ```bash
   https://github.com/Mervhd/TCPChatServer.git
