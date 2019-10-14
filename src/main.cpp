#include <stdio.h>
#include <string.h>

#include "types.hpp"
#include "System.hpp"

int main(int argc, char** argv) {
    printf("%s was run with %d arguments", argv[0], argc-1);

    for (int i = 0; i < argc; i++) {
        printf("[%d] %s {%d}\n", i, argv[i], (int)strlen(argv[i]));
    }

    printf("Creating message bus\n");

    printf("Creating subsystems:\n\n");
    System Input("Input"), Render("Render");

    Input.handleMessage();

    Input.update();
    Input.update();
    Input.update();
    Render.update();
    Input.update();
    Render.update();

    return(0);
}