// Dawid Szarek gh: Razeeeee
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    int status;
    char* arg[] = {"./program", "Dawid Szarek", NULL};
    
    pid = fork();
    
    if (pid == -1) {
        printf("Blad podczas tworzenia procesu potomnego\n");
        return 1;
    }
    else if (pid == 0) {
        // Proces potomny
        int wynik = execv("./program", arg);
        if (wynik == -1) {
            printf("Proces potomny nie wykonal programu\n");
            return 1;
        }
    }
    else {
        wait(NULL);
    }
    
    return 0;
}