#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include"pomiar_czasu.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

double funkcja ( double x );
double funkcja ( double x ){ return( sin(x) ); }

// Struktura do przekazywania danych do wątków
typedef struct {
  int id_watku;         // identyfikator wątku
  double a_local;       // lewy kraniec podobszaru
  double b_local;       // prawy kraniec podobszaru
  double dx;            // wysokość trapezu
} dane_obszaru_t;

double calka_dekompozycja_obszaru_pthread_exit(double a, double b, double dx, int l_w);
void* calka_podobszar_w_pthread_exit(void* arg_wsk);

double calka_dekompozycja_obszaru_pthread_exit(double a, double b, double dx, int l_w){

  // tworzenie struktur danych do obsługi wielowątkowości
  pthread_t watki[l_w];
  dane_obszaru_t dane_watkow[l_w];
  
  // Obliczanie szerokości podobszaru dla każdego wątku
  double szerokosc_podobszaru = (b - a) / l_w;
  
  // Wypełnianie struktur z zakresami całkowania w main
  int i;
  for(i = 0; i < l_w; i++){
    dane_watkow[i].id_watku = i;
    dane_watkow[i].a_local = a + i * szerokosc_podobszaru;
    dane_watkow[i].b_local = a + (i + 1) * szerokosc_podobszaru;
    dane_watkow[i].dx = dx;
  }

  // tworzenie wątków
  for(i = 0; i < l_w; i++){
    pthread_create(&watki[i], NULL, calka_podobszar_w_pthread_exit, (void*)&dane_watkow[i]);
  }

  // oczekiwanie na zakończenie pracy wątków i odbieranie wyników
  double calka_suma_local = 0.0;
  for(i = 0; i < l_w; i++){
    double* wynik_watku = NULL;
    pthread_join(watki[i], (void**)&wynik_watku);
    
    if(wynik_watku != NULL){
      calka_suma_local += *wynik_watku;
      // Zwolnienie pamięci zaalokowanej przez wątek
      free(wynik_watku);
      wynik_watku = NULL;
    }
  }

  return(calka_suma_local);
}

void* calka_podobszar_w_pthread_exit(void* arg_wsk){

  // rozpakowanie danych przesłanych do wątku
  dane_obszaru_t* dane = (dane_obszaru_t*)arg_wsk;
  
  int my_id = dane->id_watku;
  double a_local = dane->a_local;
  double b_local = dane->b_local;
  double dx = dane->dx;
  
  //printf("\nWątek %d: a_local %lf, b_local %lf, dx %lf\n", 
  //	 my_id, a_local, b_local, dx);
  
  // Obliczanie całki w lokalnym podobszarze
  int N_local = ceil((b_local - a_local) / dx);
  double dx_adjust_local = (b_local - a_local) / N_local;
  
  //printf("Wątek %d: a_local %lf, b_local %lf, dx_adjust_local %lf, n_local %d\n", 
  //	 my_id, a_local, b_local, dx_adjust_local, N_local);
  
  int i;
  double calka = 0.0;
  for(i = 0; i < N_local; i++){
    double x1 = a_local + i * dx_adjust_local;
    calka += 0.5 * dx_adjust_local * (funkcja(x1) + funkcja(x1 + dx_adjust_local));
    //printf("Wątek %d: i %d, x1 %lf, funkcja(x1) %lf, całka = %.15lf\n", 
    //	   my_id, i, x1, funkcja(x1), calka);
  }
  
  // Dynamiczna alokacja pamięci dla wyniku
  double* wynik = (double*)malloc(sizeof(double));
  if(wynik == NULL){
    fprintf(stderr, "Błąd alokacji pamięci w wątku %d\n", my_id);
    pthread_exit(NULL);
  }
  
  *wynik = calka;
  
  // Zwrócenie wyniku przez pthread_exit
  pthread_exit((void*)wynik);
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
  printf("Dekompozycja obszaru z pthread_exit\n");
  printf("(zwracanie wyniku przez dynamiczną alokację)\n");
  printf("Format: dx, liczba_wątków, czas[s], całka, błąd\n");
  printf("========================================\n");

  int i, j;
  for(i = 0; i < num_dx; i++){
    dx = dx_values[i];
    
    for(j = 0; j < num_lw; j++){
      int l_w = l_w_values[j];
      
      printf("\n--- Test: dx=%.6f, wątki=%d ---\n", dx, l_w);
      
      t1 = czas_zegara();
      calka = calka_dekompozycja_obszaru_pthread_exit(a, b, dx, l_w);
      t1 = czas_zegara() - t1;
      
      double blad = fabs(calka - wartosc_dokladna);
      
      printf("Wynik: %.6f,%d,%.6f,%.15lf,%.15lf\n", 
             dx, l_w, t1, calka, blad);
    }
  }

  return 0;
}
