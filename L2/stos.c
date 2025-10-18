// Dawid Szarek gh: Razeeeee

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>

// Test stosu wątku - stworzyć stos o różnych rozmiarach dla tablicy char mającej 16 elementów

int funkcja_watku(void *argument)
{
    // wypisanie stosu
    char *tablica = (char *)argument;
    if (!tablica) {
        printf("funkcja_watku: otrzymano NULL\n");
        return 1;
    }
    for (int i = 0; i < 16; i++)
    {
        printf("tablica[%d] = %c\n", i, tablica[i]);
    }
    return 0;
}

int main()
{
    void *stos;
    pid_t pid;
    int i;

    char tablica[16];
    for (int j = 0; j < 16; j++)
    {
        tablica[j] = 'A' + j;
    }

    for (i = 1; i <= 1024; i++)
    {
        int size = i;
        stos = malloc(size);
        if (!stos) {
            perror("malloc");
            continue;
        }

        void *arg_ptr = stos;
        if (size >= (int)sizeof(tablica)) {
            arg_ptr = (char *)stos + size - sizeof(tablica);
            memcpy(arg_ptr, tablica, sizeof(tablica));
        } else {
        }

        void *child_stack = (char *)stos + size;
        child_stack = (void *)((uintptr_t)child_stack & ~0xF);

        int flags = CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM | SIGCHLD;
        pid = clone(funkcja_watku, child_stack, flags, arg_ptr);
        if (pid == -1) {
            printf("i=%d: clone nie powiódł się: %s\n", i, strerror(errno));
            free(stos);
            continue;
        } else {
            printf("i=%d: clone ok, pid=%d\n", i, pid);
        }

        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
        }

        free(stos);
    }
    return 0;
}
