#include "MessageQueue.h"

MessageQueue::MessageQueue(const std::string& name) {
    _name = name.c_str();

    // ftok to generate unique key 
    _key = ftok(_name, 65); 

    // msgget creates a message queue 
    // and returns identifier 
    _msgid = msgget(_key, 0666 | IPC_CREAT);
    if (_msgid == -1) {
        Utils::LogFmt("MessageQueue Failed to create message queue Error: %s", strerror(errno));
    }
}

MessageQueue::~MessageQueue() {
    msgctl(_msgid, IPC_RMID, NULL); 
}

void MessageQueue::Write(std::string data) {

    struct msqid_ds ms_data;
    msgctl(_msgid, IPC_STAT, &ms_data);
    if (ms_data.msg_qnum > 4) {
        // clear buffer
        char buf[2560];
        for (int i = 0; i < ms_data.msg_qnum; i++) {
            int bytes = msgrcv(_msgid, &buf, sizeof(buf), 1, IPC_NOWAIT);
            if (bytes == -1) {
                Utils::LogFmt("MessageQueue::Write - Error flushing -  %s", strerror(errno));
            }
            // std::cout << "Flushed " << bytes << "bytes\n";
        }
    }

    // fill send buffer
    long msgType = 1; 
    size_t bufferLen = sizeof(long) + strlen(data.c_str());
    char* sendBuffer = new char[bufferLen];
    memcpy(sendBuffer, &msgType, sizeof(long));
    strcpy(sendBuffer+sizeof(long), data.c_str());

    // msgsnd to send message 
    int res = msgsnd(_msgid, sendBuffer, bufferLen, 0);
    if (res == -1)
            Utils::LogFmt("MessageQueue::Write - Error sending -  %s", strerror(errno));

    delete[] sendBuffer;

    // Utils::LogFmt("Sent %i bytes Queue Len %i Message Data: %s", bufferLen, ms_data.msg_qnum, data);
}