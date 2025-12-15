/************************************************************************
Zadanie 3.0: Analiza i eliminacja zależności w openmp_watki_zmienne.c

Program analizuje i eliminuje zależności przenoszonych przez zmienne:
- WAR (Write After Read) - zapis po odczycie
- WAW (Write After Write) - zapis po zapisie  
- RAW (Read After Write) - odczyt po zapisie
*************************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int main(){
  
#ifdef   _OPENMP
  printf("\nKompilator rozpoznaje dyrektywy OpenMP\n");
#endif

  int liczba_watkow;
  
  int a_shared = 1;
  int b_private = 2;
  int c_firstprivate = 3;
  int e_atomic = 5;
  
  
  printf("Przed wejsciem do obszaru rownoleglego:\n");
  printf("\ta_shared \t= %d\n", a_shared);
  printf("\tb_private \t= %d\n", b_private);
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  printf("\te_atomic \t= %d\n", e_atomic);
    
  
#pragma omp parallel default(none) shared(a_shared, e_atomic) private(b_private) firstprivate(c_firstprivate)
  {
    int i;
    int d_local_private;
    
    // WAR
    // W dalszej części kodu zmienna a_shared jest modyfikowana przed odczytem jej wartości do d_local_private
    d_local_private = a_shared + c_firstprivate;
    #pragma omp barrier
    
    // RAW / WAW między kolejnymi iteracjami pętli modyfikującej a_shared
    // Wątek musi odczytać, dodać 1 i zapisać
    // Jeśli jakiś wątek zapisze a_shared w międzyczasie, to wynik będzie błędny
    // Critical
    #pragma omp critical
    {
      for(i=0;i<10;i++){
        a_shared++;
        // temp = a_shared; a_shared = temp + 1;
      }
    }

    for(i=0;i<10;i++){
      c_firstprivate += omp_get_thread_num();
    }

    // RAW / WAW między kolejnymi iteracjami pętli modyfikującej e_atomic
    // Wątek musi odczytać, dodać 1 i zapisać
    // Jeśli jakiś wątek zapisze e_atomic w międzyczasie, to wynik będzie błędny
    // Atomic
    for(i=0;i<10;i++){
      #pragma omp atomic
      e_atomic += omp_get_thread_num();
      // temp = e_atomic; e_atomic = temp + omp_get_thread_num();
    }

#pragma omp barrier
    
#pragma omp critical
    {
      printf("Watek %d: a_shared=%d, b_private=%d, c_firstprivate=%d, d_local_private=%d, e_atomic=%d\n",
	     omp_get_thread_num(), a_shared, b_private, c_firstprivate, d_local_private, e_atomic);
    }
    
  }
  
  printf("\nPo zakonczeniu obszaru rownoleglego:\n");
  printf("\ta_shared \t= %d\n", a_shared);
  printf("\tb_private \t= %d\n", b_private);
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  printf("\te_atomic \t= %d\n", e_atomic);
  
  return 0;
}