#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int liczba_watkow;
    int licznik;
    pthread_mutex_t mutex;
    pthread_cond_t warunek;
} bariera_t;

void bariera_init(bariera_t *b, int liczba_watkow) {
    b->liczba_watkow = liczba_watkow;
    b->licznik = 0;
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->warunek, NULL);
}

void bariera(bariera_t *b) {
    pthread_mutex_lock(&b->mutex);
    b->licznik++;
    
    if (b->licznik == b->liczba_watkow) {
        // Ostatni wątek resetuje licznik i budzi wszystkich
        b->licznik = 0;
        pthread_cond_broadcast(&b->warunek);
    } else {
        // Pozostałe wątki czekają
        pthread_cond_wait(&b->warunek, &b->mutex);
    }
    
    pthread_mutex_unlock(&b->mutex);
}

void bariera_destroy(bariera_t *b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->warunek);
}

// Globalne bariery
bariera_t bariera1, bariera2, bariera3;

void* funkcja_watku(void* arg) {
    int id = *(int*)arg;
    
    printf("Watek %d: Rozpoczynam faze 1\n", id);
    sleep(rand() % 3);
    printf("Watek %d: Koncze faze 1, czekam na barierze 1\n", id);
    bariera(&bariera1);
    
    printf("Watek %d: Rozpoczynam faze 2\n", id);
    sleep(rand() % 3);
    printf("Watek %d: Koncze faze 2, czekam na barierze 2\n", id);
    bariera(&bariera2);
    
    printf("Watek %d: Rozpoczynam faze 3\n", id);
    sleep(rand() % 3);
    printf("Watek %d: Koncze faze 3, czekam na barierze 3\n", id);
    bariera(&bariera3);
    
    printf("Watek %d: Zakonczono wszystkie fazy\n", id);
    
    return NULL;
}

int main() {
    const int LICZBA_WATKOW = 5;
    pthread_t watki[LICZBA_WATKOW];
    int ids[LICZBA_WATKOW];
    
    printf("=== Test mechanizmu bariery ===\n");
    printf("Liczba watkow: %d\n", LICZBA_WATKOW);
    printf("Liczba barier: 3\n\n");
    
    // Inicjalizacja barier
    bariera_init(&bariera1, LICZBA_WATKOW);
    bariera_init(&bariera2, LICZBA_WATKOW);
    bariera_init(&bariera3, LICZBA_WATKOW);
    
    // Tworzenie wątków
    for (int i = 0; i < LICZBA_WATKOW; i++) {
        ids[i] = i + 1;
        pthread_create(&watki[i], NULL, funkcja_watku, &ids[i]);
    }
    
    // Oczekiwanie na zakończenie wątków
    for (int i = 0; i < LICZBA_WATKOW; i++) {
        pthread_join(watki[i], NULL);
    }
    
    // Niszczenie barier
    bariera_destroy(&bariera1);
    bariera_destroy(&bariera2);
    bariera_destroy(&bariera3);
    
    printf("\n=== Wszystkie watki zakonczone ===\n");
    
    return 0;
}
