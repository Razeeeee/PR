/************************************************************************
Zadanie 5.0: Modyfikacja programu z wyszukiwaniem binarnym 
             o mechanizm szybkiego sortowania (quicksort)

UWAGA: Wyszukiwanie binarne działa tylko dla posortowanych tablic!

Program implementuje:
1. Quicksort - sekwencyjny i równoległy
2. Wyszukiwanie binarne rekurencyjne (wymaga posortowanych danych)
3. Wyszukiwanie liniowe (działa dla dowolnych danych)
4. Wyszukiwanie binarne równoległe z użyciem task
5. Wyszukiwanie hybrydowe: binarne dla dużych fragmentów + liniowe dla małych
6. Analiza wydajności wszystkich metod
*************************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<omp.h>

#define ROZMIAR 100000000
#define MAX_POZIOM 10
#define THRESHOLD_LINEAR 10000  // Próg przejścia na wyszukiwanie liniowe
#define THRESHOLD_QUICKSORT 1000  // Próg przejścia na sortowanie sekwencyjne

/*** Funkcje sortowania - Quicksort ***/

// Funkcja pomocnicza - partition dla quicksorta
int partition(double *A, int p, int k)
{
  double pivot = A[k];
  int i = p - 1;
  
  for(int j = p; j < k; j++){
    if(A[j] <= pivot){
      i++;
      // Zamiana A[i] <-> A[j]
      double temp = A[i];
      A[i] = A[j];
      A[j] = temp;
    }
  }
  
  // Zamiana A[i+1] <-> A[k]
  double temp = A[i+1];
  A[i+1] = A[k];
  A[k] = temp;
  
  return i + 1;
}

// Quicksort - sekwencyjny
void quicksort_seq(double *A, int p, int k)
{
  if(p < k){
    int q = partition(A, p, k);
    quicksort_seq(A, p, q - 1);
    quicksort_seq(A, q + 1, k);
  }
}

// Quicksort - równoległy z użyciem task
void quicksort_parallel_task(double *A, int p, int k, int poziom)
{
  if(p < k){
    int size = k - p + 1;
    
    // Dla małych fragmentów użyj sortowania sekwencyjnego
    if(size < THRESHOLD_QUICKSORT || poziom > MAX_POZIOM){
      quicksort_seq(A, p, k);
      return;
    }
    
    int q = partition(A, p, k);
    poziom++;
    
#pragma omp task default(none) firstprivate(A, p, q, poziom)
    {
      quicksort_parallel_task(A, p, q - 1, poziom);
    }
    
#pragma omp task default(none) firstprivate(A, q, k, poziom)
    {
      quicksort_parallel_task(A, q + 1, k, poziom);
    }
    
#pragma omp taskwait
  }
}

/*** Funkcje wyszukiwania sekwencyjnego ***/

// Wyszukiwanie liniowe - sekwencyjne
double search_max_linear(double *A, int p, int k)
{
  double a_max = A[p];
  for(int i = p+1; i <= k; i++){
    if(a_max < A[i]) a_max = A[i];
  }
  return a_max;
}

// Wyszukiwanie binarne - sekwencyjne (dziel i zwyciężaj)
double search_max_binary(double *A, int p, int k)
{
  if(p < k){
    int s = (p + k) / 2;
    double a_max_1 = search_max_binary(A, p, s);
    double a_max_2 = search_max_binary(A, s+1, k);
    return (a_max_1 > a_max_2) ? a_max_1 : a_max_2;
  }
  else{
    return A[p];
  }
}

/*** Funkcje wyszukiwania równoległego ***/

// Wyszukiwanie liniowe - równoległe
double search_max_linear_parallel(double *A, int p, int k)
{
  double a_max = A[p];
  double a_max_local = a_max;

#pragma omp parallel default(none) firstprivate(A, p, k, a_max_local) shared(a_max)
  {
#pragma omp for
    for(int i = p+1; i <= k; i++){
      if(a_max_local < A[i]) a_max_local = A[i];
    }
    
#pragma omp critical
    {
      if(a_max < a_max_local) a_max = a_max_local;
    }
  }
  
  return a_max;
}

// Wyszukiwanie binarne - równoległe z użyciem task
double search_max_binary_task(double *A, int p, int k, int poziom)
{
  if(p < k){
    poziom++;
    int s = (p + k) / 2;
    
    double a_max_1, a_max_2;
    
#pragma omp task default(none) firstprivate(A, p, s, poziom) shared(a_max_1) final(poziom > MAX_POZIOM)
    {
      a_max_1 = search_max_binary_task(A, p, s, poziom);
    }
    
#pragma omp task default(none) firstprivate(A, s, k, poziom) shared(a_max_2) final(poziom > MAX_POZIOM)
    {
      a_max_2 = search_max_binary_task(A, s+1, k, poziom);
    }
    
#pragma omp taskwait
    
    return (a_max_1 > a_max_2) ? a_max_1 : a_max_2;
  }
  else{
    return A[p];
  }
}

