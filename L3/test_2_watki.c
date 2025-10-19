#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// nasza struktura z danymi osoby
typedef struct {
    int id_watku;
    char imie[50];
    char nazwisko[50];
    int wiek;
} DaneOsoby;

// funkcja watku - to co robi kazdy watek
void* funkcja_watku(void* arg) {
    // zmieniamy arg na nasza strukture
    DaneOsoby* dane = (DaneOsoby*)arg;
    
    // sprawdzamy id watku w systemie
    pthread_t system_id = pthread_self();
    
    // wypisujemy wszystkie dane
    printf("--- WATEK %d ---\n", dane->id_watku);
    printf("ID systemu: %lu\n", (unsigned long)system_id);
    printf("Imie: %s\n", dane->imie);
    printf("Nazwisko: %s\n", dane->nazwisko);
    printf("Wiek: %d\n", dane->wiek);
    printf("Adres: %p\n", (void*)dane);
    printf("---\n\n");
    
    // watek robi cos przez 2 sek
    sleep(2);
    
    printf("Watek %d (%s %s) konczy\n", dane->id_watku, dane->imie, dane->nazwisko);
    
    return NULL;
}

int main() {
    pthread_t watek1, watek2;
    
    printf("Test 2 watkow - lokalny obiekt vs malloc\n");
    printf("========================================\n\n");
    
    // SPOSOB 1: zwykla zmienna na stosie
    printf("--- SPOSOB 1: zwykla zmienna ---\n");
    DaneOsoby osoba1;  // zwykla zmienna
    osoba1.id_watku = 1;
    strcpy(osoba1.imie, "Jan");
    strcpy(osoba1.nazwisko, "Kowalski");
    osoba1.wiek = 25;
    
    printf("Adres zmiennej: %p\n", (void*)&osoba1);
    printf("Tworze watek 1...\n\n");
    
    if (pthread_create(&watek1, NULL, funkcja_watku, &osoba1) != 0) {
        printf("Nie udalo sie stworzyc watku 1\n");
        exit(1);
    }
    
    // SPOSOB 2: malloc - rezerwujemy pamiec recznie
    printf("--- SPOSOB 2: malloc ---\n");
    DaneOsoby* osoba2 = malloc(sizeof(DaneOsoby));  // rezerwujemy pamiec
    if (osoba2 == NULL) {
        printf("Nie udalo sie zarezerwowac pamieci\n");
        exit(1);
    }
    
    osoba2->id_watku = 2;
    strcpy(osoba2->imie, "Anna");
    strcpy(osoba2->nazwisko, "Nowak");
    osoba2->wiek = 30;
    
    printf("Adres malloc: %p\n", (void*)osoba2);
    printf("Tworze watek 2...\n\n");
    
    if (pthread_create(&watek2, NULL, funkcja_watku, osoba2) != 0) {
        printf("Nie udalo sie stworzyc watku 2\n");
        free(osoba2);
        exit(1);
    }
    
    printf("Oba watki utworzone. Czekam...\n\n");
    
    // czekamy az watek 1 skonczy
    if (pthread_join(watek1, NULL) != 0) {
        printf("Blad przy czekaniu na watek 1\n");
        free(osoba2);
        exit(1);
    }
    
    // czekamy az watek 2 skonczy
    if (pthread_join(watek2, NULL) != 0) {
        printf("Blad przy czekaniu na watek 2\n");
        free(osoba2);
        exit(1);
    }
    
    // zwalniamy pamiec z malloc
    free(osoba2);
    
    return 0;
}
