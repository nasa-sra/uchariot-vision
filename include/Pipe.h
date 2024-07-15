#pragma once

#include <iostream>
#include <string>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <cstdlib>
#include <cstring>

#include "Utils.h"

struct MsgBuffer { 
	long _type; 
	char _content[16]; 
}; 

class Pipe {
    const char* name;
    MsgBuffer _msg;

public:
    Pipe(const std::string& name);
    void Write(std::string data);
};

