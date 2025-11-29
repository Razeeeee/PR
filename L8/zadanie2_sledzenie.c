#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Struktura czytelni ze śledzeniem błędów
typedef struct {
    int liczba_czyt;
    int liczba_pisz;
    pthread_mutex_t mutex;
    pthread_cond_t czytelnicy;
    pthread_cond_t pisarze;
} czytelnia_t;

czytelnia_t czytelnia;

// Wiele czytelników może czytać jednocześnie
// Tylko jeden pisarz może pisać na raz
// Pisarz nie może pisać, gdy czytelnicy czytają
// Czytelnik nie może czytać, gdy pisarz pisze

void sprawdz_poprawnosc(const char* funkcja) {
    if (czytelnia.liczba_czyt < 0) {
        printf("BLAD w %s: liczba_czyt = %d (nie moze byc ujemna!)\n", 
               funkcja, czytelnia.liczba_czyt);
    }
    if (czytelnia.liczba_pisz < 0) {
        printf("BLAD w %s: liczba_pisz = %d (nie moze byc ujemna!)\n", 
               funkcja, czytelnia.liczba_pisz);
    }
    if (czytelnia.liczba_pisz > 1) {
        printf("BLAD w %s: liczba_pisz = %d (moze byc maksymalnie 1!)\n", 
               funkcja, czytelnia.liczba_pisz);
    }
    if (czytelnia.liczba_czyt > 0 && czytelnia.liczba_pisz > 0) {
        printf("BLAD w %s: jednoczesnie czytelnicy (%d) i pisarze (%d)!\n", 
               funkcja, czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    }
}

void chce_pisac() {
    pthread_mutex_lock(&czytelnia.mutex);
    printf("  [PISARZ] Chce pisac (czyt=%d, pisz=%d)\n", 
           czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    sprawdz_poprawnosc("chce_pisac - przed wait");
    
    // While zamiast if, aby uniknac problemu spurious wakeup
    // Wątek może zostać obudzony bez sygnału
    while (czytelnia.liczba_czyt + czytelnia.liczba_pisz > 0) {
        pthread_cond_wait(&czytelnia.pisarze, &czytelnia.mutex);
    }
    
    czytelnia.liczba_pisz++;
    printf("  [PISARZ] Rozpoczynam pisanie (czyt=%d, pisz=%d)\n", 
           czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    sprawdz_poprawnosc("chce_pisac - po wait");
    pthread_mutex_unlock(&czytelnia.mutex);
}

void koniec_pisania() {
    pthread_mutex_lock(&czytelnia.mutex);
    czytelnia.liczba_pisz--;
    printf("  [PISARZ] Koniec pisania (czyt=%d, pisz=%d)\n", 
           czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    sprawdz_poprawnosc("koniec_pisania");
    
    // Signal a nie broadcast, bo tylko jeden pisarz moze pisac na raz
    // Obudzony czytelnik sam budzi następnego czytelnika w chce_czytac()
    pthread_cond_signal(&czytelnia.czytelnicy);
    pthread_cond_signal(&czytelnia.pisarze);
    pthread_mutex_unlock(&czytelnia.mutex);
}

void chce_czytac() {
    pthread_mutex_lock(&czytelnia.mutex);
    printf("  [CZYTELNIK] Chce czytac (czyt=%d, pisz=%d)\n", 
           czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    sprawdz_poprawnosc("chce_czytac - przed wait");
    
    // While zamiast if, aby uniknac problemu spurious wakeup
    // Wątek może zostać obudzony bez sygnału
    while (czytelnia.liczba_pisz > 0) {
        pthread_cond_wait(&czytelnia.czytelnicy, &czytelnia.mutex);
    }
    
    czytelnia.liczba_czyt++;
    printf("  [CZYTELNIK] Rozpoczynam czytanie (czyt=%d, pisz=%d)\n", 
           czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    sprawdz_poprawnosc("chce_czytac - po wait");
    
    // Obudzenie kolejnego czytelnika, jeśli czeka
    pthread_cond_signal(&czytelnia.czytelnicy);
    pthread_mutex_unlock(&czytelnia.mutex);
}

void koniec_czytania() {
    pthread_mutex_lock(&czytelnia.mutex);
    czytelnia.liczba_czyt--;
    printf("  [CZYTELNIK] Koniec czytania (czyt=%d, pisz=%d)\n", 
           czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    sprawdz_poprawnosc("koniec_czytania");
    
    // Jeśli to był ostatni czytelnik, obudź pisarza
    if (czytelnia.liczba_czyt == 0) {
        pthread_cond_signal(&czytelnia.pisarze);
    }
    pthread_mutex_unlock(&czytelnia.mutex);
}

void* czytelnik(void* arg) {
    int id = *(int*)arg;
    
    for (int i = 0; i < 2; i++) {
        chce_czytac();
        printf("    Czytelnik %d czyta...\n", id);
        sleep(1);
        koniec_czytania();
        sleep(rand() % 2);
    }
    
    return NULL;
}

void* pisarz(void* arg) {
    int id = *(int*)arg;
    
    for (int i = 0; i < 2; i++) {
        chce_pisac();
        printf("    Pisarz %d pisze...\n", id);
        sleep(1);
        koniec_pisania();
        sleep(rand() % 2);
    }
    
    return NULL;
}

int main() {
    const int LICZBA_CZYTELNIKOW = 3;
    const int LICZBA_PISARZY = 2;
    pthread_t czyt[LICZBA_CZYTELNIKOW];
    pthread_t pis[LICZBA_PISARZY];
    int czyt_ids[LICZBA_CZYTELNIKOW];
    int pis_ids[LICZBA_PISARZY];
    
    printf("=== Test sledzenia liczby czytelnikow i pisarzy ===\n\n");
    
    // Inicjalizacja
    czytelnia.liczba_czyt = 0;
    czytelnia.liczba_pisz = 0;
    pthread_mutex_init(&czytelnia.mutex, NULL);
    pthread_cond_init(&czytelnia.czytelnicy, NULL);
    pthread_cond_init(&czytelnia.pisarze, NULL);
    
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
    
    printf("\n=== Test zakonczony ===\n");
    printf("Stan koncowy: czyt=%d, pisz=%d\n", 
           czytelnia.liczba_czyt, czytelnia.liczba_pisz);
    
    return 0;
}
