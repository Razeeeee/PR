#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define ILE_MUSZE_WYPIC 3

struct struktura_t { 
  int l_wkf;
  pthread_mutex_t *tab_kuf; 
  int l_kr;
  pthread_mutex_t *tab_kran;  
};
struct struktura_t pub_wsk;

// Zmienna wspólna S4 - liczba wypitych kufli (z dodatkowym śledzeniem)
int S4 = 0;
pthread_mutex_t mutex_S4 = PTHREAD_MUTEX_INITIALIZER;

// Dodatkowe śledzenie dla S4
int *kufle_wykorzystane;      // Licznik wykorzystania każdego kufla
int *krany_wykorzystane;      // Licznik wykorzystania każdego kranu
pthread_mutex_t mutex_statystyki = PTHREAD_MUTEX_INITIALIZER;

// Statystyki czasów oczekiwania
long int calkowity_czas_oczekiwania_kufle = 0;
long int calkowity_czas_oczekiwania_krany = 0;
int liczba_prob_kufle = 0;
int liczba_prob_krany = 0;
pthread_mutex_t mutex_czasy = PTHREAD_MUTEX_INITIALIZER;



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

  // Inicjalizacja tablic statystyk
  kufle_wykorzystane = (int *) calloc(l_kf, sizeof(int));
  krany_wykorzystane = (int *) calloc(l_kr, sizeof(int));

  printf("\nOtwieramy pub (z rozszerzonym śledzeniem S4)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 
  printf("Liczba kranów %d\n", l_kr);
  printf("Śledzenie liczby kufli: WŁĄCZONE (S4 z dodatkowymi statystykami)\n");
  printf("Dodatkowe funkcje: śledzenie wykorzystania kufli/kranów, czasy oczekiwania\n");

  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  
  printf("\n=== WYNIKI SYMULACJI (S4 z rozszerzonym śledzeniem) ===\n");
  printf("Łączna liczba wypitych kufli (S4): %d\n", S4);
  printf("Oczekiwana liczba kufli: %d\n", l_kl * ILE_MUSZE_WYPIC);
  printf("Różnica: %d\n", (l_kl * ILE_MUSZE_WYPIC) - S4);
  
  if(liczba_prob_kufle > 0) {
    printf("Średni czas oczekiwania na kufel: %.2f μs\n", 
           (double)calkowity_czas_oczekiwania_kufle / liczba_prob_kufle);
  }
  if(liczba_prob_krany > 0) {
    printf("Średni czas oczekiwania na kran: %.2f μs\n", 
           (double)calkowity_czas_oczekiwania_krany / liczba_prob_krany);
  }

  // Statystyki wykorzystania kufli
  printf("\n=== STATYSTYKI WYKORZYSTANIA KUFLI ===\n");
  int max_kufle = 0, min_kufle = kufle_wykorzystane[0];
  double suma_kufle = 0;
  
  for(i=0; i<l_kf; i++) {
    printf("Kufel %d: wykorzystany %d razy\n", i, kufle_wykorzystane[i]);
    suma_kufle += kufle_wykorzystane[i];
    if(kufle_wykorzystane[i] > max_kufle) max_kufle = kufle_wykorzystane[i];
    if(kufle_wykorzystane[i] < min_kufle) min_kufle = kufle_wykorzystane[i];
  }
  
  printf("Średnie wykorzystanie kufla: %.2f użyć\n", suma_kufle / l_kf);
  printf("Najbardziej wykorzystany kufel: %d użyć\n", max_kufle);
  printf("Najmniej wykorzystany kufel: %d użyć\n", min_kufle);
  printf("Różnica w wykorzystaniu: %d użyć\n", max_kufle - min_kufle);

  // Statystyki wykorzystania kranów
  printf("\n=== STATYSTYKI WYKORZYSTANIA KRANÓW ===\n");
  int max_krany = 0, min_krany = krany_wykorzystane[0];
  double suma_krany = 0;
  
  for(i=0; i<l_kr; i++) {
    printf("Kran %d: wykorzystany %d razy", i, krany_wykorzystane[i]);
    if(i==0) printf(" (Guinness)");
    else if(i==1) printf(" (Żywiec)");
    else if(i==2) printf(" (Heineken)");
    else if(i==3) printf(" (Okocim)");
    else if(i==4) printf(" (Karlsberg)");
    printf("\n");
    
    suma_krany += krany_wykorzystane[i];
    if(krany_wykorzystane[i] > max_krany) max_krany = krany_wykorzystane[i];
    if(krany_wykorzystane[i] < min_krany) min_krany = krany_wykorzystane[i];
  }
  
  printf("Średnie wykorzystanie kranu: %.2f użyć\n", suma_krany / l_kr);
  printf("Najbardziej wykorzystany kran: %d użyć\n", max_krany);
  printf("Najmniej wykorzystany kran: %d użyć\n", min_krany);
  printf("Różnica w wykorzystaniu: %d użyć\n", max_krany - min_krany);

  printf("\nZamykamy pub!\n");

  // Zniszczenie mutexów
  for(i=0;i<l_kf;i++) pthread_mutex_destroy(&pub_wsk.tab_kuf[i]);
  for(i=0;i<l_kr;i++) pthread_mutex_destroy(&pub_wsk.tab_kran[i]);
  pthread_mutex_destroy(&mutex_S4);
  pthread_mutex_destroy(&mutex_statystyki);
  pthread_mutex_destroy(&mutex_czasy);
  
  free(pub_wsk.tab_kuf);
  free(pub_wsk.tab_kran);
  free(tab_klient);
  free(tab_klient_id);
  free(kufle_wykorzystane);
  free(krany_wykorzystane);

  return 0;
}

