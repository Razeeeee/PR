#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 10

int main ()
{
  double a[WYMIAR][WYMIAR];

  for(int i=0;i<WYMIAR;i++) for(int j=0;j<WYMIAR;j++) a[i][j]=1.02*i+1.01*j;

  double suma=0.0;
  for(int i=0;i<WYMIAR;i++) {
    for(int j=0;j<WYMIAR;j++) {
      suma += a[i][j];
    }
  }
  
  printf("Suma wyrazow tablicy: %lf\n", suma);

  omp_set_nested(1);

  double suma_parallel=0.0; int i,j;

  for(i=0;i<WYMIAR;i++) {
    int id_w = omp_get_thread_num();

    for(j=0;j<WYMIAR;j++) {
      suma_parallel += a[i][j];

    }
    printf("\n");
  }

  printf("Suma wyrazow tablicy rownolegle: %lf\n", suma_parallel);

}