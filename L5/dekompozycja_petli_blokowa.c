#include<stdio.h>
#include<pthread.h>
#include<math.h>
#include"pomiar_czasu.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

double funkcja ( double x );
double funkcja ( double x ){ return( sin(x) ); }

double calka_zrownoleglenie_petli_blokowa(double a, double b, double dx, int l_w);

static int l_w_global=0;

static double calka_global=0.0;
static double a_global;
static double b_global;
static double dx_global;
static int N_global;

pthread_mutex_t mutex_calka = PTHREAD_MUTEX_INITIALIZER;

void* calka_fragment_petli_w(void* arg_wsk);

double calka_zrownoleglenie_petli_blokowa(double a, double b, double dx, int l_w){

  int N = ceil((b-a)/dx);
  double dx_adjust = (b-a)/N;

  //printf("Obliczona liczba trapezów: N = %d, dx_adjust = %lf\n", N, dx_adjust);

  // ustawienie zmiennych globalnych
  a_global = a;
  b_global = b;
  dx_global = dx_adjust;
  N_global = N;
  l_w_global = l_w;
  calka_global = 0.0;

  // tworzenie struktur danych do obsługi wielowątkowości
  pthread_t watki[l_w];
  int indeksy_watkow[l_w];

  // tworzenie wątków
  int i;
  for(i=0; i<l_w; i++){
    indeksy_watkow[i] = i;
    pthread_create(&watki[i], NULL, calka_fragment_petli_w, (void*)&indeksy_watkow[i]);
  }

  // oczekiwanie na zakończenie pracy wątków
  for(i=0; i<l_w; i++){
    pthread_join(watki[i], NULL);
  }

  return(calka_global);
}

void* calka_fragment_petli_w(void* arg_wsk){

  int my_id = *((int*)arg_wsk);

  double a, b, dx;
  int N, l_w;

  a = a_global;
  b = b_global;
  dx = dx_global;
  N = N_global;
  l_w = l_w_global;

  // dekompozycja blokowa
  // Każdy wątek dostaje ciągły blok iteracji
  int block_size = N / l_w;           // podstawowy rozmiar bloku
  int remainder = N % l_w;            // reszta do rozdzielenia
  
  int my_start, my_end;
  
  // Pierwszy 'remainder' wątków dostaje o 1 iterację więcej
  if(my_id < remainder){
    my_start = my_id * (block_size + 1);
    my_end = my_start + (block_size + 1);
  } else {
    my_start = my_id * block_size + remainder;
    my_end = my_start + block_size;
  }
  
  int my_stride = 1;  // w dekompozycji blokowej wykonujemy kolejne iteracje

  // Przykład z N=10, l_w=4:
  // block_size = 10/4 = 2, remainder = 10%4 = 2
  // wątek 0: iteracje 0,1,2 (start=0, end=3) - dostaje +1
  // wątek 1: iteracje 3,4,5 (start=3, end=6) - dostaje +1
  // wątek 2: iteracje 6,7   (start=6, end=8)
  // wątek 3: iteracje 8,9   (start=8, end=10)

  //printf("\nWątek %d: my_start %d, my_end %d, my_stride %d\n", 
  //	 my_id, my_start, my_end, my_stride);

  int i;
  double calka = 0.0;
  for(i=my_start; i<my_end; i+=my_stride){

    double x1 = a + i*dx;
    calka += 0.5*dx*(funkcja(x1)+funkcja(x1+dx));

  }

  // sekcja krytyczna - dodanie lokalnej wartości do globalnej
  pthread_mutex_lock(&mutex_calka);
  calka_global += calka;
  pthread_mutex_unlock(&mutex_calka);

  pthread_exit(NULL);
}

int main( int argc, char *argv[] ){

  double t1;
  double a, b, dx, calka;

  a = 0.0;
  b = M_PI;

  // Wartość dokładna całki z sin(x) od 0 do π to 2.0
  double wartosc_dokladna = 2.0;

  // Tablice wartości do testowania
  double dx_values[] = {0.1, 0.01, 0.001, 0.0001};
  int l_w_values[] = {1, 2, 4, 8};
  int num_dx = 4;
  int num_lw = 4;

  printf("\n========================================\n");
  printf("DEKOMPOZYCJA BLOKOWA PĘTLI\n");
  printf("Format: dx, liczba_wątków, czas[s], całka, błąd\n");
  printf("========================================\n");

  int i, j;
  for(i = 0; i < num_dx; i++){
    dx = dx_values[i];
    
    for(j = 0; j < num_lw; j++){
      int l_w = l_w_values[j];
      
      printf("\n--- Test: dx=%.6f, wątki=%d ---\n", dx, l_w);
      
      t1 = czas_zegara();
      calka = calka_zrownoleglenie_petli_blokowa(a, b, dx, l_w);
      t1 = czas_zegara() - t1;
      
      double blad = fabs(calka - wartosc_dokladna);
      
      printf("Wynik: %.6f,%d,%.6f,%.15lf,%.15lf\n", 
             dx, l_w, t1, calka, blad);
    }
  }

  return 0;
}
