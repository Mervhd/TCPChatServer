#include "ChatServer.h"
#include "MessageUtils.h"
#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")  // Link Winsock library

// Constructor
ChatServer::ChatServer() : udpSocket(INVALID_SOCKET), commandChar('~') {} // Default command char ~

// Destructor
ChatServer::~ChatServer() {
    if (udpSocket != INVALID_SOCKET) {
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;
    }
}

// Function to set up the server
SOCKET ChatServer::setupServer(int port, int chatCapacity, char commandChar) {
    WSADATA wsaData;
    this->commandChar = commandChar; // Store command Character

    // Initialize Winsock
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupResult != 0) {
        std::cerr << "Error: WSAStartup failed with error " << wsaStartupResult << std::endl;
        return INVALID_SOCKET;
    }

    struct sockaddr_in serverAddress;
    SOCKET serverSocket;

    // Configure the server address structure for IPv4
    serverAddress.sin_family = AF_INET;  // IPv4 address family
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // Bind to any available network interface
    serverAddress.sin_port = htons(port);  // Set port and convert to network byte order

    // Create the server socket (IPv4, TCP)
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error: Failed to create socket with error " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Bind the socket to the specified port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        std::cerr << "Error: Failed to bind socket with error code " << errorCode << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, chatCapacity) == SOCKET_ERROR) {
        std::cerr << "Error: Failed to listen on socket with error " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    std::cout << "Server setup complete." << std::endl;
    std::cout << "Listening on port " << port << std::endl;

    // Setup UDP socket for broadcasting
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Warning: Failed to create UDP socket with error " << WSAGetLastError() << std::endl;
        return serverSocket;
    }

    // Enable broadcast on the UDP socket
    BOOL broadcastEnabled = TRUE;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnabled, sizeof(broadcastEnabled)) == SOCKET_ERROR) {
        std::cerr << "Warning: Failed to enable broadcast on UDP socket with error " << WSAGetLastError() << std::endl;
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET; // Reset to invalid
        return serverSocket;
    }

    return serverSocket;
}

// Function to send a welcome message
void ChatServer::sendWelcomeMessage(SOCKET clientSocket, char commandChar) {
    std::string welcomeMessage = "Welcome to the server! Use '" + std::string(1, commandChar) + "' for commands.\n";
    sendFramedMessage(clientSocket, welcomeMessage);
}

// Function to receive a framed message
std::string ChatServer::receiveFramedMessage(SOCKET clientSocket) {
    uint8_t messageLength;

    // Receive the length of the message (1 byte)
    int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&messageLength), sizeof(messageLength), 0);
    if (bytesReceived <= 0) {
        std::cerr << "Error: Failed to receive message length." << std::endl;
        return "";
    }

    std::vector<char> buffer(messageLength);  // Vector to handle message storage without manual memory management

    int totalBytesReceived = 0;

    // Keep receiving until the full message is received
    while (totalBytesReceived < messageLength) {
        bytesReceived = recv(clientSocket, buffer.data() + totalBytesReceived, messageLength - totalBytesReceived, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error: Failed to receive full message content." << std::endl;
            return "";
        }
        totalBytesReceived += bytesReceived;
    }

    return std::string(buffer.begin(), buffer.end());
}

// Function to broadcast server information
void ChatServer::broadcastServerInfo(int port) {
    struct sockaddr_in broadcastAddr;
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(port);
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    std::string broadcastMessage = "Server is running on port " + std::to_string(port);
    int broadcastResult = sendto(udpSocket, broadcastMessage.c_str(), broadcastMessage.length(), 0,
        (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    if (broadcastResult < 0) {
        std::cerr << "Error: Failed to send broadcast message with error " << WSAGetLastError() << std::endl;
    }
}
