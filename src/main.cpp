#include <stdio.h>
#include <string.h>

#include "types.hpp"
#include "System.hpp"
#include "MessageBus.hpp"

const int FPS = 1;

int main(int argc, char** argv) {
    printf("%s was run with %d arguments\n", argv[0], argc-1);

    for (int i = 1; i < argc; i++) {
        printf("[%d] %s {%d}\n", i, argv[i], (int)strlen(argv[i]));
    }
    printf("\n\n");

    printf("Creating message bus\n");
    MessageBus msgBus;

    printf("\nCreating subsystems:\n");
    System Input("Input", &msgBus), Render("Render", &msgBus);

    int frame_number = 1;

    msgBus.processEntireQueue();
    printf("\nStarting message loop\n");
    bool done = false;
    while (!done) {
        printf("[%-3d] ", frame_number++);
        msgBus.PopQueue();

        Input.update();
        Render.update();

        if (Input.counter == 18) {
            msgBus.PostMessageByType(MessageType::type2);
            msgBus.PostMessageByType(MessageType::empty);
            msgBus.PostMessageByType(MessageType::type2);
            msgBus.PostMessageByType(MessageType::empty);
            msgBus.PostMessageByType(MessageType::type2);
            msgBus.PostMessageByType(MessageType::empty);
            msgBus.PostMessageByType(MessageType::empty);
            msgBus.PostMessageByType(MessageType::type2);
        }

        if (Input.counter >= 10 && !msgBus.hasMessages) {
            done = true;
        }
    }

    printf("\nEnd of message loop\n");

    system("pause");
    return(0);
}