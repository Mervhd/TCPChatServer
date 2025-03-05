#pragma once
#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include <winsock2.h>
#include <string>

// Function to send framed messages
void sendFramedMessage(SOCKET clientSocket, const std::string& message);

#endif
