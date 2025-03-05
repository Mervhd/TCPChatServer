#include "MessageUtils.h"
#include <winsock2.h>

// Definition of the sendFramedMessage function
void sendFramedMessage(SOCKET clientSocket, const std::string& message) {
    uint8_t messageLength = static_cast<uint8_t>(message.size());  // Get the length of the message (1 byte)

    // Send the length of the message
    send(clientSocket, reinterpret_cast<const char*>(&messageLength), sizeof(messageLength), 0);

    // Send the actual message
    send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);
}
