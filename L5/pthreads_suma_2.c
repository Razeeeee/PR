#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<math.h>

#include"pomiar_czasu.h"

pthread_mutex_t muteks; 
pthread_t *watki;
double *global_array_of_local_sums;

void *suma_w_no_mutex( void *arg_wsk);

double *tab;
double suma=0; 

typedef struct {
    int rozmiar;
    int liczba_watkow;
    int moj_id;
} thread_data_t;

int main( int argc, char *argv[] ){

  int i, j, k, w;
  double t_seq, t_par;
  int rozmiary[] = {1000, 1000000, 100000000};
  int liczby_watkow[] = {1, 2, 4};
  int n_rozmiarow = 3;
  int n_watkow = 3;
  int powtorzenia = 3;

  // Dla każdej liczby wątków (1, 2, 4)
  for(w = 0; w < n_watkow; w++) {
    int LICZBA_W = liczby_watkow[w];
    
    // Wypisz nagłówek dla danej liczby wątków
    printf("Liczba watkow: %d\n", LICZBA_W);
    
    // Dla każdego rozmiaru tablicy
    for(k = 0; k < n_rozmiarow; k++) {
      int ROZMIAR = rozmiary[k];
      
      tab = (double *) malloc(ROZMIAR*sizeof(double));
      for(i=0; i<ROZMIAR; i++) tab[i] = ((double) i+1) / ROZMIAR;

      // 3 pomiary
      for(j = 0; j < powtorzenia; j++) {
        
        // Wersja sekwencyjna
        suma = 0;
        t_seq = czas_zegara();
        for(i = 0; i < ROZMIAR; i++){ 
          suma += tab[i]; 
        }
        t_seq = czas_zegara() - t_seq;

        // Wersja równoległa
        watki = (pthread_t *) malloc(LICZBA_W * sizeof(pthread_t));
        global_array_of_local_sums = (double *) malloc(LICZBA_W * sizeof(double));
        thread_data_t *thread_data = (thread_data_t *) malloc(LICZBA_W * sizeof(thread_data_t));

        suma = 0;
        t_par = czas_zegara();

        for(i=0; i<LICZBA_W; i++) {
          global_array_of_local_sums[i] = 0.0;
          thread_data[i].rozmiar = ROZMIAR;
          thread_data[i].liczba_watkow = LICZBA_W;
          thread_data[i].moj_id = i;
          pthread_create(&watki[i], NULL, suma_w_no_mutex, (void *) &thread_data[i]);
        }

        for(i=0; i<LICZBA_W; i++) {
          pthread_join(watki[i], NULL);
          suma += global_array_of_local_sums[i];
        }

        t_par = czas_zegara() - t_par;

        // Wypisz w formacie CSV: sekwencyjna,równoległa
        printf("%lf,%lf\n", t_seq, t_par);

        free(watki);
        free(global_array_of_local_sums);
        free(thread_data);
      }

      free(tab);
    }
  }

  return 0;
}

void *suma_w_no_mutex( void *arg_wsk){

  int i, j;
  thread_data_t *data = (thread_data_t *) arg_wsk;
  int moj_id = data->moj_id;
  int ROZMIAR = data->rozmiar;
  int LICZBA_W = data->liczba_watkow;

  double tmp = 0.0;
  j = ceil((float)ROZMIAR/LICZBA_W);
  
  int start = j * moj_id;
  int end = (start + j < ROZMIAR) ? start + j : ROZMIAR;
  
  for(i = start; i < end; i++){ 
    tmp += tab[i];
  }

  global_array_of_local_sums[moj_id] = tmp;

  pthread_exit((void *)0);
}
