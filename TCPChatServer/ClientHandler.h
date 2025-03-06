#pragma once
#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <string>
#include <unordered_map>
#include <winsock2.h>

class ClientHandler {
public:
    // Singleton patter
    static ClientHandler& getInstance();

    // Function to handle client commands
    bool handleClient(SOCKET clientSocket, const std::string& clientMessage);

private:
    // Private constructor for singleton
    ClientHandler() {}

    // User management functions
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    bool isUserLoggedIn(const std::string& username);
    void logoutUser(const std::string& username);

    // Map to store client sockets associated with usernames
    std::unordered_map<std::string, SOCKET> clientSockets;

    // Map to store users and their login status
    std::unordered_map<std::string, bool> loggedInStatus;

    // Map to store users and their passwords
    std::unordered_map<std::string, std::string> users;

    // Function to process the ~help command
    void processHelpCommand(SOCKET clientSocket);

    // Function to process the ~register command
    void processRegisterCommand(SOCKET clientSocket, const std::string& command);

    // Function to process the ~login command
    void processLoginCommand(SOCKET clientSocket, const std::string& command);

    // Function to process the ~logout command
    bool processLogoutCommand(SOCKET clientSocket); // Only one parameter

    // Function to process the ~getlist command
    void processGetListCommand(SOCKET clientSocket);

    // Function to process the ~getlog command
    void processGetLogCommand(SOCKET clientSocket);

    // Function to process the ~send command
    void processSendCommand(SOCKET clientSocket, const std::string& command);

    // Function to relay a message to all active clients
    void relayMessageToAll(SOCKET senderSocket, const std::string& message);

    // Function to find a client socket by username
    SOCKET findClientSocketByUsername(const std::string& username);
};

#endif 