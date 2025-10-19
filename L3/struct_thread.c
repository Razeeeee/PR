#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define ILE_WATKOW 3

// dane ktore wysylamy do watku
typedef struct {
    int id_watku;
    char imie[50];
    char nazwisko[50];
    int wiek;
} DaneOsoby;

// to robi kazdy watek
void* funkcja_watku(void* arg) {
    // zmieniamy arg na nasza strukture
    DaneOsoby* dane = (DaneOsoby*)arg;
    
    // id watku w systemie
    pthread_t system_id = pthread_self();
    
    // wypisujemy dane osoby
    printf("--- WATEK %d ---\n", dane->id_watku);
    printf("ID: %lu\n", (unsigned long)system_id);
    printf("Imie: %s\n", dane->imie);
    printf("Nazwisko: %s\n", dane->nazwisko);
    printf("Wiek: %d\n", dane->wiek);
    printf("---\n\n");
    
    // robimy cos przez sekunde
    sleep(1);
    
    printf("Watek %d (%s %s) konczy\n", dane->id_watku, dane->imie, dane->nazwisko);
    
    return NULL;
}

int main() {
    pthread_t watki[ILE_WATKOW];
    DaneOsoby osoby[ILE_WATKOW];
    int i;
    
    printf("Program - wysylanie struktury do watkow\n");
    printf("=======================================\n\n");
    
    // ustawiamy dane dla kazdego watku
    strcpy(osoby[0].imie, "Jan");
    strcpy(osoby[0].nazwisko, "Kowalski");
    osoby[0].wiek = 25;
    osoby[0].id_watku = 0;
    
    strcpy(osoby[1].imie, "Anna");
    strcpy(osoby[1].nazwisko, "Nowak");
    osoby[1].wiek = 30;
    osoby[1].id_watku = 1;
    
    strcpy(osoby[2].imie, "Piotr");
    strcpy(osoby[2].nazwisko, "Wisniewski");
    osoby[2].wiek = 22;
    osoby[2].id_watku = 2;
    
    printf("Tworze %d watkow...\n\n", ILE_WATKOW);
    
    // tworzymy watki - wysylamy adres struktury
    for (i = 0; i < ILE_WATKOW; i++) {
        printf("Tworze watek dla: %s %s\n", osoby[i].imie, osoby[i].nazwisko);
        
        // wysylamy adres struktury do watku
        if (pthread_create(&watki[i], NULL, funkcja_watku, &osoby[i]) != 0) {
            printf("Nie udalo sie stworzyc watku %d\n", i);
            exit(1);
        }
    }
    
    printf("\nWszystkie watki utworzone. Czekam...\n\n");
    
    // czekamy az wszystkie watki skoncza
    for (i = 0; i < ILE_WATKOW; i++) {
        if (pthread_join(watki[i], NULL) != 0) {
            printf("Blad przy czekaniu na watek %d\n", i);
            exit(1);
        }
    }
    
    printf("\nWszystkie watki skonczone.\n");
    
    return 0;
}
