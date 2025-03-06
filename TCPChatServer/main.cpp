#include "ChatServer.h"
#include "ClientHandler.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")  // Link Winsock library

std::atomic<bool> keepBroadcasting(true);

void broadcastServerInfo(ChatServer& server, int port) {
    while (keepBroadcasting) {
        server.broadcastServerInfo(port);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

int main() {
    ChatServer server;
    int port, chatCapacity;
    char commandChar;

    // Get user input for setup
    std::cout << "Enter server port: ";
    std::cin >> port;
    std::cout << "Enter chat capacity: ";
    std::cin >> chatCapacity;
    std::cout << "Enter command character (default ~): ";
    std::cin >> commandChar;

    // Setup the server
    SOCKET serverSocket = server.setupServer(port, chatCapacity, commandChar);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Server setup failed!" << std::endl;
        return -1;
    }

    // Start broadcasting server information
    std::thread broadcastThread(broadcastServerInfo, std::ref(server), port);
    broadcastThread.detach();

    std::cout << "Server is running..." << std::endl;

    // Initialize master set and add the server socket
    fd_set masterSet, readfds;
    FD_ZERO(&masterSet);
    FD_SET(serverSocket, &masterSet);
    SOCKET max_sd = serverSocket;

    int activeClients = 0; // Track the number of connected clients

    while (true) {
        readfds = masterSet;

        int activity = select(static_cast<int>(max_sd) + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "Error: select() failed." << std::endl;
            break;
        }

        // Check for new connections
        if (FD_ISSET(serverSocket, &readfds)) {
            struct sockaddr_in clientAddr;
            int clientAddrLen = sizeof(clientAddr);
            SOCKET newClientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

            if (newClientSocket != INVALID_SOCKET) {
                if (activeClients >= chatCapacity) {
                    std::cerr << "Maximum client capacity reached. New connection refused." << std::endl;
                    // Send a message to the client explaining that the server is full, then close the connection
                    std::string fullMessage = "Server is full. Try again later.\n";
                    send(newClientSocket, fullMessage.c_str(), static_cast<int>(fullMessage.length()), 0); // Cast length to int
                    closesocket(newClientSocket);
                }
                else {
                    std::cout << "New client connected!" << std::endl;
                    server.sendWelcomeMessage(newClientSocket, commandChar);

                    FD_SET(newClientSocket, &masterSet);
                    if (newClientSocket > max_sd) {
                        max_sd = newClientSocket;
                    }
                    activeClients++; // Increment active client count
                }
            }
        }

        // Check all clients for incoming data
        for (SOCKET i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &readfds) && i != serverSocket) {
                std::string message = server.receiveFramedMessage(i);
                if (!message.empty()) {
                    std::cout << "Message received from client: " << message << std::endl;
                    ClientHandler& handler = ClientHandler::getInstance();
                    if (handler.handleClient(i, message)) {
                        FD_CLR(i, &masterSet);
                        activeClients--;
                    }
                    else {
                        // Connection closed by the client
                        std::cout << "Client disconnected." << std::endl;
                        closesocket(i);
                        FD_CLR(i, &masterSet);
                        activeClients--; // Decrement active client count
                    }
                }
            }
        }
    }

    keepBroadcasting = false; // Stop Broadcasting
    // Clean up Winsock when done
    WSACleanup();

    return 0;
}
