#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

struct struktura_t { 
  int l_wkf;
  int l_kr;
};
struct struktura_t pub_wsk;

// Zmienna wspólna S2 - liczba wypitych kufli (z aktywnym czekaniem)
int S2 = 0;
pthread_mutex_t mutex_S2 = PTHREAD_MUTEX_INITIALIZER;

// Licznik dostępnych kufli (aktywne czekanie)
int dostepne_kufle = 0;
pthread_mutex_t mutex_kufle = PTHREAD_MUTEX_INITIALIZER;

// Licznik dostępnych kranów (aktywne czekanie)
int dostepne_krany = 0;
pthread_mutex_t mutex_krany = PTHREAD_MUTEX_INITIALIZER;

void * watek_klient (void * arg);

int main( void ){
  pthread_t *tab_klient;
  int *tab_klient_id;

  int l_kl, l_kf, l_kr, i;

  // Inicjalizacja generatora liczb losowych
  srand(time(NULL));

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);
  pub_wsk.l_wkf = l_kf;
  dostepne_kufle = l_kf; // Inicjalizacja licznika dostępnych kufli

  printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  pub_wsk.l_kr = l_kr;
  dostepne_krany = l_kr; // Inicjalizacja licznika dostępnych kranów

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;

  // Usunięto inicjalizację poszczególnych mutexów kranów - używamy licznika

  printf("\nOtwieramy pub (z aktywnym czekaniem)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 
  printf("Liczba kranów %d\n", l_kr);
  printf("Śledzenie liczby kufli: WŁĄCZONE (aktywne czekanie - S2)\n");
  printf("Śledzenie liczby kranów: WŁĄCZONE (aktywne czekanie)\n");

  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  
  printf("\n=== WYNIKI SYMULACJI (aktywne czekanie) ===\n");
  printf("Łączna liczba wypitych kufli (S2): %d\n", S2);
  printf("Oczekiwana liczba kufli: %d\n", l_kl * ILE_MUSZE_WYPIC);
  printf("Różnica (powinno być 0 z aktywnym czekaniem): %d\n", (l_kl * ILE_MUSZE_WYPIC) - S2);
  printf("\nZamykamy pub!\n");

  // Zniszczenie mutexów
  pthread_mutex_destroy(&mutex_S2);
  pthread_mutex_destroy(&mutex_kufle);
  pthread_mutex_destroy(&mutex_krany);
  free(tab_klient);
  free(tab_klient_id);

  return 0;
}

void * watek_klient (void * arg_wsk){

  int moj_id = * ((int *)arg_wsk);
  int i_wypite, kran, kufel;
  int ile_musze_wypic = ILE_MUSZE_WYPIC;
  int losowy_czas_czekania;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i_wypite=0; i_wypite<ile_musze_wypic; i_wypite++){

    printf("\nKlient %d, szukam wolnego kufla\n", moj_id); 

    // Aktywne czekanie na dostępny kufel
    while(1) {
      pthread_mutex_lock(&mutex_kufle);
      if(dostepne_kufle > 0) {
        dostepne_kufle--;
        kufel = dostepne_kufle;
        printf("\nKlient %d, zarezerwowałem kufel (pozostało: %d)\n", moj_id, dostepne_kufle);
        pthread_mutex_unlock(&mutex_kufle);
        break;
      }
      pthread_mutex_unlock(&mutex_kufle);
      usleep(1000);
    }

    printf("\nKlient %d, wybrałem kufel %d\n", moj_id, kufel);

    printf("\nKlient %d, szukam wolnego kranu\n", moj_id); 

    // Aktywne czekanie na dostępny kran
    while(1) {
      pthread_mutex_lock(&mutex_krany);
      if(dostepne_krany > 0) {
        dostepne_krany--;
        kran = dostepne_krany;
        printf("\nKlient %d, zarezerwowałem kran (pozostało: %d)\n", moj_id, dostepne_krany);
        pthread_mutex_unlock(&mutex_krany);
        break;
      }
      pthread_mutex_unlock(&mutex_krany);
      usleep(1000);
    }
    
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, kran);

    // Oddanie kranu - zwiększenie licznika dostępnych kranów
    pthread_mutex_lock(&mutex_krany);
    dostepne_krany++;
    printf("\nKlient %d, zwolniłem kran (dostępnych: %d)\n", moj_id, dostepne_krany);
    pthread_mutex_unlock(&mutex_krany);

    if(kran==0)  printf("\nKlient %d, pije piwo Guinness\n", moj_id); 
    else if(kran==1)  printf("\nKlient %d, pije piwo Żywiec\n", moj_id); 
    else if(kran==2)  printf("\nKlient %d, pije piwo Heineken\n", moj_id); 
    else if(kran==3)  printf("\nKlient %d, pije piwo Okocim\n", moj_id); 
    else if(kran==4)  printf("\nKlient %d, pije piwo Karlsberg\n", moj_id); 
    else printf("\nKlient %d, pije piwo z kranu %d\n", moj_id, kran);

    pthread_mutex_lock(&mutex_S2);
    S2++;
    printf("\nKlient %d, wypił kufel (łącznie wypito: S2=%d)\n", moj_id, S2);
    pthread_mutex_unlock(&mutex_S2);

    // Oddanie kufla - zwiększenie licznika dostępnych kufli
    pthread_mutex_lock(&mutex_kufle);
    dostepne_kufle++;
    printf("\nKlient %d, oddałem kufel (dostępnych: %d)\n", moj_id, dostepne_kufle);
    pthread_mutex_unlock(&mutex_kufle); 

  }

  printf("\nKlient %d, wychodzę z pubu\n", moj_id);
  return(NULL);
} 
