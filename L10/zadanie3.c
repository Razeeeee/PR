/************************************************************************
Zadanie 4.0: Wykorzystanie zadań (task) do realizacji sortowania
             i analiza wydajności różnych technik zrównoleglenia

Program porównuje wydajność różnych metod sortowania:
1. Sortowanie sekwencyjne (merge sort)
2. Sortowanie sekwencyjne (quick sort)
3. Sortowanie równoległe z użyciem task (merge sort)
4. Sortowanie równoległe z użyciem task + final (merge sort)
5. Sortowanie równoległe z użyciem sections (merge sort)
6. Sortowanie hybrydowe (task + quicksort dla małych fragmentów)
*************************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<omp.h>
#include<string.h>
#include"sortowanie_seq.h"

// Parametr ograniczający liczbę tworzonych zadań
#define MAX_POZIOM 5
#define MIN_SIZE_QUICKSORT 1000

/*** Funkcje sortowania równoległego ***/

// 1. Sortowanie równoległe z użyciem task
void merge_sort_task(double* A, int p, int r, int poziom)
{
  if(p < r){
    poziom++;
    int q = (p + r) / 2;
    
#pragma omp task default(none) firstprivate(A, p, q, poziom) final(poziom > MAX_POZIOM)
    {
      merge_sort_task(A, p, q, poziom);
    }

#pragma omp task default(none) firstprivate(A, q, r, poziom) final(poziom > MAX_POZIOM)
    {
      merge_sort_task(A, q+1, r, poziom);
    }

#pragma omp taskwait
    
    scal(A, p, q, r);
  }
}

// 2. Sortowanie hybrydowe - task + quicksort dla małych fragmentów
void merge_sort_hybrid(double* A, int p, int r, int poziom)
{
  if(p < r){
    poziom++;
    
    // Dla małych fragmentów użyj quicksort
    if(r - p < MIN_SIZE_QUICKSORT || poziom > MAX_POZIOM){
      sortowanie_szybkie(A, p, r);
      return;
    }
    
    int q = (p + r) / 2;
    
#pragma omp task default(none) firstprivate(A, p, q, poziom)
    {
      merge_sort_hybrid(A, p, q, poziom);
    }

#pragma omp task default(none) firstprivate(A, q, r, poziom)
    {
      merge_sort_hybrid(A, q+1, r, poziom);
    }

#pragma omp taskwait
    
    scal(A, p, q, r);
  }
}

// 3. Sortowanie z użyciem sections
void merge_sort_sections(double* A, int p, int r, int poziom)
{
  poziom++;
  
  if(poziom > MAX_POZIOM){
    sortowanie_przez_scalanie(A, p, r);
  }
  else{
    if(p < r){
      int q = (p + r) / 2;

#pragma omp parallel sections default(none) firstprivate(A, p, r, q, poziom)
      {
#pragma omp section
        {
          merge_sort_sections(A, p, q, poziom);
        }

#pragma omp section
        {
          merge_sort_sections(A, q+1, r, poziom);
        }
      }
      
      scal(A, p, q, r);
    }
  }
}

// 4. Sortowanie równoległe z użyciem task i if dla warunkowego tworzenia zadań
void merge_sort_task_if(double* A, int p, int r, int poziom)
{
  if(p < r){
    poziom++;
    int q = (p + r) / 2;
    int create_tasks = (poziom <= MAX_POZIOM);
    
#pragma omp task default(none) firstprivate(A, p, q, poziom) if(create_tasks)
    {
      merge_sort_task_if(A, p, q, poziom);
    }

#pragma omp task default(none) firstprivate(A, q, r, poziom) if(create_tasks)
    {
      merge_sort_task_if(A, q+1, r, poziom);
    }

#pragma omp taskwait
    
    scal(A, p, q, r);
  }
}

/*** Funkcje pomocnicze ***/

void generuj_tablice(double* A, int n)
{
  for(int i = 0; i < n; i++){
    A[i] = (double)rand() / RAND_MAX * 1000.0;
  }
}

int sprawdz_sortowanie(double* A, int n)
{
  for(int i = 0; i < n-1; i++){
    if(A[i] > A[i+1]){
      printf("   BLAD sortowania!\n");
      return 0;
    }
  }
  return 1;
}

/*** Program główny ***/

int main(int argc, char** argv)
{
  int n = 1000000; // Domyślny rozmiar tablicy
  int num_threads = 4; // Domyślna liczba wątków
  
  if(argc > 1) n = atoi(argv[1]);
  if(argc > 2) num_threads = atoi(argv[2]);
  
  omp_set_num_threads(num_threads);
  
  printf("Rozmiar tablicy: %d\n", n);
  printf("Liczba watkow: %d\n\n", num_threads);
  
  double *A_orig = (double*)malloc(n * sizeof(double));
  double *A_test = (double*)malloc(n * sizeof(double));
  double t1, t2;
  
  // Generuj dane
  srand(12345);
  generuj_tablice(A_orig, n);
  
  // TEST 1: Sortowanie sekwencyjne merge sort
  printf("1. Sortowanie sekwencyjne (merge sort):\n");
  memcpy(A_test, A_orig, n * sizeof(double));
  t1 = omp_get_wtime();
  sortowanie_przez_scalanie(A_test, 0, n-1);
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s\n", t2);
  double t_seq_merge = t2;
  sprawdz_sortowanie(A_test, n);
  
  // TEST 2: Sortowanie sekwencyjne quick sort
  printf("2. Sortowanie sekwencyjne (quick sort):\n");
  memcpy(A_test, A_orig, n * sizeof(double));
  t1 = omp_get_wtime();
  sortowanie_szybkie(A_test, 0, n-1);
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s, przyspieszenie: %.2fx\n", t2, t_seq_merge / t2);
  double t_seq_quick = t2;
  sprawdz_sortowanie(A_test, n);
  
  // TEST 3: Sortowanie równoległe z task
  printf("3. Sortowanie rownolegle (task + merge sort):\n");
  memcpy(A_test, A_orig, n * sizeof(double));
  t1 = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single
    {
      merge_sort_task(A_test, 0, n-1, 0);
    }
  }
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s, przyspieszenie: %.2fx, efektywnosc: %.1f%%\n", 
         t2, t_seq_merge / t2, (t_seq_merge / t2) / num_threads * 100);
  sprawdz_sortowanie(A_test, n);
  
  // TEST 4: Sortowanie hybrydowe task + quicksort
  printf("4. Sortowanie hybrydowe (task + quicksort):\n");
  memcpy(A_test, A_orig, n * sizeof(double));
  t1 = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single
    {
      merge_sort_hybrid(A_test, 0, n-1, 0);
    }
  }
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s, przyspieszenie: %.2fx, efektywnosc: %.1f%%\n",
         t2, t_seq_quick / t2, (t_seq_quick / t2) / num_threads * 100);
  sprawdz_sortowanie(A_test, n);
  
  // TEST 5: Sortowanie z sections
  printf("5. Sortowanie rownolegle (sections + merge sort):\n");
  memcpy(A_test, A_orig, n * sizeof(double));
  t1 = omp_get_wtime();
  merge_sort_sections(A_test, 0, n-1, 0);
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s, przyspieszenie: %.2fx, efektywnosc: %.1f%%\n",
         t2, t_seq_merge / t2, (t_seq_merge / t2) / num_threads * 100);
  sprawdz_sortowanie(A_test, n);

  free(A_orig);
  free(A_test);
  
  return 0;
}
