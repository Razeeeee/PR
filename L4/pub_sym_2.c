#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

struct struktura_t { 
  int l_wkf;
  pthread_mutex_t *tab_kuf; 
  int l_kr;
  pthread_mutex_t *tab_kran;  
};
struct struktura_t pub_wsk;

// Zmienna wspólna S2 - liczba wypitych kufli (z aktywnym czekaniem)
int S2 = 0;
pthread_mutex_t mutex_S2 = PTHREAD_MUTEX_INITIALIZER;

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

  printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  pub_wsk.l_kr = l_kr;

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;

  pub_wsk.tab_kuf = (pthread_mutex_t *) malloc(l_kf*sizeof(pthread_mutex_t));
  for(i=0;i<l_kf;i++) pthread_mutex_init(&pub_wsk.tab_kuf[i], NULL);

  pub_wsk.tab_kran = (pthread_mutex_t *) malloc(l_kr*sizeof(pthread_mutex_t));
  for(i=0;i<l_kr;i++) pthread_mutex_init(&pub_wsk.tab_kran[i], NULL);

  printf("\nOtwieramy pub (z aktywnym czekaniem)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 
  printf("Liczba kranów %d\n", l_kr);
  printf("Śledzenie liczby kufli: WŁĄCZONE (aktywne czekanie na zasób - S2)\n");

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
  for(i=0;i<l_kf;i++) pthread_mutex_destroy(&pub_wsk.tab_kuf[i]);
  for(i=0;i<l_kr;i++) pthread_mutex_destroy(&pub_wsk.tab_kran[i]);
  pthread_mutex_destroy(&mutex_S2);
  
  free(pub_wsk.tab_kuf);
  free(pub_wsk.tab_kran);
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

    // AKTYWNE CZEKANIE NA KUFEL - do {} while() z lock/unlock
    do {
      kufel = -1; // Brak kufla
      
      // Przeszukujemy wszystkie kufle w poszukiwaniu wolnego
      for(int j = 0; j < pub_wsk.l_wkf; j++) {
        if(pthread_mutex_trylock(&pub_wsk.tab_kuf[j]) == 0) {
          kufel = j;
          printf("\nKlient %d, znalazłem wolny kufel %d\n", moj_id, kufel);
          break; // Znaleźliśmy wolny kufel, wychodzimy z pętli
        }
      }
      
      if(kufel == -1) {
        // Nie znaleziono wolnego kufla - czekamy losową liczbę sekund
        losowy_czas_czekania = 1 + rand() % 3; // 1-3 sekundy
        printf("\nKlient %d, brak wolnych kufli, czekam %d sek\n", moj_id, losowy_czas_czekania);
        sleep(losowy_czas_czekania);
      }
      
    } while(kufel == -1);

    printf("\nKlient %d, wybrałem kufel %d\n", moj_id, kufel);

    printf("\nKlient %d, szukam wolnego kranu\n", moj_id); 

    // AKTYWNE CZEKANIE NA KRAN - do {} while() z lock/unlock
    do {
      kran = -1; // Brak kranu
      
      // Przeszukujemy wszystkie krany w poszukiwaniu wolnego
      for(int j = 0; j < pub_wsk.l_kr; j++) {
        if(pthread_mutex_trylock(&pub_wsk.tab_kran[j]) == 0) {
          kran = j;
          printf("\nKlient %d, znalazłem wolny kran %d\n", moj_id, kran);
          break; // Znaleźliśmy wolny kran, wychodzimy z pętli
        }
      }
      
      if(kran == -1) {
        // Nie znaleziono wolnego kranu - czekamy losową liczbę sekund
        losowy_czas_czekania = 1 + rand() % 2; // 1-2 sekundy
        printf("\nKlient %d, brak wolnych kranów, czekam %d sek\n", moj_id, losowy_czas_czekania);
        sleep(losowy_czas_czekania);
      }
      
    } while(kran == -1);
    
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, kran); 
    usleep(100000); // 100ms na nalewanie

    // Zwalniamy kran po nalewaniu
    pthread_mutex_unlock(&pub_wsk.tab_kran[kran]);
    printf("\nKlient %d, zwolniłem kran %d\n", moj_id, kran);

    // Picie piwa
    if(kran==0)  printf("\nKlient %d, pije piwo Guinness\n", moj_id); 
    else if(kran==1)  printf("\nKlient %d, pije piwo Żywiec\n", moj_id); 
    else if(kran==2)  printf("\nKlient %d, pije piwo Heineken\n", moj_id); 
    else if(kran==3)  printf("\nKlient %d, pije piwo Okocim\n", moj_id); 
    else if(kran==4)  printf("\nKlient %d, pije piwo Karlsberg\n", moj_id); 
    else printf("\nKlient %d, pije piwo z kranu %d\n", moj_id, kran);

    usleep(200000); // 200ms na picie

    // ZABEZPIECZONY dostęp do zmiennej wspólnej S2
    pthread_mutex_lock(&mutex_S2);
    S2++;
    printf("\nKlient %d, wypił kufel (łącznie wypito: S2=%d)\n", moj_id, S2);
    pthread_mutex_unlock(&mutex_S2);

    // Zwracamy kufel
    pthread_mutex_unlock(&pub_wsk.tab_kuf[kufel]);
    printf("\nKlient %d, oddałem kufel %d\n", moj_id, kufel); 

  }

  printf("\nKlient %d, wychodzę z pubu\n", moj_id);
  return(NULL);
} 
