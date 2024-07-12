#include "Pipe.h"

Pipe::Pipe(const std::string& name) {
   
}

void Pipe::Write(std::string data) {
    key_t key; 
    int msgid; 

    // ftok to generate unique key 
    key = ftok("progfile", 65); 

    // msgget creates a message queue 
    // and returns identifier 
    msgid = msgget(key, 0666 | IPC_CREAT); 
    _msg._type = 1; 

    strcpy(_msg._content, data.c_str()); 

    // msgsnd to send message 
    msgsnd(msgid, &_msg, sizeof(_msg), 0); 

    // display the message 
    printf("Data send is : %s \n", _msg._content); 
}