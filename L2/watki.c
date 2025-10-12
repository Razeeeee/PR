#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>

// Zmienna globalna współdzielona między wątkami
int zmienna_globalna = 0;

#define ROZMIAR_STOSU 1024*64
#define LICZBA_ITERACJI 100000

// Struktura do przekazania argumentów do wątku
struct argumenty_watku {
    int *zmienna_lokalna;
    int id_watku;
};

// Funkcja wykonywana przez pierwszy wątek
int funkcja_watku_1(void* argument) {
    struct argumenty_watku *args = (struct argumenty_watku*)argument;
    int i;
    
    printf("Wątek 1: rozpoczynam pracę\n");
    
    // Pętla zwiększająca zmienne
    for(i = 0; i < LICZBA_ITERACJI; i++) {
        zmienna_globalna++;      // Zwiększenie zmiennej globalnej
        (*(args->zmienna_lokalna))++;  // Zwiększenie zmiennej lokalnej
    }
    
    printf("Wątek 1: zakończyłem pracę\n");
    printf("Wątek 1: zmienna_globalna = %d, zmienna_lokalna = %d\n", 
           zmienna_globalna, *(args->zmienna_lokalna));
    
    return 0;
}

// Funkcja wykonywana przez drugi wątek
int funkcja_watku_2(void* argument) {
    struct argumenty_watku *args = (struct argumenty_watku*)argument;
    int i;
    
    printf("Wątek 2: rozpoczynam pracę\n");
    
    // Pętla zwiększająca zmienne  
    for(i = 0; i < LICZBA_ITERACJI; i++) {
        zmienna_globalna++;      // Zwiększenie zmiennej globalnej
        (*(args->zmienna_lokalna))++;  // Zwiększenie zmiennej lokalnej
    }
    
    printf("Wątek 2: zakończyłem pracę\n");
    printf("Wątek 2: zmienna_globalna = %d, zmienna_lokalna = %d\n", 
           zmienna_globalna, *(args->zmienna_lokalna));
    
    return 0;
}

int main() {
    void *stos1, *stos2;
    pid_t pid1, pid2;
    
    // Zmienne lokalne przekazywane do wątków
    int zmienna_lokalna_1 = 0;
    int zmienna_lokalna_2 = 0;
    
    // Struktury argumentów dla wątków
    struct argumenty_watku args1 = {&zmienna_lokalna_1, 1};
    struct argumenty_watku args2 = {&zmienna_lokalna_2, 2};
    
    printf("=== DWA WĄTKI ===\n");
    printf("Liczba iteracji: %d\n", LICZBA_ITERACJI);
    printf("Oczekiwana wartość zmiennej globalnej: %d\n", 2 * LICZBA_ITERACJI);
    printf("Oczekiwana wartość każdej zmiennej lokalnej: %d\n", LICZBA_ITERACJI);
    printf("========================================\n\n");
    
    // Alokacja stosów dla wątków
    stos1 = malloc(ROZMIAR_STOSU);
    if (stos1 == 0) {
        printf("nie można zaalokować stosu dla wątku 1\n");
        exit(1);
    }
    
    stos2 = malloc(ROZMIAR_STOSU);
    if (stos2 == 0) {
        printf("nie można zaalokować stosu dla wątku 2\n");
        free(stos1);
        exit(1);
    }
    
    printf("stan początkowy:\n");
    printf("zmienna_globalna = %d\n", zmienna_globalna);
    printf("zmienna_lokalna_1 = %d\n", zmienna_lokalna_1);
    printf("zmienna_lokalna_2 = %d\n\n", zmienna_lokalna_2);
    
    // Tworzenie pierwszego wątku
    printf("Tworzę wątek 1...\n");
    pid1 = clone(&funkcja_watku_1, (void*)stos1 + ROZMIAR_STOSU,
                 CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &args1);
    
    if (pid1 == -1) {
        printf("nie można utworzyć wątku 1\n");
        free(stos1);
        free(stos2);
        exit(1);
    }
    
    // Tworzenie drugiego wątku BEZPOŚREDNIO po pierwszym (bez czekania!)
    printf("Tworzę wątek 2...\n");
    pid2 = clone(&funkcja_watku_2, (void*)stos2 + ROZMIAR_STOSU,
                 CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &args2);
    
    if (pid2 == -1) {
        printf("nie można utworzyć wątku 2\n");
        free(stos1);
        free(stos2);
        exit(1);
    }
    
    printf("Oba wątki zostały uruchomione\n\n");
    
    // Teraz czekamy na zakończenie obu wątków
    waitpid(pid1, NULL, __WCLONE);
    printf("Wątek 1 zakończony\n");
    
    waitpid(pid2, NULL, __WCLONE);
    printf("Wątek 2 zakończony\n\n");
    
    // Wypisanie końcowych wartości zmiennych w main
    printf("=== WYNIKI KOŃCOWE ===\n");
    printf("zmienna_globalna = %d (oczekiwana: %d)\n", 
           zmienna_globalna, 2 * LICZBA_ITERACJI);
    printf("zmienna_lokalna_1 = %d (oczekiwana: %d)\n", 
           zmienna_lokalna_1, LICZBA_ITERACJI);
    printf("zmienna_lokalna_2 = %d (oczekiwana: %d)\n", 
           zmienna_lokalna_2, LICZBA_ITERACJI);
    
    // Analiza wyników
    printf("\n=== ANALIZA WYNIKÓW ===\n");
    
    if (zmienna_globalna == 2 * LICZBA_ITERACJI) {
        printf("zmienna globalna wartość jest zgodna z oczekiwaną\n");
    } else {
        printf("zmienna globalna wartość różni się od oczekiwanej!\n");
        printf("różnica: %d\n", 
               (2 * LICZBA_ITERACJI) - zmienna_globalna);
    }
    
    if (zmienna_lokalna_1 == LICZBA_ITERACJI) {
        printf("zmienna lokalna 1 wartość jest zgodna z oczekiwaną\n");
    } else {
        printf("zmienna lokalna 1 wartość różni się od oczekiwanej!\n");
    }
    
    if (zmienna_lokalna_2 == LICZBA_ITERACJI) {
        printf("zmienna lokalna 2 wartość jest zgodna z oczekiwaną\n");
    } else {
        printf("zmienna lokalna 2 wartość różni się od oczekiwanej!\n");
    }
    
    // Zwolnienie pamięci
    free(stos1);
    free(stos2);
    
    return 0;
}
