
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> 

#include "Utils.h"
#include "NetworkConnection.h"

NetworkConnection::NetworkConnection() {}

bool NetworkConnection::Connect(const char* address, int port) {

    _socket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(AF_INET, address, &serverAddress.sin_addr);
  
    Utils::LogFmt("Connecting to %s:%i", address, port);
    if (connect(_socket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
        Utils::LogFmt("NetworkConnection::Connect failed to connect");
        close(_socket);
        return false;
    }
    Utils::LogFmt("Connected");


    _connected = true;
    return true;
}

void NetworkConnection::SendStr(std::string str) {
    Send(str.data(), str.size());
}

void NetworkConnection::Send(const char* buffer, int len) {
    if (!_connected) {return;}

    int total = 0;
    int bytesleft = len;
    int n;
    while (total < len) {
        n = send(_socket, buffer + total, bytesleft, 0);
        if (n == -1) {
            Utils::LogFmt("NetworkConnection - Error on send");
            return;
        }
        total += n;
        bytesleft -= n;
    }
}

void NetworkConnection::Close() {
    close(_socket);
}