/************************************************************************
Zadanie dodatkowe: Mnożenie macierz-wektor z wykorzystaniem OpenMP

Program implementuje różne wersje mnożenia macierz-wektor:
1. Wersja sekwencyjna - porządek wierszowy (row-major)
2. Równoległa dekompozycja wierszowa (row decomposition)
3. Równoległa dekompozycja kolumnowa (column decomposition)
4. Wersja sekwencyjna - porządek kolumnowy (column-major)
5. Równoległa dekompozycja kolumnowa dla col-major
6. Równoległa dekompozycja wierszowa dla col-major
7. Analiza wydajności i cache locality
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 8000
#define ROZMIAR (WYMIAR * WYMIAR)

/*** Funkcje dla porządku wierszowego (row-major) ***/

// Wersja sekwencyjna - row-major
void mat_vec_row_seq(double* a, double* x, double* y, int n)
{
  for(int i = 0; i < n; i++){
    y[i] = 0.0;
    for(int j = 0; j < n; j++){
      y[i] += a[n*i + j] * x[j];
    }
  }
}

// Równoległa dekompozycja wierszowa - row-major
void mat_vec_row_parallel(double* a, double* x, double* y, int n)
{
#pragma omp parallel for default(none) shared(a, x, y, n) schedule(static)
  for(int i = 0; i < n; i++){
    y[i] = 0.0;
    for(int j = 0; j < n; j++){
      y[i] += a[n*i + j] * x[j];
    }
  }
}

// Równoległa dekompozycja wierszowa z różnymi strategiami schedulingu
void mat_vec_row_parallel_dynamic(double* a, double* x, double* y, int n)
{
#pragma omp parallel for default(none) shared(a, x, y, n) schedule(dynamic)
  for(int i = 0; i < n; i++){
    y[i] = 0.0;
    for(int j = 0; j < n; j++){
      y[i] += a[n*i + j] * x[j];
    }
  }
}

// Równoległa dekompozycja kolumnowa - row-major (wymaga synchronizacji!)
void mat_vec_row_col_decomp(double* a, double* x, double* y, int n)
{
  // Inicjalizacja
  for(int i = 0; i < n; i++) y[i] = 0.0;
  
#pragma omp parallel for default(none) shared(a, x, y, n) schedule(static)
  for(int j = 0; j < n; j++){
    for(int i = 0; i < n; i++){
      #pragma omp atomic
      y[i] += a[n*i + j] * x[j];
    }
  }
}

/*** Funkcje dla porządku kolumnowego (column-major) ***/

// Wersja sekwencyjna - column-major
void mat_vec_col_seq(double* a, double* x, double* y, int n)
{
  for(int i = 0; i < n; i++) y[i] = 0.0;
  
  for(int j = 0; j < n; j++){
    for(int i = 0; i < n; i++){
      y[i] += a[i + j*n] * x[j];
    }
  }
}

// Równoległa dekompozycja kolumnowa - column-major
void mat_vec_col_parallel(double* a, double* x, double* y, int n)
{
  for(int i = 0; i < n; i++) y[i] = 0.0;
  
#pragma omp parallel for default(none) shared(a, x, y, n) schedule(static)
  for(int j = 0; j < n; j++){
    for(int i = 0; i < n; i++){
      #pragma omp atomic
      y[i] += a[i + j*n] * x[j];
    }
  }
}

// Równoległa dekompozycja wierszowa - column-major
void mat_vec_col_row_decomp(double* a, double* x, double* y, int n)
{
#pragma omp parallel for default(none) shared(a, x, y, n) schedule(static)
  for(int i = 0; i < n; i++){
    y[i] = 0.0;
    for(int j = 0; j < n; j++){
      y[i] += a[i + j*n] * x[j];
    }
  }
}

// Równoległa z redukcją - column-major
void mat_vec_col_reduction(double* a, double* x, double* y, int n)
{
#pragma omp parallel for default(none) shared(a, x, y, n) schedule(static)
  for(int i = 0; i < n; i++){
    double sum = 0.0;
    for(int j = 0; j < n; j++){
      sum += a[i + j*n] * x[j];
    }
    y[i] = sum;
  }
}

/*** Funkcje pomocnicze ***/

