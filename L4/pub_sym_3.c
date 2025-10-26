#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

#define ILE_MUSZE_WYPIC 3

struct struktura_t { 
  int l_wkf;
  pthread_mutex_t *tab_kuf; 
  int l_kr;
  pthread_mutex_t *tab_kran;  
};
struct struktura_t pub_wsk;

// Zmienna wspólna S3 - liczba wypitych kufli (z trylock i liczeniem pracy)
int S3 = 0;
pthread_mutex_t mutex_S3 = PTHREAD_MUTEX_INITIALIZER;

// Zmienne do zliczania pracy podczas oczekiwania
long int calkowita_praca_oczekiwania = 0;
pthread_mutex_t mutex_praca = PTHREAD_MUTEX_INITIALIZER;

// Zmienne do pomiaru czasu
struct timeval start_time, end_time;
struct rusage start_usage, end_usage;

void * watek_klient (void * arg);

int main( void ){
  pthread_t *tab_klient;
  int *tab_klient_id;

  int l_kl, l_kf, l_kr, i;

  // Inicjalizacja generatora liczb losowych
  srand(time(NULL));

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);
  pub_wsk.l_wkf = l_kf;

  printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  pub_wsk.l_kr = l_kr;

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;

  pub_wsk.tab_kuf = (pthread_mutex_t *) malloc(l_kf*sizeof(pthread_mutex_t));
  for(i=0;i<l_kf;i++) pthread_mutex_init(&pub_wsk.tab_kuf[i], NULL);

  pub_wsk.tab_kran = (pthread_mutex_t *) malloc(l_kr*sizeof(pthread_mutex_t));
  for(i=0;i<l_kr;i++) pthread_mutex_init(&pub_wsk.tab_kran[i], NULL);

  printf("\nOtwieramy pub (z trylock i liczeniem pracy)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 
  printf("Liczba kranów %d\n", l_kr);
  printf("Śledzenie liczby kufli: WŁĄCZONE (trylock z liczeniem pracy oczekiwania - S3)\n");
  
  // Pomiar czasu rozpoczęcia
  printf("\nRozpoczynanie pomiaru czasu...\n");
  gettimeofday(&start_time, NULL);
  getrusage(RUSAGE_SELF, &start_usage);

  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  
  // Pomiar czasu zakończenia
  gettimeofday(&end_time, NULL);
  getrusage(RUSAGE_SELF, &end_usage);
  
  // Obliczenie czasów
  double czas_zegarowy = (end_time.tv_sec - start_time.tv_sec) + 
                        (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
  
  double czas_cpu_user = (end_usage.ru_utime.tv_sec - start_usage.ru_utime.tv_sec) + 
                         (end_usage.ru_utime.tv_usec - start_usage.ru_utime.tv_usec) / 1000000.0;
  
  double czas_cpu_system = (end_usage.ru_stime.tv_sec - start_usage.ru_stime.tv_sec) + 
                           (end_usage.ru_stime.tv_usec - start_usage.ru_stime.tv_usec) / 1000000.0;
  
  double czas_cpu_calkowity = czas_cpu_user + czas_cpu_system;
  
  printf("\n=== POMIARY CZASU ===\n");
  printf("Czas zegarowy: %.6f sekund\n", czas_zegarowy);
  printf("Czas CPU: %.6f sekund\n", czas_cpu_calkowity);
  
  printf("\n=== WYNIKI SYMULACJI (trylock z liczeniem pracy) ===\n");
  printf("Łączna liczba wypitych kufli (S3): %d\n", S3);
  printf("Oczekiwana liczba kufli: %d\n", l_kl * ILE_MUSZE_WYPIC);
  printf("Różnica (powinno być 0 z trylock): %d\n", (l_kl * ILE_MUSZE_WYPIC) - S3);
  printf("Całkowita praca wykonana podczas oczekiwania: %ld jednostek\n", calkowita_praca_oczekiwania);
  printf("Średnia praca oczekiwania na klienta: %.2f jednostek\n", 
         (double)calkowita_praca_oczekiwania / l_kl);
  printf("\nZamykamy pub!\n");

  // Zniszczenie mutexów
  for(i=0;i<l_kf;i++) pthread_mutex_destroy(&pub_wsk.tab_kuf[i]);
  for(i=0;i<l_kr;i++) pthread_mutex_destroy(&pub_wsk.tab_kran[i]);
  pthread_mutex_destroy(&mutex_S3);
  pthread_mutex_destroy(&mutex_praca);
  
  free(pub_wsk.tab_kuf);
  free(pub_wsk.tab_kran);
  free(tab_klient);
  free(tab_klient_id);

  return 0;
}

void * watek_klient (void * arg_wsk){

  int moj_id = * ((int *)arg_wsk);
  int i_wypite, kran, kufel;
  int ile_musze_wypic = ILE_MUSZE_WYPIC;
  long int moja_praca_oczekiwania = 0;
  int licznik_prob = 0;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i_wypite=0; i_wypite<ile_musze_wypic; i_wypite++){

    printf("\nKlient %d, szukam wolnego kufla\n", moj_id); 

    // TRYLOCK z liczeniem pracy podczas oczekiwania na kufel
    kufel = -1;
    licznik_prob = 0;
    
    do {
      // Przeszukujemy wszystkie kufle w poszukiwaniu wolnego
      for(int j = 0; j < pub_wsk.l_wkf; j++) {
        if(pthread_mutex_trylock(&pub_wsk.tab_kuf[j]) == 0) {
          kufel = j;
          printf("\nKlient %d, znalazłem wolny kufel %d (po %d próbach)\n", 
                 moj_id, kufel, licznik_prob);
          break; // Znaleźliśmy wolny kufel, wychodzimy z pętli
        }
      }
      
      if(kufel == -1) {
        // Nie znaleziono wolnego kufla - wykonujemy pracę podczas oczekiwania
        licznik_prob++;

        moja_praca_oczekiwania ++;
        
        if(licznik_prob % 5 == 0) { // Co 5 prób pokazuj postęp
          printf("\nKlient %d, brak wolnych kufli (próba %d), wykonuję pracę (+1 jednostek)\n", 
                 moj_id, licznik_prob);
        }
        
        // Krótka pauza przed kolejną próbą
        usleep(1000 + rand() % 5000); // 1-6ms pauzy
      }
      
    } while(kufel == -1);

    printf("\nKlient %d, wybrałem kufel %d\n", moj_id, kufel);

    printf("\nKlient %d, szukam wolnego kranu\n", moj_id); 

    // TRYLOCK z liczeniem pracy podczas oczekiwania na kran
    kran = -1;
    licznik_prob = 0;
    
    do {
      // Przeszukujemy wszystkie krany w poszukiwaniu wolnego
      for(int j = 0; j < pub_wsk.l_kr; j++) {
        if(pthread_mutex_trylock(&pub_wsk.tab_kran[j]) == 0) {
          kran = j;
          printf("\nKlient %d, znalazłem wolny kran %d (po %d próbach)\n", 
                 moj_id, kran, licznik_prob);
          break; // Znaleźliśmy wolny kran, wychodzimy z pętli
        }
      }
      
      if(kran == -1) {
        // Nie znaleziono wolnego kranu - wykonujemy pracę podczas oczekiwania
        licznik_prob++;
        
        // Symulacja pracy podczas oczekiwania na kran
        moja_praca_oczekiwania ++;
        
        if(licznik_prob % 3 == 0) { // Co 3 próby pokazuj postęp
          printf("\nKlient %d, brak wolnych kranów (próba %d), wykonuję pracę (+1 jednostek)\n", 
                 moj_id, licznik_prob);
        }
        
        // Krótka pauza przed kolejną próbą
        usleep(500 + rand() % 2000); // 0.5-2.5ms pauzy
      }
      
    } while(kran == -1);
    
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, kran); 
    usleep(100000); // 100ms na nalewanie

    // Zwalniamy kran po nalewaniu
    pthread_mutex_unlock(&pub_wsk.tab_kran[kran]);
    printf("\nKlient %d, zwolniłem kran %d\n", moj_id, kran);

    // Picie piwa
    if(kran==0)  printf("\nKlient %d, pije piwo Guinness\n", moj_id); 
    else if(kran==1)  printf("\nKlient %d, pije piwo Żywiec\n", moj_id); 
    else if(kran==2)  printf("\nKlient %d, pije piwo Heineken\n", moj_id); 
    else if(kran==3)  printf("\nKlient %d, pije piwo Okocim\n", moj_id); 
    else if(kran==4)  printf("\nKlient %d, pije piwo Karlsberg\n", moj_id); 
    else printf("\nKlient %d, pije piwo z kranu %d\n", moj_id, kran);

    usleep(200000); // 200ms na picie

    // ZABEZPIECZONY dostęp do zmiennej wspólnej S3
    pthread_mutex_lock(&mutex_S3);
    S3++;
    printf("\nKlient %d, wypił kufel (łącznie wypito: S3=%d)\n", moj_id, S3);
    pthread_mutex_unlock(&mutex_S3);

    // Zwracamy kufel
    pthread_mutex_unlock(&pub_wsk.tab_kuf[kufel]);
    printf("\nKlient %d, oddałem kufel %d\n", moj_id, kufel); 

  }

  // Dodajemy naszą pracę oczekiwania do globalnego licznika
  pthread_mutex_lock(&mutex_praca);
  calkowita_praca_oczekiwania += moja_praca_oczekiwania;
  pthread_mutex_unlock(&mutex_praca);

  printf("\nKlient %d, wychodzę z pubu; wykonana praca oczekiwania: %ld jednostek\n",
         moj_id, moja_praca_oczekiwania);
  return(NULL);
} 
