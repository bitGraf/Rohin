#include "System.hpp"

System::System(const char* _name, MessageBus* _msgBus) {
    nameLength = strlen(_name);
    name = (char*)malloc((nameLength+1)*sizeof(char));
    strcpy(name, _name);

    msgBus = _msgBus;
    Message msg(MessageType::standard, std::string(name));
    msgBus->_PostMessage(msg);
}

System::~System() {
    free(name);
    name = NULL;
    nameLength = 0;
}

void System::handleMessage(Message* msg) {
}

void System::update() {
    counter++;
}