void sprawdz_wynik(double* y1, double* y2, int n, const char* nazwa)
{
  for(int i = 0; i < n; i++){
    double diff = fabs(y1[i] - y2[i]);
    if(diff > 1.e-6 * fabs(y2[i])){
      printf("   BLAD!\n");
      return;
    }
  }
}

/*** Program główny ***/

int main(int argc, char** argv)
{
  int n = WYMIAR;
  int num_threads = 4;
  
  if(argc > 1) n = atoi(argv[1]);
  if(argc > 2) num_threads = atoi(argv[2]);
  
  omp_set_num_threads(num_threads);
  
  printf("Wymiar macierzy: %d x %d\n", n, n);
  printf("Liczba watkow: %d\n\n", num_threads);
  
  // Alokacja pamięci
  double *a_row = (double*)malloc(n * n * sizeof(double));
  double *a_col = (double*)malloc(n * n * sizeof(double));
  double *x = (double*)malloc(n * sizeof(double));
  double *y = (double*)malloc(n * sizeof(double));
  double *y_ref = (double*)malloc(n * sizeof(double));
  
  if(!a_row || !a_col || !x || !y || !y_ref){
    printf("Blad alokacji pamieci!\n");
    return 1;
  }
  
  // Inicjalizacja danych
  for(int i = 0; i < n*n; i++) a_row[i] = 1.0001 * i;
  for(int i = 0; i < n; i++) x[i] = 1.0 * (n - i);
  
  // Przekształcenie do formatu column-major
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      a_col[i + j*n] = a_row[n*i + j];
    }
  }
  
  double t1, t2;
  double nr_oper = (double)n * n * 2.0;
  
  printf("=== ROW-MAJOR ===\n\n");
  
  t1 = omp_get_wtime();
  mat_vec_row_seq(a_row, x, y_ref, n);
  t2 = omp_get_wtime() - t1;
  printf("1. mat_vec_row_seq: %.6f s (%.2f GFLOPS)\n", t2, nr_oper / t2 / 1.0e9);
  double t_row_seq = t2;
  
  t1 = omp_get_wtime();
  mat_vec_row_parallel(a_row, x, y, n);
  t2 = omp_get_wtime() - t1;
  printf("2. mat_vec_row_parallel: %.6f s (%.2fx, %.2f GFLOPS)\n",
         t2, t_row_seq / t2, nr_oper / t2 / 1.0e9);
  sprawdz_wynik(y, y_ref, n, "row-parallel");
  
  t1 = omp_get_wtime();
  mat_vec_row_col_decomp(a_row, x, y, n);
  t2 = omp_get_wtime() - t1;
  printf("3. mat_vec_row_col_decomp: %.6f s (%.2fx, %.2f GFLOPS)\n",
         t2, t_row_seq / t2, nr_oper / t2 / 1.0e9);
  sprawdz_wynik(y, y_ref, n, "row-col-decomp");
  
  printf("\n=== COLUMN-MAJOR ===\n\n");
  t1 = omp_get_wtime();
  mat_vec_col_seq(a_col, x, y_ref, n);
  t2 = omp_get_wtime() - t1;
  printf("4. mat_vec_col_seq: %.6f s (%.2f GFLOPS)\n", t2, nr_oper / t2 / 1.0e9);
  double t_col_seq = t2;
  
  // TEST 5: Column-major równoległy - dekompozycja kolumnowa
  printf("5. Column-major rownolegle (dekompozycja kolumnowa z atomic):\n");
  t1 = omp_get_wtime();
  mat_vec_col_parallel(a_col, x, y, n);
  t2 = omp_get_wtime() - t1;
  printf("   Czas: %.6f s, przyspieszenie: %.2fx\n", t2, t_col_seq / t2);
  sprawdz_wynik(y, y_ref, n, "col-parallel");
  
  t1 = omp_get_wtime();
  mat_vec_col_row_decomp(a_col, x, y, n);
  t2 = omp_get_wtime() - t1;
  printf("6. mat_vec_col_row_decomp: %.6f s (%.2fx, %.2f GFLOPS)\n",
         t2, t_col_seq / t2, nr_oper / t2 / 1.0e9);
  sprawdz_wynik(y, y_ref, n, "col-row-decomp");
  
  free(a_row);
  free(a_col);
  free(x);
  free(y);
  free(y_ref);
  
  return 0;
}