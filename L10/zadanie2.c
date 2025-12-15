/************************************************************************
Zadanie 3.5: Wykorzystanie threadprivate do przenoszenia informacji 
             pomiędzy kolejnymi blokami równoległymi

Program demonstruje użycie threadprivate do zachowania wartości zmiennych
między różnymi regionami równoległymi.
*************************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

// Zmienne globalne z atrybutem threadprivate
int thread_counter = 0;
double thread_sum = 0.0;
int thread_id_saved = -1;

#pragma omp threadprivate(thread_counter, thread_sum, thread_id_saved)

int main(){
  
#ifdef   _OPENMP
  printf("\nKompilator rozpoznaje dyrektywy OpenMP\n");
#endif

  omp_set_num_threads(5);
  
  int global_counter = 0;
  
  printf("\n=== PIERWSZY BLOK ROWNOLEGLLY ===\n");
  
  // Pierwszy blok równoległy - inicjalizacja zmiennych threadprivate
#pragma omp parallel default(none) shared(global_counter)
  {
    int tid = omp_get_thread_num();
    
    // Inicjalizacja zmiennych threadprivate
    thread_id_saved = tid;
    thread_counter = 0;
    thread_sum = 0.0;
    
    // Każdy wątek wykonuje pewne obliczenia
    for(int i = 0; i < 10; i++){
      thread_counter++;
      thread_sum += (double)(tid + 1) * (i + 1);
    }
    
#pragma omp critical
    {
      printf("Watek %d w bloku 1: thread_id_saved=%d, counter=%d, sum=%.2f\n", 
             tid, thread_id_saved, thread_counter, thread_sum);
    }
  }
  
  printf("\n=== DRUGI BLOK ROWNOLEGLLY ===\n");
  
  // Drugi blok równoległy - wykorzystanie zachowanych wartości
#pragma omp parallel default(none)
  {
    int tid = omp_get_thread_num();
    
#pragma omp critical
    {
      printf("Watek %d w bloku 2: thread_id_saved=%d (zachowane z bloku 1), counter=%d, sum=%.2f\n",
             tid, thread_id_saved, thread_counter, thread_sum);
    }
  }
  
  printf("\nProgram zakonczony.\n");
  
  return 0;
}