#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#include"../L1/pomiar_czasu.h"

#define ROZMIAR_STOSU 1024*64

int zmienna_globalna = 0;

// Zmienne statyczne dla lokalnego pomiaru czasu
static clock_t ct_lokalny;
static struct rusage rp_lokalny;
static struct timeval tp_lokalny;

// Funkcja do inicjalizacji pomiaru czasu
void inicjuj_czas_lokalny() {
    ct_lokalny = clock();
    getrusage(RUSAGE_SELF, &rp_lokalny);
    gettimeofday(&tp_lokalny, NULL);
}

// Funkcja do poprawnego pomiaru czasu standardowego
double czas_C_poprawny() {
    clock_t czas_aktualny = clock();
    if (czas_aktualny == -1) return -1.0;
    
    return (double)(czas_aktualny - ct_lokalny) / (double)CLOCKS_PER_SEC;
}

// Funkcja do poprawnego pomiaru czasu CPU
double czas_CPU_poprawny() {
    struct rusage rk;
    double cputime;
    
    getrusage(RUSAGE_SELF, &rk);
    
    cputime = (rk.ru_utime.tv_usec - rp_lokalny.ru_utime.tv_usec) / 1e6;
    cputime += rk.ru_utime.tv_sec - rp_lokalny.ru_utime.tv_sec;
    
    return cputime;
}

// Funkcja do poprawnego pomiaru czasu zegarowego
double czas_zegara_poprawny() {
    struct timeval tk;
    double daytime;
    
    gettimeofday(&tk, NULL);
    
    daytime = (tk.tv_usec - tp_lokalny.tv_usec) / 1e6 + tk.tv_sec - tp_lokalny.tv_sec;
    
    return daytime;
}

// Clone thread function
int funkcja_watku(void* argument) {
    zmienna_globalna++;
    return 0;
}

// Fork test function
void test_fork() {
    int pid, i;
    
    inicjuj_czas_lokalny();
    
    for(i = 0; i < 1000; i++) {
        pid = fork();
        
        if(pid == 0) {
            zmienna_globalna++;
            exit(0);
        } else {
            wait(NULL);
        }
    }
    
    printf("%.6f\n", czas_C_poprawny());
    printf("%.6f\n", czas_CPU_poprawny());
    printf("%.6f\n", czas_zegara_poprawny());
}

// Clone test function
void test_clone() {
    void *stos;
    pid_t pid;
    int i;
    
    stos = malloc(ROZMIAR_STOSU);
    if (stos == 0) {
        printf("Błąd alokacji stosu\n");
        return;
    }
    
    inicjuj_czas_lokalny();
    
    for(i = 0; i < 1000; i++) {
        pid = clone(&funkcja_watku, (void*)stos + ROZMIAR_STOSU,
                   CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0);
        waitpid(pid, NULL, __WCLONE);
    }
    
    printf("%.6f\n", czas_C_poprawny());
    printf("%.6f\n", czas_CPU_poprawny());
    printf("%.6f\n", czas_zegara_poprawny());
    
    free(stos);
}

int main() {
    int liczba_prob;
    int i;
    
    printf("=== PORÓWNANIE FORK CLONE ===\n");
    printf("Format wyników:\n");
    printf("Każdy test wypisuje 3 wartości w sekundach:\n");
    printf("1. standardowy\n");
    printf("2. cpu\n");
    printf("3. zegarowy\n");
    printf("===========================================\n\n");
    
    printf("Podaj liczbę prób do wykonania: ");
    scanf("%d", &liczba_prob);
    
    printf("\n=== TESTY FORK (procesy) ===\n");
    for(i = 1; i <= liczba_prob; i++) {
        printf("fork %d\n", i);
        test_fork();
    }
    
    printf("\n=== TESTY CLONE (wątki) ===\n");
    for(i = 1; i <= liczba_prob; i++) {
        printf("clone %d\n", i);
        test_clone();
    }
    
    return 0;
}
