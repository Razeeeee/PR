#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

void * watek_klient (void * arg);

// Zmienna wspólna S0 - liczba wypitych kufli (bez zabezpieczenia dostępu)
int S0 = 0;

int main( void ){

  pthread_t *tab_klient;
  int *tab_klient_id;

  int l_kl, l_kf, l_kr, i;

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);

  //printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  l_kr = 1000000000; // wystarczająco dużo, żeby nie było rywalizacji 

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;

  printf("\nOtwieramy pub (simple)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 
  printf("Śledzenie liczby kufli: WŁĄCZONE (bez zabezpieczenia dostępu do S0)\n");

  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  
  printf("\n=== WYNIKI SYMULACJI ===\n");
  printf("Łączna liczba wypitych kufli (S0): %d\n", S0);
  printf("Oczekiwana liczba kufli: %d\n", l_kl * ILE_MUSZE_WYPIC);
  printf("Różnica (utracone aktualizacje): %d\n", (l_kl * ILE_MUSZE_WYPIC) - S0);
  printf("\nZamykamy pub!\n");

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
    
    j=0;
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, j); 
    
    printf("\nKlient %d, pije\n", moj_id); 
    
    int temp_S0 = S0;
    usleep(1);
    S0 = temp_S0 + 1;
    
    printf("\nKlient %d, odkladam kufel (S0=%d)\n", moj_id, S0); 
    
  }

  printf("\nKlient %d, wychodzę z pubu; wykonana praca %ld\n",
	 moj_id, wykonana_praca); 
    
  return(NULL);
} 
