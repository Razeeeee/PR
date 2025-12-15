/************************************************************************
Modyfikacja programu z wyszukiwaniem binarnym 
o mechanizm szybkiego sortowania liniowego
*************************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<omp.h>
#include"sortowanie_seq.h"

#define ROZMIAR 10000000
#define MAX_LEVEL 10
#define NUM_TASKS_MULT 2

double search_max(double *A, int p, int k)
{
  double a_max = A[p];
  for(int i = p+1; i <= k; i++){
    if(a_max < A[i]) a_max = A[i];
  }
  return a_max;
}

double search_max_openmp_simple(double *A, int p, int k)
{
  double a_max = A[p];
  double a_max_local = a_max;

#pragma omp parallel default(none) firstprivate(A, p, k, a_max_local) shared(a_max)
  {
#pragma omp for
    for(int i = p+1; i <= k; i++){
      if(a_max_local < A[i]) a_max_local = A[i];
    }
    
#pragma omp critical (cs_a_max)
    {
      if(a_max < a_max_local) a_max = a_max_local;
    }
  }
  
  return a_max;
}

double search_max_openmp_task(double *A, int p, int k)
{
  double a_max = A[p];

#pragma omp parallel default(none) shared(a_max) firstprivate(A, p, k)
  {
#pragma omp single
    {
      int num_threads = omp_get_num_threads();
      float n = k - p + 1;
      
      int num_tasks = NUM_TASKS_MULT * num_threads;
      int n_loc = ceil(n / num_tasks);
      
      for(int itask = 0; itask < num_tasks; itask++){
        int p_task = p + itask * n_loc;
        if(p_task > k) break;
        int k_task = p + (itask + 1) * n_loc - 1;
        if(k_task > k) k_task = k;
        
#pragma omp task default(none) firstprivate(A, p_task, k_task) shared(a_max)
        {
          double a_max_local = search_max(A, p_task, k_task);
          
#pragma omp critical (cs_a_max)
          {
            if(a_max < a_max_local) a_max = a_max_local;
          }
        }
      }
    }
  }
  
  return a_max;
}

double bin_search_max(double *A, int p, int k)
{
  if(k - p < 1000) return search_max(A, p, k);
  
  if(p < k){
    int s = (p + k) / 2;
    
    double a_max_1 = bin_search_max(A, p, s);
    double a_max_2 = bin_search_max(A, s+1, k);
    
    return (a_max_1 > a_max_2) ? a_max_1 : a_max_2;
  }
  else{
    return A[p];
  }
}

double bin_search_max_task(double *A, int p, int k, int level)
{
  if(p < k){
    int s = (p + k) / 2;
    double a_max_1, a_max_2;
    
#pragma omp task default(none) firstprivate(A, p, s, level) shared(a_max_1)
    {
      a_max_1 = bin_search_max_task(A, p, s, level + 1);
    }
    
#pragma omp task default(none) firstprivate(A, s, k, level) shared(a_max_2)
    {
      a_max_2 = bin_search_max_task(A, s+1, k, level + 1);
    }
    
#pragma omp taskwait
    
    return (a_max_1 > a_max_2) ? a_max_1 : a_max_2;
  }
  else{
    return A[p];
  }
}

double bin_search_max_openmp(double *A, int p, int k)
{
  double a_max;
  
#pragma omp parallel default(none) firstprivate(A, p, k) shared(a_max)
  {
#pragma omp single
    {
#pragma omp task
      {
        a_max = bin_search_max_task(A, p, k, 0);
      }
    }
  }
  
  return a_max;
}

double bin_search_max_task_hybrid(double *A, int p, int k, int level)
{
  if(level > MAX_LEVEL) return search_max(A, p, k);
  
  if(p < k){
    int s = (p + k) / 2;
    double a_max_1, a_max_2;
    
#pragma omp task default(none) firstprivate(A, p, s, level) shared(a_max_1)
    {
      a_max_1 = bin_search_max_task_hybrid(A, p, s, level + 1);
    }
    
#pragma omp task default(none) firstprivate(A, s, k, level) shared(a_max_2)
    {
      a_max_2 = bin_search_max_task_hybrid(A, s+1, k, level + 1);
    }
    
#pragma omp taskwait
    
    return (a_max_1 > a_max_2) ? a_max_1 : a_max_2;
  }
  else{
    return A[p];
  }
}

double bin_search_max_openmp_hybrid(double *A, int p, int k)
{
  double a_max;
  
#pragma omp parallel default(none) firstprivate(A, p, k) shared(a_max)
  {
#pragma omp single
    {
#pragma omp task
      {
        a_max = bin_search_max_task_hybrid(A, p, k, 0);
      }
    }
  }
  
  return a_max;
}

void generuj_dane(double *A, int n)
{
  for(int i = 0; i < n; i++){
    A[i] = (0.00000001 * i) * (n - 1 - i);
  }
}

int main(int argc, char** argv)
{
  int n = ROZMIAR;
  int num_threads = 4;
  
  if(argc > 1) n = atoi(argv[1]);
  if(argc > 2) num_threads = atoi(argv[2]);
  
  omp_set_num_threads(num_threads);
  
  printf("Rozmiar: %d, Watki: %d, Max level: %d\n\n", n, num_threads, MAX_LEVEL);
  
  double *a = (double*)malloc(n * sizeof(double));
  if(a == NULL){
    printf("Blad alokacji pamieci!\n");
    return 1;
  }
  
  generuj_dane(a, n);
  
  double a_max, t1;
  
  t1 = omp_get_wtime();
  a_max = search_max(a, 0, n-1);
  t1 = omp_get_wtime() - t1;
  printf("1. search_max: %.6f s, max: %lf\n", t1, a_max);
  double t_seq_linear = t1;
  
  t1 = omp_get_wtime();
  a_max = search_max_openmp_simple(a, 0, n-1);
  t1 = omp_get_wtime() - t1;
  printf("2. search_max_openmp_simple: %.6f s (%.2fx)\n", t1, t_seq_linear / t1);
  
  t1 = omp_get_wtime();
  a_max = search_max_openmp_task(a, 0, n-1);
  t1 = omp_get_wtime() - t1;
  printf("3. search_max_openmp_task: %.6f s (%.2fx)\n\n", t1, t_seq_linear / t1);
  
  // Sortowanie tablicy przed wyszukiwaniem binarnym
  printf("Sortowanie tablicy...\n");
  t1 = omp_get_wtime();
  sortowanie_szybkie(a, 0, n-1);
  t1 = omp_get_wtime() - t1;
  printf("Czas sortowania: %.6f s\n\n", t1);
  
  t1 = omp_get_wtime();
  a_max = bin_search_max(a, 0, n-1);
  t1 = omp_get_wtime() - t1;
  printf("4. bin_search_max: %.6f s\n", t1);
  double t_seq_binary = t1;
  
  t1 = omp_get_wtime();
  a_max = bin_search_max_openmp(a, 0, n-1);
  t1 = omp_get_wtime() - t1;
  printf("5. bin_search_max_openmp: %.6f s (%.2fx)\n", t1, t_seq_binary / t1);
  
  t1 = omp_get_wtime();
  a_max = bin_search_max_openmp_hybrid(a, 0, n-1);
  t1 = omp_get_wtime() - t1;
  printf("6. bin_search_max_openmp_hybrid: %.6f s (%.2fx)\n", t1, t_seq_binary / t1);
  
  free(a);
  
  return 0;
}
