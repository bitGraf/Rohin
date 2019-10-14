#include "System.hpp"

System::System(const char* _name) {
    nameLength = strlen(_name);
    name = (char*)malloc((nameLength+1)*sizeof(char));
    strcpy(name, _name);

    printf("System [%s] created.\n", name);
}

System::~System() {
    free(name);
    name = NULL;
    nameLength = 0;
}

void System::handleMessage() {
    printf("%s::handleMessage()\n", name);
}

void System::update() {
    counter++;
    printf("%s::update() #%d\n", name, counter);
}