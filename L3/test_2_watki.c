#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>
#include <limits.h>

// Definicja minimalnego rozmiaru stosu jesli nie jest dostepna
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384  // 16KB - typowa wartosc
#endif

// nasza struktura z danymi osoby
typedef struct {
    int id_watku;
    char imie[50];
    char nazwisko[50];
    int wiek;
    int detached;  // czy watek jest detached
    size_t stack_size;  // rozmiar stosu
    void* custom_stack_addr;  // adres custom stosu (jesli jest)
} DaneOsoby;

// funkcja pomocnicza do wypisania atrybutow watku
void wypisz_atrybuty(pthread_attr_t* attr, int id) {
    int detach_state;
    size_t stack_size;
    void* stack_addr;
    int policy;
    struct sched_param param;
    
    printf("=== ATRYBUTY WATKU %d ===\n", id);
    
    // stan detach
    if (pthread_attr_getdetachstate(attr, &detach_state) == 0) {
        printf("Detach state: %s\n", 
               detach_state == PTHREAD_CREATE_DETACHED ? "DETACHED" : "JOINABLE");
    }
    
    // rozmiar stosu
    if (pthread_attr_getstacksize(attr, &stack_size) == 0) {
        printf("Stack size: %zu bytes (%.2f KB)\n", stack_size, stack_size / 1024.0);
    }
    
    // adres stosu
    if (pthread_attr_getstack(attr, &stack_addr, &stack_size) == 0) {
        printf("Stack address: %p\n", stack_addr);
    }
    
    // polityka szeregowania
    if (pthread_attr_getschedpolicy(attr, &policy) == 0) {
        printf("Scheduling policy: ");
        switch(policy) {
            case SCHED_OTHER: printf("SCHED_OTHER\n"); break;
            case SCHED_FIFO: printf("SCHED_FIFO\n"); break;
            case SCHED_RR: printf("SCHED_RR\n"); break;
            default: printf("Unknown (%d)\n", policy); break;
        }
    }
    
    // priorytet
    if (pthread_attr_getschedparam(attr, &param) == 0) {
        printf("Priority: %d\n", param.sched_priority);
    }
    
    printf("========================\n\n");
}

// funkcja watku - to co robi kazdy watek
void* funkcja_watku(void* arg) {
    // zmieniamy arg na nasza strukture
    DaneOsoby* dane = (DaneOsoby*)arg;
    
    // sprawdzamy id watku w systemie
    pthread_t system_id = pthread_self();
    
    // wypisujemy wszystkie dane
    printf("--- WATEK %d ROZPOCZYNA ---\n", dane->id_watku);
    printf("ID systemu: %lu\n", (unsigned long)system_id);
    printf("Imie: %s\n", dane->imie);
    printf("Nazwisko: %s\n", dane->nazwisko);
    printf("Wiek: %d\n", dane->wiek);
    printf("Adres struktury: %p\n", (void*)dane);
    printf("Detached: %s\n", dane->detached ? "TAK" : "NIE");
    printf("Rozmiar stosu: %zu bytes\n", dane->stack_size);
    
    // sprawdzamy aktualny stos
    char local_var;
    printf("Adres zmiennej lokalnej: %p\n", (void*)&local_var);
    printf("---\n\n");
    
    // watek robi cos przez rozny czas w zaleznosci od id
    int sleep_time = dane->id_watku;
    printf("Watek %d pracuje przez %d sekund...\n", dane->id_watku, sleep_time);
    sleep(sleep_time);
    
    printf("Watek %d (%s %s) konczy\n", dane->id_watku, dane->imie, dane->nazwisko);
    
    return NULL;
}

// funkcja dla detached watku
void* funkcja_detached_watku(void* arg) {
    DaneOsoby* dane = (DaneOsoby*)arg;
    pthread_t system_id = pthread_self();
    
    printf("--- DETACHED WATEK %d ---\n", dane->id_watku);
    printf("ID systemu: %lu\n", (unsigned long)system_id);
    printf("Imie: %s %s, wiek: %d\n", dane->imie, dane->nazwisko, dane->wiek);
    printf("Adres struktury: %p\n", (void*)dane);
    printf("Rozmiar stosu: %zu bytes\n", dane->stack_size);
    printf("Adres zmiennej lokalnej: %p\n", (void*)&dane);
    
    // sprawdzamy czy adres zmiennej lokalnej jest w naszym custom stosie
    char local_var;
    printf("Adres zmiennej lokalnej w stosie: %p\n", (void*)&local_var);
    printf("---\n\n");
    
    sleep(3);
    
    printf("DETACHED watek %d konczy (moze byc niewidoczne)\n", dane->id_watku);
    
    // zapisujemy adres stosu przed zwolnieniem struktury
    void* stack_to_free = dane->custom_stack_addr;
    
    // zwalniamy pamiec bo nikt na nas nie czeka
    free(dane);
    
    if (stack_to_free) {
        free(stack_to_free);
    }
    
    return NULL;
}