// Wyszukiwanie hybrydowe: binarne dla dużych fragmentów + liniowe dla małych
double search_max_hybrid(double *A, int p, int k, int poziom)
{
  int size = k - p + 1;
  
  // Dla małych fragmentów użyj wyszukiwania liniowego
  if(size <= THRESHOLD_LINEAR || poziom > MAX_POZIOM){
    return search_max_linear(A, p, k);
  }
  
  // Dla dużych fragmentów użyj wyszukiwania binarnego (dziel i zwyciężaj)
  poziom++;
  int s = (p + k) / 2;
  
  double a_max_1, a_max_2;
  
#pragma omp task default(none) firstprivate(A, p, s, poziom) shared(a_max_1)
  {
    a_max_1 = search_max_hybrid(A, p, s, poziom);
  }
  
#pragma omp task default(none) firstprivate(A, s, k, poziom) shared(a_max_2)
  {
    a_max_2 = search_max_hybrid(A, s+1, k, poziom);
  }
  
#pragma omp taskwait
  
  return (a_max_1 > a_max_2) ? a_max_1 : a_max_2;
}

// Wyszukiwanie z task decomposition na zadaną liczbę zadań
double search_max_task_decomp(double *A, int p, int k, int num_tasks)
{
  double a_max = A[p];
  int n = k - p + 1;
  int n_loc = (n + num_tasks - 1) / num_tasks;  // Ceiling division
  
#pragma omp parallel default(none) firstprivate(A, p, k, num_tasks, n_loc) shared(a_max)
  {
#pragma omp single
    {
      for(int itask = 0; itask < num_tasks; itask++){
        int p_task = p + itask * n_loc;
        if(p_task > k) break;
        
        int k_task = p + (itask + 1) * n_loc - 1;
        if(k_task > k) k_task = k;
        
#pragma omp task default(none) firstprivate(A, p_task, k_task) shared(a_max)
        {
          double a_max_local = search_max_linear(A, p_task, k_task);
          
#pragma omp critical
          {
            if(a_max < a_max_local) a_max = a_max_local;
          }
        }
      }
    }
  }
  
  return a_max;
}

/*** Funkcje pomocnicze ***/

void generuj_dane(double *A, int n)
{
  // Generuj dane losowe (nieposortowane)
  for(int i = 0; i < n; i++){
    A[i] = (double)rand() / RAND_MAX * 1000000.0;
  }
}

void kopiuj_tablice(double *src, double *dst, int n)
{
  for(int i = 0; i < n; i++){
    dst[i] = src[i];
  }
}

int sprawdz_sortowanie(double *A, int n)
{
  for(int i = 0; i < n-1; i++){
    if(A[i] > A[i+1]) return 0;
  }
  return 1;
}

/*** Program główny ***/

