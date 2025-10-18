#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define LICZBA_WATKOW 20

// Funkcja wykonywana przez każdy wątek
void* funkcja_watku(void* arg) {
    // Pobranie przekazanego identyfikatora wątku
    int id_watku = *(int*)arg;
    
    // Pobranie systemowego ID wątku
    pthread_t system_id = pthread_self();
    
    // Wypisanie informacji o wątku
    printf("Watek ID: %d, Systemowy ID: %lu\n", id_watku, (unsigned long)system_id);
    
    // Krótka praca wątku - symulacja działania
    sleep(1);
    
    printf("Watek ID: %d konczy prace\n", id_watku);
    
    return NULL;
}

int main() {
    pthread_t watki[LICZBA_WATKOW];
    int identyfikatory[LICZBA_WATKOW];
    int i;
    
    printf("=== WERSJA POPRAWNA - przesylanie odrebnych identyfikatorow ===\n");
    printf("Tworzenie %d watkow...\n\n", LICZBA_WATKOW);
    
    // WERSJA POPRAWNA: Tworzenie wątków z odrębnymi identyfikatorami
    for (i = 0; i < LICZBA_WATKOW; i++) {
        identyfikatory[i] = i;  // Każdy wątek ma swój własny identyfikator
        
        printf("Tworzenie watku o ID: %d\n", i);
        
        // Przekazujemy adres ODREBNEJ komórki pamięci dla każdego wątku
        if (pthread_create(&watki[i], NULL, funkcja_watku, &identyfikatory[i]) != 0) {
            printf("Blad tworzenia watku %d\n", i);
            exit(1);
        }
    }
    
    printf("\nWszystkie watki zostaly utworzone. Oczekiwanie na zakonczenie...\n\n");
    
    // Oczekiwanie na zakończenie wszystkich wątków
    for (i = 0; i < LICZBA_WATKOW; i++) {
        if (pthread_join(watki[i], NULL) != 0) {
            printf("Blad oczekiwania na watek %d\n", i);
            exit(1);
        }
    }
    
    printf("\n=== WERSJA BLEDNA - przesylanie tego samego wskaznika ===\n");
    printf("Tworzenie %d watkow z bledna synchronizacja...\n\n", LICZBA_WATKOW);
    
    // WERSJA BŁĘDNA: Tworzenie wątków z tym samym wskaźnikiem
    for (i = 0; i < LICZBA_WATKOW; i++) {
        printf("Tworzenie watku o ID: %d\n", i);
        
        // BŁĄD: Przekazujemy adres zmiennej sterującej pętli!
        // Wszystkie wątki otrzymują ten sam adres &i
        if (pthread_create(&watki[i], NULL, funkcja_watku, &i) != 0) {
            printf("Blad tworzenia watku %d\n", i);
            exit(1);
        }
        
        // Usunięto opóźnienie - zwiększa to prawdopodobieństwo błędu
        // usleep(1000); // 1ms
    }
    
    printf("\nWszystkie BLEDNE watki zostaly utworzone. Oczekiwanie na zakonczenie...\n\n");
    
    // Oczekiwanie na zakończenie wszystkich wątków
    for (i = 0; i < LICZBA_WATKOW; i++) {
        if (pthread_join(watki[i], NULL) != 0) {
            printf("Blad oczekiwania na watek %d\n", i);
            exit(1);
        }
    }

    return 0;
}
