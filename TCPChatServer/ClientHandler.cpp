#include "ClientHandler.h"
#include "MessageUtils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <winsock2.h>
#include <algorithm>

// Handle client commands
void ClientHandler::handleClient(SOCKET clientSocket, const std::string& clientMessage) {
    if (clientMessage == "~help") {
        processHelpCommand(clientSocket);
    }
    else if (clientMessage.find("~register") == 0) {
        processRegisterCommand(clientSocket, clientMessage);
    }
    else if (clientMessage.find("~login") == 0) {
        processLoginCommand(clientSocket, clientMessage);
    }
    else if (clientMessage.find("~logout") == 0) {
        processLogoutCommand(clientSocket);
    }
    else if (clientMessage.find("~getlist") == 0) {
        processGetListCommand(clientSocket);
    }
    else if (clientMessage.find("~getlog") == 0) {
        processGetLogCommand(clientSocket);
    }
    else if (clientMessage.find("~send") == 0) {
        processSendCommand(clientSocket, clientMessage);
    }
    else {
        relayMessageToAll(clientSocket, clientMessage);
    }
}

// User management functions
bool ClientHandler::registerUser(const std::string& username, const std::string& password) {
    if (users.find(username) == users.end()) {
        users[username] = password;
        return true;
    }
    return false;
}

bool ClientHandler::loginUser(const std::string& username, const std::string& password) {
    auto it = users.find(username);
    if (it != users.end() && it->second == password) {
        loggedInStatus[username] = true;
        return true;
    }
    return false;
}

bool ClientHandler::isUserLoggedIn(const std::string& username) {
    auto it = loggedInStatus.find(username);
    return it != loggedInStatus.end() && it->second;
}

void ClientHandler::logoutUser(const std::string& username) {
    loggedInStatus[username] = false;
}

// Function to process the ~help command
void ClientHandler::processHelpCommand(SOCKET clientSocket) {
    std::string helpMessage = "Available commands:\n"
        "~help: Show this help message\n"
        "~register <username> <password>: Register a new user\n"
        "~login <username> <password>: Login with your username and password\n"
        "~logout: Logout from the server\n"
        "~getlist: Get a list of active users\n"
        "~getlog: Retrieve the public message log\n"
        "~send <username> <message>: Send a direct message to a specific user\n"
        "~exit: Disconnect from the server\n";

    sendFramedMessage(clientSocket, helpMessage);
}

// Function to process the ~register command
void ClientHandler::processRegisterCommand(SOCKET clientSocket, const std::string& command) {
    size_t firstSpace = command.find(' ');
    size_t secondSpace = command.find(' ', firstSpace + 1);

    if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
        std::string errorMessage = "Error: Invalid register command. Use ~register <username> <password>\n";
        sendFramedMessage(clientSocket, errorMessage);
        return;
    }

    std::string username = command.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string password = command.substr(secondSpace + 1);

    // Normalize username to lowercase to ensure case-insensitivity
    std::transform(username.begin(), username.end(), username.begin(), ::tolower);

    if (registerUser(username, password)) {
        std::string successMessage = "Registration successful! You can now login using ~login <username> <password>.\n";
        sendFramedMessage(clientSocket, successMessage);
    }
    else {
        std::string errorMessage = "Error: Username already exists. Please choose another one.\n";
        sendFramedMessage(clientSocket, errorMessage);
    }
}

// Function to process the ~login command
void ClientHandler::processLoginCommand(SOCKET clientSocket, const std::string& command) {
    size_t firstSpace = command.find(' ');
    size_t secondSpace = command.find(' ', firstSpace + 1);

    if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
        std::string errorMessage = "Error: Invalid login command. Use ~login <username> <password>\n";
        sendFramedMessage(clientSocket, errorMessage);
        return;
    }

    std::string username = command.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string password = command.substr(secondSpace + 1);

    // Normalize username to lowercase to ensure case-insensitivity
    std::transform(username.begin(), username.end(), username.begin(), ::tolower);

    if (!isUserLoggedIn(username) && loginUser(username, password)) {
        std::string successMessage = "Login successful! Welcome, " + username + ".\n";
        sendFramedMessage(clientSocket, successMessage);
        clientSockets[username] = clientSocket;  // Store the client's socket
    }
    else if (isUserLoggedIn(username)) {
        std::string errorMessage = "Error: User already logged in.\n";
        sendFramedMessage(clientSocket, errorMessage);
    }
    else {
        std::string errorMessage = "Error: Invalid username or password.\n";
        sendFramedMessage(clientSocket, errorMessage);
    }
}

// Function to process the ~logout command
void ClientHandler::processLogoutCommand(SOCKET clientSocket) {
    for (auto& user : clientSockets) {
        if (user.second == clientSocket) {
            logoutUser(user.first);
            std::string message = "Logout successful. Goodbye, " + user.first + ".\n";
            sendFramedMessage(clientSocket, message);
            closesocket(clientSocket);
            clientSockets.erase(user.first);
            return;
        }
    }
    sendFramedMessage(clientSocket, "Error: No active session to log out.\n");
}

// Function to process the ~getlist command
void ClientHandler::processGetListCommand(SOCKET clientSocket) {
    std::string activeUsers = "Active users:\n";
    for (const auto& user : loggedInStatus) {
        if (user.second) {
            activeUsers += user.first + "\n";
        }
    }
    sendFramedMessage(clientSocket, activeUsers);
}

// Function to process the ~getlog command
void ClientHandler::processGetLogCommand(SOCKET clientSocket) {
    std::ifstream messageLog("message_log.txt");
    std::string line, logContent;
    while (std::getline(messageLog, line)) {
        logContent += line + "\n";
    }
    sendFramedMessage(clientSocket, logContent);
}

// Function to process the ~send command
void ClientHandler::processSendCommand(SOCKET clientSocket, const std::string& command) {
    size_t firstSpace = command.find(' ');
    size_t secondSpace = command.find(' ', firstSpace + 1);

    if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
        std::string errorMessage = "Error: Invalid send command. Use ~send <username> <message>\n";
        sendFramedMessage(clientSocket, errorMessage);
        return;
    }

    std::string targetUser = command.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string message = command.substr(secondSpace + 1);

    SOCKET targetSocket = findClientSocketByUsername(targetUser);
    if (targetSocket != INVALID_SOCKET) {
        sendFramedMessage(targetSocket, message);
    }
    else {
        std::string errorMessage = "Error: User not found.\n";
        sendFramedMessage(clientSocket, errorMessage);
    }
}

// Function to relay a message to all active clients
void ClientHandler::relayMessageToAll(SOCKET senderSocket, const std::string& message) {
    for (const auto& client : clientSockets) {
        if (client.second != senderSocket) {
            sendFramedMessage(client.second, message);
        }
    }
}

// Function to find a client socket by username
SOCKET ClientHandler::findClientSocketByUsername(const std::string& username) {
    auto it = clientSockets.find(username);
    if (it != clientSockets.end()) {
        return it->second;
    }
    else {
        return INVALID_SOCKET;
    }
}
