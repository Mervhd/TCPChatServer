#pragma once
#ifndef USER_H
#define USER_H

#include <unordered_map>
#include <string>

class User {
public:
    // Map to store username and password pairs
    std::unordered_map<std::string, std::string> userTable;

    // Map to track the login status of each user
    std::unordered_map<std::string, bool> loggedInStatus;

    // Function to register a new user
    bool registerUser(const std::string& username, const std::string& password);

    // Function to log in an existing user
    bool loginUser(const std::string& username, const std::string& password);

    // Function to log out a user
    void logoutUser(const std::string& username);

    // Function to check if a user is logged in
    bool isUserLoggedIn(const std::string& username);
};

#endif 