int main(int argc, char** argv)
{
  int n = ROZMIAR;
  int num_threads = 4;
  
  if(argc > 1) n = atoi(argv[1]);
  if(argc > 2) num_threads = atoi(argv[2]);
  
  omp_set_num_threads(num_threads);
  srand(12345);  // Stały seed dla powtarzalności
  
  printf("=================================================================\n");
  printf("Program z wyszukiwaniem binarnym i sortowaniem szybkim (quicksort)\n");
  printf("=================================================================\n");
  printf("Rozmiar tablicy: %d\n", n);
  printf("Liczba watkow: %d\n", num_threads);
  printf("Prog sortowania rownoleglego: %d\n", THRESHOLD_QUICKSORT);
  printf("Prog wyszukiwania liniowego: %d\n\n", THRESHOLD_LINEAR);
  
  double *A = (double*)malloc(n * sizeof(double));
  double *A_copy = (double*)malloc(n * sizeof(double));
  if(A == NULL || A_copy == NULL){
    printf("Blad alokacji pamieci!\n");
    return 1;
  }
  
  generuj_dane(A, n);
  printf("Dane wygenerowane (nieposortowane).\n\n");
  
  double a_max, t1, t2;
  double t_sort_seq, t_sort_par, t_linear_seq, t_binary_seq, t_binary_par, t_hybrid;
  
  // ===== CZĘŚĆ 1: WYSZUKIWANIE LINIOWE (DANE NIEPOSORTOWANE) =====
  printf("=== ETAP 1: WYSZUKIWANIE LINIOWE (DANE NIEPOSORTOWANE) ===\n\n");
  
  // TEST 1: Wyszukiwanie liniowe sekwencyjne
  printf("1. Wyszukiwanie liniowe sekwencyjne:\n");
  t1 = omp_get_wtime();
  a_max = search_max_linear(A, 0, n-1);
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s, maksimum: %.6f\n", t2, a_max);
  t_linear_seq = t2;
  double max_value = a_max;  // Zapamiętaj dla weryfikacji
  
  // ===== CZĘŚĆ 2: SORTOWANIE =====
  printf("\n=== ETAP 2: SORTOWANIE TABLICY ===\n\n");
  
  // TEST 2: Quicksort sekwencyjny
  kopiuj_tablice(A, A_copy, n);
  printf("2. Quicksort sekwencyjny:\n");
  t1 = omp_get_wtime();
  quicksort_seq(A_copy, 0, n-1);
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s\n", t2);
  printf("   Sprawdzenie poprawnosci: %s\n", 
         sprawdz_sortowanie(A_copy, n) ? "OK" : "BLAD!");
  t_sort_seq = t2;
  
  // TEST 3: Quicksort równoległy
  kopiuj_tablice(A, A_copy, n);
  printf("\n3. Quicksort rownolegle (OpenMP task):\n");
  t1 = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single
    {
      quicksort_parallel_task(A_copy, 0, n-1, 0);
    }
  }
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s\n", t2);
  printf("   Przyspieszenie: %.2fx\n", t_sort_seq / t2);
  printf("   Sprawdzenie poprawnosci: %s\n", 
         sprawdz_sortowanie(A_copy, n) ? "OK" : "BLAD!");
  t_sort_par = t2;
  
  // Użyj posortowanej tablicy dla wyszukiwania binarnego
  kopiuj_tablice(A_copy, A, n);
  
  // ===== CZĘŚĆ 3: WYSZUKIWANIE BINARNE (DANE POSORTOWANE) =====
  printf("\n=== ETAP 3: WYSZUKIWANIE BINARNE (DANE POSORTOWANE) ===\n\n");
  
  // TEST 4: Wyszukiwanie binarne sekwencyjne (tylko sam search, bez sortowania)
  printf("4. Wyszukiwanie binarne sekwencyjne (sam search):\n");
  t1 = omp_get_wtime();
  a_max = search_max_binary(A, 0, n-1);
  t2 = omp_get_wtime() - t1;
  printf("   Czas search: %.6f s, maksimum: %.6f\n", t2, a_max);
  t_binary_seq = t2;
  
  // TEST 5: Wyszukiwanie binarne równoległe (tylko sam search)
  printf("\n5. Wyszukiwanie binarne rownolegle (sam search):\n");
  t1 = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single
    {
      a_max = search_max_binary_task(A, 0, n-1, 0);
    }
  }
  t2 = omp_get_wtime() - t1;
  printf("   Czas search: %.6f s, przyspieszenie: %.2fx, maksimum: %.6f\n", 
         t2, t_binary_seq / t2, a_max);
  t_binary_par = t2;
  
  // TEST 6: Wyszukiwanie hybrydowe
  printf("\n6. Wyszukiwanie hybrydowe (binarny + liniowy):\n");
  t1 = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single
    {
      a_max = search_max_hybrid(A, 0, n-1, 0);
    }
  }
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s, maksimum: %.6f\n", t2, a_max);
  t_hybrid = t2;
  
  // ===== PODSUMOWANIE - RZECZYWISTY KOSZT METOD =====
  printf("\n=== PODSUMOWANIE - CALKOWITY CZAS (SORTOWANIE + WYSZUKIWANIE) ===\n\n");
  
  printf("METODA LINIOWA (nie wymaga sortowania):\n");
  printf("  Liniowe sekwencyjne: %.6f s\n", t_linear_seq);
  
  printf("\nMETODA BINARNA (wymaga sortowania):\n");
  printf("  Sort seq + Binary seq:  %.6f s (sort: %.6f + search: %.6f)\n", 
         t_sort_seq + t_binary_seq, t_sort_seq, t_binary_seq);
  printf("  Sort par + Binary seq:  %.6f s (sort: %.6f + search: %.6f)\n", 
         t_sort_par + t_binary_seq, t_sort_par, t_binary_seq);
  printf("  Sort par + Binary par:  %.6f s (sort: %.6f + search: %.6f)\n", 
         t_sort_par + t_binary_par, t_sort_par, t_binary_par);
  printf("  Sort par + Hybrid:      %.6f s (sort: %.6f + search: %.6f)\n", 
         t_sort_par + t_hybrid, t_sort_par, t_hybrid);
  
  printf("\nNAJSZYBSZA METODA:\n");
  double najszybszy_czas = t_linear_seq;
  char* najszybsza_metoda = "Liniowe sekwencyjne";
  
  if(t_sort_par + t_binary_par < najszybszy_czas){
    najszybszy_czas = t_sort_par + t_binary_par;
    najszybsza_metoda = "Sort rownolegle + Binary rownolegle";
  }
  
  printf("  %s: %.6f s\n", najszybsza_metoda, najszybszy_czas);
  printf("  Znalezione maksimum: %.6f\n", max_value);
  
  free(A);
  free(A_copy);
  
  return 0;
}
