#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("%s was run with %d arguments", argv[0], argc-1);

    for (int i = 0; i < argc; i++) {
        printf("[%d] %s {%d}\n", i, argv[i], (int)strlen(argv[i]));
    }

    return(0);
}