#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

void * watek_klient (void * arg);

// Zmienna wspólna S1 - liczba wypitych kufli (z zabezpieczeniem mutex)
int S1 = 0;
pthread_mutex_t mutex_S1 = PTHREAD_MUTEX_INITIALIZER;

// Zmienne globalne dla liczby kufli i klientów
int liczba_kufli_dostepnych;
pthread_mutex_t mutex_kufle = PTHREAD_MUTEX_INITIALIZER;

int main( void ){

  pthread_t *tab_klient;
  int *tab_klient_id;

  int l_kl, l_kf, l_kr, i;

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);
  liczba_kufli_dostepnych = l_kf;

  //printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  l_kr = 1000000000; // wystarczająco dużo, żeby nie było rywalizacji 

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;

  printf("\nOtwieramy pub (z zabezpieczeniem mutex)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 
  printf("Śledzenie liczby kufli: WŁĄCZONE (z zabezpieczeniem mutex dla S1)\n");
  printf("UWAGA: Brak sprawdzania warunku na ujemne kufle!\n");

  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  
  printf("\n=== WYNIKI SYMULACJI (z mutex) ===\n");
  printf("Łączna liczba wypitych kufli (S1): %d\n", S1);
  printf("Oczekiwana liczba kufli: %d\n", l_kl * ILE_MUSZE_WYPIC);
  printf("Różnica (powinno być 0 z mutex): %d\n", (l_kl * ILE_MUSZE_WYPIC) - S1);
  printf("Końcowa liczba dostępnych kufli: %d\n", liczba_kufli_dostepnych);
  printf("\nZamykamy pub!\n");

  // Zniszczenie mutexów
  pthread_mutex_destroy(&mutex_S1);
  pthread_mutex_destroy(&mutex_kufle);
  
  free(tab_klient);
  free(tab_klient_id);

  return 0;
}


void * watek_klient (void * arg_wsk){

  int moj_id = * ((int *)arg_wsk);

  int i, j, kufel, result;
  int ile_musze_wypic = ILE_MUSZE_WYPIC;

  long int wykonana_praca = 0;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i=0; i<ile_musze_wypic; i++){

    printf("\nKlient %d, wybieram kufel\n", moj_id); 
    
    // ZABEZPIECZONY dostęp do kufli (lock/unlock)
    pthread_mutex_lock(&mutex_kufle);
    
    // UWAGA: Nie sprawdzamy warunku na ujemne kufle zgodnie z wymaganiem!
    liczba_kufli_dostepnych--;
    printf("\nKlient %d, wziąłem kufel (zostało: %d)\n", moj_id, liczba_kufli_dostepnych);
    
    pthread_mutex_unlock(&mutex_kufle);
    
    j=0;
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, j); 
    usleep(30);
    
    printf("\nKlient %d, pije\n", moj_id); 
    nanosleep((struct timespec[]){{0, 50000000L}}, NULL);
    
    // ZABEZPIECZONY dostęp do zmiennej wspólnej S1 (lock/unlock)
    pthread_mutex_lock(&mutex_S1);
    S1++;
    printf("\nKlient %d, wypił kufel (łącznie wypito: S1=%d)\n", moj_id, S1);
    pthread_mutex_unlock(&mutex_S1);
    
    // ZABEZPIECZONY zwrot kufla (lock/unlock)
    pthread_mutex_lock(&mutex_kufle);
    liczba_kufli_dostepnych++;
    printf("\nKlient %d, odkladam kufel (dostępne: %d)\n", moj_id, liczba_kufli_dostepnych); 
    pthread_mutex_unlock(&mutex_kufle);
    
  }

  printf("\nKlient %d, wychodzę z pubu; wykonana praca %ld\n",
	 moj_id, wykonana_praca); 
    
  return(NULL);
} 
