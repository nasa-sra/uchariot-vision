#pragma once

#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstdlib>
#include <cstring>

#include "Utils.h"

class MessageQueue {
public:
    MessageQueue(const std::string &name);
    ~MessageQueue();
    void Write(std::string data);

private:
    key_t _key;
    const char *_name;
    int _msgid;
};
