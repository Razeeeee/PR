// Dawid Szarek gh: Razeeeee

// wypisanie arg1
#include <stdio.h>

int main(int argc, char *argv[])
{
    if(argc > 1) {
        printf("Argument 1: %s\n", argv[1]);
    } else {
        printf("Brak argumentu 1\n");
    }
    return 0;
}