int main() {
    pthread_t watek1, watek2, watek3;
    pthread_attr_t attr1, attr2, attr3;
    
    printf("Test watkow z roznymy atrybutami\n");
    printf("=================================\n\n");
    
    // WATEK 1: Domyslne atrybuty (joinable)
    printf("--- WATEK 1: Domyslne atrybuty ---\n");
    
    // inicjalizujemy atrybuty domyslne
    if (pthread_attr_init(&attr1) != 0) {
        printf("Blad inicjalizacji atrybutow 1\n");
        exit(1);
    }
    
    wypisz_atrybuty(&attr1, 1);
    
    DaneOsoby osoba1;
    osoba1.id_watku = 1;
    strcpy(osoba1.imie, "Jan");
    strcpy(osoba1.nazwisko, "Kowalski");
    osoba1.wiek = 25;
    osoba1.detached = 0;
    osoba1.custom_stack_addr = NULL;  // brak custom stosu
    
    // pobieramy rozmiar stosu
    size_t stack_size1;
    pthread_attr_getstacksize(&attr1, &stack_size1);
    osoba1.stack_size = stack_size1;
    
    if (pthread_create(&watek1, &attr1, funkcja_watku, &osoba1) != 0) {
        printf("Nie udalo sie stworzyc watku 1\n");
        exit(1);
    }
    
    // WATEK 2: Wiekszy stos + malloc
    printf("--- WATEK 2: Wiekszy stos (1MB) ---\n");
    
    if (pthread_attr_init(&attr2) != 0) {
        printf("Blad inicjalizacji atrybutow 2\n");
        exit(1);
    }
    
    // ustawiamy wiekszy stos - 1MB
    size_t new_stack_size = 1024 * 1024;  // 1MB
    if (pthread_attr_setstacksize(&attr2, new_stack_size) != 0) {
        printf("Nie udalo sie ustawic rozmiaru stosu\n");
    }
    
    wypisz_atrybuty(&attr2, 2);
    
    DaneOsoby* osoba2 = malloc(sizeof(DaneOsoby));
    if (osoba2 == NULL) {
        printf("Nie udalo sie zarezerwowac pamieci\n");
        exit(1);
    }
    
    osoba2->id_watku = 2;
    strcpy(osoba2->imie, "Anna");
    strcpy(osoba2->nazwisko, "Nowak");
    osoba2->wiek = 30;
    osoba2->detached = 0;
    osoba2->stack_size = new_stack_size;
    osoba2->custom_stack_addr = NULL;  // brak custom stosu
    
    if (pthread_create(&watek2, &attr2, funkcja_watku, osoba2) != 0) {
        printf("Nie udalo sie stworzyc watku 2\n");
        free(osoba2);
        exit(1);
    }
    
    // WATEK 3: DETACHED + maly stos
    printf("--- WATEK 3: DETACHED + maly stos ---\n");
    
    if (pthread_attr_init(&attr3) != 0) {
        printf("Blad inicjalizacji atrybutow 3\n");
        exit(1);
    }
    
    // ustawiamy jako detached
    if (pthread_attr_setdetachstate(&attr3, PTHREAD_CREATE_DETACHED) != 0) {
        printf("Nie udalo sie ustawic detached\n");
    }
    
    // ustawiamy mniejszy stos - 64KB (ale nie mniej niz minimum)
    size_t min_stack = PTHREAD_STACK_MIN;
    size_t small_stack = (64 * 1024 > min_stack) ? 64 * 1024 : min_stack;
    
    // alokujemy wlasny stos
    void* custom_stack = malloc(small_stack);
    if (custom_stack == NULL) {
        printf("Nie udalo sie zaalokowac stosu\n");
        exit(1);
    }
    
    printf("Alokowany stos pod adresem: %p, rozmiar: %zu bytes\n", custom_stack, small_stack);
    
    // ustawiamy adres i rozmiar stosu
    if (pthread_attr_setstack(&attr3, custom_stack, small_stack) != 0) {
        printf("Nie udalo sie ustawic adresu stosu\n");
        free(custom_stack);
    }
    
    wypisz_atrybuty(&attr3, 3);
    
    DaneOsoby* osoba3 = malloc(sizeof(DaneOsoby));
    if (osoba3 == NULL) {
        printf("Nie udalo sie zarezerwowac pamieci\n");
        exit(1);
    }
    
    osoba3->id_watku = 3;
    strcpy(osoba3->imie, "Piotr");
    strcpy(osoba3->nazwisko, "Detached");
    osoba3->wiek = 35;
    osoba3->detached = 1;
    osoba3->stack_size = small_stack;
    osoba3->custom_stack_addr = custom_stack;  // zapisujemy adres custom stosu
    
    if (pthread_create(&watek3, &attr3, funkcja_detached_watku, osoba3) != 0) {
        printf("Nie udalo sie stworzyc watku 3\n");
        free(osoba3);
        exit(1);
    }
    
    printf("Wszystkie watki utworzone!\n");
    printf("Watek 3 jest DETACHED - nie mozemy na niego czekac\n\n");
    
    // czekamy na watek 1 (joinable)
    printf("Czekam na watek 1...\n");
    if (pthread_join(watek1, NULL) != 0) {
        printf("Blad przy czekaniu na watek 1\n");
    } else {
        printf("Watek 1 zakonczony\n");
    }
    
    // czekamy na watek 2 (joinable)
    printf("Czekam na watek 2...\n");
    if (pthread_join(watek2, NULL) != 0) {
        printf("Blad przy czekaniu na watek 2\n");
    } else {
        printf("Watek 2 zakonczony\n");
    }
    
    // nie mozemy czekac na watek 3 bo jest detached
    printf("Nie czekam na watek 3 (detached)\n");
    
    // dajemy czas watek 3 na zakonczenie
    printf("Czekam 4 sekundy zeby watek 3 mogl sie zakonczyc...\n");
    sleep(4);
    
    // sprzatamy
    free(osoba2);
    // osoba3 zostala zwolniona przez sam watek detached
    
    pthread_attr_destroy(&attr1);
    pthread_attr_destroy(&attr2);
    pthread_attr_destroy(&attr3);
    
    printf("\nProgram zakonczony!\n");
    
    return 0;
}