void * watek_klient (void * arg_wsk){

  int moj_id = * ((int *)arg_wsk);
  int i_wypite, kran, kufel;
  int ile_musze_wypic = ILE_MUSZE_WYPIC;
  struct timeval czas_start, czas_koniec;
  long int moj_czas_oczekiwania = 0;
  long int czas_oczekiwania_kufel = 0;
  long int czas_oczekiwania_kran = 0;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i_wypite=0; i_wypite<ile_musze_wypic; i_wypite++){

    printf("\nKlient %d, szukam wolnego kufla\n", moj_id); 

    kufel = -1;
    int licznik_prob_kufel = 0;
    
    do {
      for(int j = 0; j < pub_wsk.l_wkf; j++) {
        if(pthread_mutex_trylock(&pub_wsk.tab_kuf[j]) == 0) {
          kufel = j;
          break;
        }
      }
      
      if(kufel == -1) {
        licznik_prob_kufel++;
      }
      
    } while(kufel == -1);

    printf("\nKlient %d, znalazłem wolny kufel %d (po %d próbach)\n", 
           moj_id, kufel, licznik_prob_kufel);

    pthread_mutex_lock(&mutex_statystyki);
    kufle_wykorzystane[kufel]++;
    pthread_mutex_unlock(&mutex_statystyki);

    printf("\nKlient %d, wybrałem kufel %d\n", moj_id, kufel);

    printf("\nKlient %d, szukam wolnego kranu\n", moj_id); 

    kran = -1;
    int licznik_prob_kran = 0;
    
    do {
      for(int j = 0; j < pub_wsk.l_kr; j++) {
        if(pthread_mutex_trylock(&pub_wsk.tab_kran[j]) == 0) {
          kran = j;
          break;
        }
      }
      
      if(kran == -1) {
        licznik_prob_kran++;
      }
      
    } while(kran == -1);

    printf("\nKlient %d, znalazłem wolny kran %d (po %d próbach)\n", 
           moj_id, kran, licznik_prob_kran);

    // Aktualizacja statystyk kranu
    pthread_mutex_lock(&mutex_statystyki);
    krany_wykorzystane[kran]++;
    pthread_mutex_unlock(&mutex_statystyki);
    
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, kran); 

    pthread_mutex_unlock(&pub_wsk.tab_kran[kran]);
    printf("\nKlient %d, zwolniłem kran %d\n", moj_id, kran);

    if(kran==0)  printf("\nKlient %d, pije piwo Guinness\n", moj_id); 
    else if(kran==1)  printf("\nKlient %d, pije piwo Żywiec\n", moj_id); 
    else if(kran==2)  printf("\nKlient %d, pije piwo Heineken\n", moj_id); 
    else if(kran==3)  printf("\nKlient %d, pije piwo Okocim\n", moj_id); 
    else if(kran==4)  printf("\nKlient %d, pije piwo Karlsberg\n", moj_id); 
    else printf("\nKlient %d, pije piwo z kranu %d\n", moj_id, kran);

    pthread_mutex_lock(&mutex_S4);
    S4++;
    printf("\nKlient %d, wypił kufel (łącznie wypito: S4=%d)\n", moj_id, S4);
    pthread_mutex_unlock(&mutex_S4);

    pthread_mutex_unlock(&pub_wsk.tab_kuf[kufel]);
    printf("\nKlient %d, oddałem kufel %d\n", moj_id, kufel); 

  }

  printf("\nKlient %d, wychodzę z pubu\n",
         moj_id);
  return(NULL);
} 
