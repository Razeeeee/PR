#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// Monitor Czytelnia - implementacja według Wykładu 6, slajd 10-12
typedef struct {
    int liczba_czyt;
    int liczba_pisz;
    int czekajacy_pisarze;  // zastępuje empty(pisarze)
    pthread_mutex_t mutex;
    pthread_cond_t czytelnicy;
    pthread_cond_t pisarze;
} czytelnia_t;

czytelnia_t czytelnia;

void chce_pisac() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    // 1. prosty protokół wejścia
    czytelnia.czekajacy_pisarze++;
    while (czytelnia.liczba_czyt + czytelnia.liczba_pisz > 0) {
        pthread_cond_wait(&czytelnia.pisarze, &czytelnia.mutex);
    }
    czytelnia.czekajacy_pisarze--;
    
    czytelnia.liczba_pisz++;
    
    pthread_mutex_unlock(&czytelnia.mutex);
}

void koniec_pisania() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    // 3. protokół wyjścia – uczciwy względem czytelników
    czytelnia.liczba_pisz--;
    
    // JEŻELI( ~empty( czytelnicy ) ) signal( czytelnicy );
    // WPP signal( pisarze )
    // W C nie mamy empty(), więc budzi się pierwszy z kolejki czytelnicy
    pthread_cond_signal(&czytelnia.czytelnicy);
    pthread_cond_signal(&czytelnia.pisarze);
    
    pthread_mutex_unlock(&czytelnia.mutex);
}

void chce_czytac() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    // 4. protokół wejścia: bezpieczny i uczciwy względem pisarzy
    // JEŻELI( liczba_pisz > 0 lub ~empty( pisarze ) ) wait( czytelnicy );
    while (czytelnia.liczba_pisz > 0 || czytelnia.czekajacy_pisarze > 0) {
        pthread_cond_wait(&czytelnia.czytelnicy, &czytelnia.mutex);
    }
    
    czytelnia.liczba_czyt++;
    
    // 6. czytelnicy czekający na wejście budzą się kolejno nawzajem
    pthread_cond_signal(&czytelnia.czytelnicy);
    
    pthread_mutex_unlock(&czytelnia.mutex);
}

void koniec_czytania() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    // 5. protokół wyjścia – budzi tylko pisarzy
    czytelnia.liczba_czyt--;
    
    if (czytelnia.liczba_czyt == 0) {
        pthread_cond_signal(&czytelnia.pisarze);
    }
    
    pthread_mutex_unlock(&czytelnia.mutex);
}

void* czytelnik(void* arg) {
    int id = *(int*)arg;
    
    for (int i = 0; i < 3; i++) {
        printf("Czytelnik %d: chce czytac\n", id);
        chce_czytac();
        
        printf("Czytelnik %d: CZYTAM (iteracja %d)\n", id, i+1);
        sleep(1);
        
        koniec_czytania();
        printf("Czytelnik %d: koniec czytania\n", id);
        
        sleep(rand() % 2);
    }
    
    return NULL;
}

void* pisarz(void* arg) {
    int id = *(int*)arg;
    
    for (int i = 0; i < 3; i++) {
        printf("Pisarz %d: chce pisac\n", id);
        chce_pisac();
        
        printf("Pisarz %d: PISZE (iteracja %d)\n", id, i+1);
        sleep(2);
        
        koniec_pisania();
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
    
    printf("=== Implementacja czytelni ===\n");
    printf("Czytelnikow: %d, Pisarzy: %d\n\n", LICZBA_CZYTELNIKOW, LICZBA_PISARZY);
    
    // Inicjalizacja monitora
    czytelnia.liczba_czyt = 0;
    czytelnia.liczba_pisz = 0;
    czytelnia.czekajacy_pisarze = 0;
    pthread_mutex_init(&czytelnia.mutex, NULL);
    pthread_cond_init(&czytelnia.czytelnicy, NULL);
    pthread_cond_init(&czytelnia.pisarze, NULL);
    
    // Tworzenie wątków
    for (int i = 0; i < LICZBA_CZYTELNIKOW; i++) {
        czyt_ids[i] = i + 1;
        pthread_create(&czyt[i], NULL, czytelnik, &czyt_ids[i]);
    }
    
    for (int i = 0; i < LICZBA_PISARZY; i++) {
        pis_ids[i] = i + 1;
        pthread_create(&pis[i], NULL, pisarz, &pis_ids[i]);
    }
    
    // Oczekiwanie na zakończenie
    for (int i = 0; i < LICZBA_CZYTELNIKOW; i++) {
        pthread_join(czyt[i], NULL);
    }
    for (int i = 0; i < LICZBA_PISARZY; i++) {
        pthread_join(pis[i], NULL);
    }
    
    // Sprzątanie
    pthread_mutex_destroy(&czytelnia.mutex);
    pthread_cond_destroy(&czytelnia.czytelnicy);
    pthread_cond_destroy(&czytelnia.pisarze);
    
    printf("\n=== Wszystkie watki zakonczone ===\n");
    
    return 0;
}