#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// Monitor Czytelnia - przemienne pierwszeństwo
// Najpierw jeden pisarz, potem wszyscy czytelnicy, potem jeden pisarz...
typedef struct {
    int liczba_czyt;           // Liczba aktywnie czytających
    int liczba_pisz;           // Liczba aktywnie piszących (0 lub 1)
    int czekajacy_pisarze;     // Liczba czekających pisarzy
    int czekajacy_czytelnicy;  // Liczba czekających czytelników
    bool kolej_pisarzy;        // true = kolej pisarzy, false = kolej czytelników
    pthread_mutex_t mutex;
    pthread_cond_t czytelnicy;
    pthread_cond_t pisarze;
} czytelnia_t;

czytelnia_t czytelnia;

void chce_pisac() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    czytelnia.czekajacy_pisarze++;
    
    // Pisarz czeka dopóki:
    // - ktoś czyta lub pisze LUB
    // - nie ma kolejki dla pisarzy
    while (czytelnia.liczba_czyt > 0 || czytelnia.liczba_pisz > 0 || 
           !czytelnia.kolej_pisarzy) {
        pthread_cond_wait(&czytelnia.pisarze, &czytelnia.mutex);
    }
    
    czytelnia.liczba_pisz = 1;
    czytelnia.czekajacy_pisarze--;
    
    pthread_mutex_unlock(&czytelnia.mutex);
}

void koniec_pisania() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    czytelnia.liczba_pisz = 0;
    czytelnia.kolej_pisarzy = false; // Teraz kolej czytelników
    
    // Budzi WSZYSTKICH czytelników
    pthread_cond_broadcast(&czytelnia.czytelnicy);
    
    pthread_mutex_unlock(&czytelnia.mutex);
}

void chce_czytac() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    czytelnia.czekajacy_czytelnicy++;
    
    // Czytelnik czeka dopóki:
    // - ktoś pisze LUB
    // - jest kolejka dla pisarzy
    while (czytelnia.liczba_pisz > 0 || czytelnia.kolej_pisarzy) {
        pthread_cond_wait(&czytelnia.czytelnicy, &czytelnia.mutex);
    }
    
    // Pierwszy czytelnik zmienia kolejkę na pisarzy (blokuje nowych czytelników)
    if (czytelnia.liczba_czyt == 0) {
        czytelnia.kolej_pisarzy = true;
    }
    
    czytelnia.liczba_czyt++;
    czytelnia.czekajacy_czytelnicy--;
    
    // Każdy czytelnik budzi kolejnych (wszyscy czytają razem)
    pthread_cond_broadcast(&czytelnia.czytelnicy);
    
    pthread_mutex_unlock(&czytelnia.mutex);
}

void koniec_czytania() {
    pthread_mutex_lock(&czytelnia.mutex);
    
    czytelnia.liczba_czyt--;
    
    // Ostatni czytelnik budzi pisarza
    if (czytelnia.liczba_czyt == 0) {
        // Budzi JEDNEGO pisarza
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
    
    printf("=== Implementacja czytelni - przemienne pierwszenstwo ===\n");
    printf("Czytelnikow: %d, Pisarzy: %d\n\n", LICZBA_CZYTELNIKOW, LICZBA_PISARZY);
    
    // Inicjalizacja monitora
    czytelnia.liczba_czyt = 0;
    czytelnia.liczba_pisz = 0;
    czytelnia.czekajacy_pisarze = 0;
    czytelnia.czekajacy_czytelnicy = 0;
    czytelnia.kolej_pisarzy = true; // Zaczynamy od pisarza!
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