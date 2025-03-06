#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <winsock2.h>
#include <string>

class ChatServer {
public:
    // Constructor
    ChatServer();

    // Destructor
    ~ChatServer();

    // Setup the server
    SOCKET setupServer(int port, int chatCapacity, char commandChar);

    // Function to send a welcome message
    void sendWelcomeMessage(SOCKET clientSocket, char commandChar);

    // Function to receive a framed message
    std::string receiveFramedMessage(SOCKET clientSocket);

    // Function to broadcast server information
    void broadcastServerInfo(int port);

private:
    SOCKET udpSocket;  // UDP socket for broadcasting
    char commandChar; // Store command character
};

#endif 
