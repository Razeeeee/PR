#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Czytelnia wykorzystująca dedykowane zamki do zapisu i odczytu
pthread_rwlock_t rwlock;

void* czytelnik(void* arg) {
    int id = *(int*)arg;
    
    for (int i = 0; i < 3; i++) {
        printf("Czytelnik %d: chce czytac\n", id);
        
        // Zamek do odczytu - wiele wątków może czytać jednocześnie
        pthread_rwlock_rdlock(&rwlock);
        
        printf("Czytelnik %d: CZYTAM (iteracja %d)\n", id, i+1);
        sleep(1);
        
        pthread_rwlock_unlock(&rwlock);
        printf("Czytelnik %d: koniec czytania\n", id);
        
        sleep(rand() % 2);
    }
    
    return NULL;
}

void* pisarz(void* arg) {
    int id = *(int*)arg;
    
    for (int i = 0; i < 3; i++) {
        printf("Pisarz %d: chce pisac\n", id);
        
        // Zamek do zapisu - tylko jeden wątek może pisać
        pthread_rwlock_wrlock(&rwlock);
        
        printf("Pisarz %d: PISZE (iteracja %d)\n", id, i+1);
        sleep(2);
        
        pthread_rwlock_unlock(&rwlock);
        printf("Pisarz %d: koniec pisania\n", id);
        
        sleep(rand() % 2);
    }
    
    return NULL;
}

int main() {
    const int LICZBA_CZYTELNIKOW = 5;
    const int LICZBA_PISARZY = 2;
    pthread_t czyt[LICZBA_CZYTELNIKOW];
    pthread_t pis[LICZBA_PISARZY];
    int czyt_ids[LICZBA_CZYTELNIKOW];
    int pis_ids[LICZBA_PISARZY];
    
    printf("=== Czytelnia z pthread_rwlock ===\n");
    printf("Wykorzystanie: pthread_rwlock_rdlock, pthread_rwlock_wrlock, pthread_rwlock_unlock\n");
    printf("Czytelnikow: %d, Pisarzy: %d\n\n", LICZBA_CZYTELNIKOW, LICZBA_PISARZY);
    
    // Inicjalizacja zamka odczytu/zapisu
    pthread_rwlock_init(&rwlock, NULL);
    
    // Tworzenie wątków czytelników
    for (int i = 0; i < LICZBA_CZYTELNIKOW; i++) {
        czyt_ids[i] = i + 1;
        pthread_create(&czyt[i], NULL, czytelnik, &czyt_ids[i]);
    }
    
    // Tworzenie wątków pisarzy
    for (int i = 0; i < LICZBA_PISARZY; i++) {
        pis_ids[i] = i + 1;
        pthread_create(&pis[i], NULL, pisarz, &pis_ids[i]);
    }
    
    // Oczekiwanie na zakończenie wątków
    for (int i = 0; i < LICZBA_CZYTELNIKOW; i++) {
        pthread_join(czyt[i], NULL);
    }
    for (int i = 0; i < LICZBA_PISARZY; i++) {
        pthread_join(pis[i], NULL);
    }
    
    // Niszczenie zamka
    pthread_rwlock_destroy(&rwlock);
    
    printf("\n=== Wszystkie watki zakonczone ===\n");
    
    return 0;
}
