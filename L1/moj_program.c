#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#include "pomiar_czasu.h"

#define POMIAR_CZASU
 
const int liczba = 100000;

int main(){

  double a, b, c;
  int i,j,k,l;

  k = -50000;
  #ifdef POMIAR_CZASU
  inicjuj_czas();
  for(i=0;i<liczba-1;i++){

    printf("%d ",k+i);

  }
  printf("\nCzas wykonania %d operacji wejscia/wyjscia: \n",liczba);
  drukuj_czas();
  #else
  inicjuj_czas();
  for(i=0;i<liczba;i++){

    printf("%d ",k+i);

  }
  printf("\nCzas wykonania %d operacji wejscia/wyjscia: \n",liczba);
  drukuj_czas();
  #endif

  a = 1.000001;
  double t1 = czas_zegara(), t2 = czas_CPU();
  for(i=0;i<liczba;i++){

    a = 1.000001*a+0.000001; 

  }

  t1 = czas_zegara() - t1;
  t2 = czas_CPU() - t2;

  printf("Wynik operacji arytmetycznych: %lf\n", a);
  #ifdef POMIAR_CZASU
  printf("Czas wykonania %d operacji arytmetycznych: \n",liczba);
  printf("czas CPU         = %lf\n",t2);
  printf("czas zegarowy    = %lf\n",t1);
  #endif

  return 0;
}
