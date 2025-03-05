#include "User.h"
#include <unordered_map>

// Function to register a new user
bool User::registerUser(const std::string& username, const std::string& password) {
    // Check if the username is already in use
    if (userTable.find(username) == userTable.end()) {
        // If not, add the username and password to the userTable
        userTable[username] = password;

        // Set the initial login status to false
        loggedInStatus[username] = false;

        // Registration successful
        return true;
    }

    // Registration failed (username already exists)
    return false;
}

// Function to log in an existing user
bool User::loginUser(const std::string& username, const std::string& password) {
    // Check if the username exists and the password matches
    if (userTable.find(username) != userTable.end() && userTable[username] == password) {
        // Set the user's login status to true
        loggedInStatus[username] = true;

        // Login successful
        return true;
    }

    // Login failed (incorrect username or password)
    return false;
}

// Function to log out a user
void User::logoutUser(const std::string& username) {
    // Set the user's login status to false
    loggedInStatus[username] = false;
}

// Function to check if a user is logged in
bool User::isUserLoggedIn(const std::string& username) {
    // Return the login status of the user
    return loggedInStatus[username];
}
