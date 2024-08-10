#pragma once

#include <string>

class NetworkConnection {
public:

    NetworkConnection();
    bool Connect(const char* address, int port);

    void SendStr(std::string str);
    void Send(const char* buffer, int len);
    void Close();

private:

    int _socket;
    bool _connected;

};

