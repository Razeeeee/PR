#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include <unistd.h>

int zmienna_wspolna=0;

#define WYMIAR 1000
#define ROZMIAR WYMIAR*WYMIAR
double a[ROZMIAR],b[ROZMIAR],c[ROZMIAR];

// mnożenie macierzy 1000 x 1000
double czasozajmowacz(){
  int i, j, k;
  int n=WYMIAR;
  for(i=0;i<ROZMIAR;i++) a[i]=1.0*i;
  for(i=0;i<ROZMIAR;i++) b[i]=1.0*(ROZMIAR-i);
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      c[i+n*j]=0.0;
      for(k=0;k<n;k++){
	c[i+n*j] += a[i+n*k]*b[k+n*j];
      }
    }
  }
  return(c[ROZMIAR-1]);
}

void * zadanie_watku (void * arg_wsk)
{
	// Wyłączenie możliwości anulowania wątku
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	printf("\twatek potomny: uniemozliwione zabicie\n");

	czasozajmowacz();

	printf("\twatek potomny: umozliwienie zabicia\n");
	// Włączenie możliwości anulowania wątku
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	// Punkt sprawdzania anulowania - jeśli wątek otrzymał sygnał
	// anulowania, zostanie zakończony w tym miejscu
	pthread_testcancel();

	zmienna_wspolna++;
	printf("\twatek potomny: zmiana wartosci zmiennej wspolnej\n");

	return(NULL);
}

int main()
{
	pthread_t tid;
	pthread_attr_t attr;
	void *wynik;
	int i;

	// Wątek przyłączalny (joinable)
	
	printf("watek glowny: tworzenie watku potomnego nr 1\n");

	// Tworzenie wątku z domyślnymi właściwościami
	// Argumenty:
	// &tid - wskaźnik na identyfikator tworzonego wątku
	// NULL - domyślne atrybuty wątku (wątek przyłączalny)
	// zadanie_watku - funkcja wykonywana przez wątek
	// NULL - argument przekazywany do funkcji wątku
	pthread_create(&tid, NULL, zadanie_watku, NULL);

	sleep(2); // Czas na uruchomienie wątku

	printf("\twatek glowny: wyslanie sygnalu zabicia watku\n");
	pthread_cancel(tid);

	// Przed sprawdzeniem czy wątek się skończył, należy na niego poczekać
	// Oczekiwanie na zakończenie wątku potomnego i pobranie wyniku
	pthread_join(tid, &wynik);
	
	if (wynik == PTHREAD_CANCELED)
		printf("\twatek glowny: watek potomny zostal zabity\n");
	else
		printf("\twatek glowny: watek potomny NIE zostal zabity - blad\n");

	// Odłączanie wątku w trakcie działania

	zmienna_wspolna = 0;

	printf("watek glowny: tworzenie watku potomnego nr 2\n");

	// Tworzenie wątku z domyślnymi właściwościami (początkowo przyłączalny)
	pthread_create(&tid, NULL, zadanie_watku, NULL);

	sleep(2); // Czas na uruchomienie wątku

	printf("\twatek glowny: odlaczenie watku potomnego\n");
	// Instrukcja odłączenia wątku już działającego
	// Odłączenie wątku zmienia go z przyłączalnego na odłączony
	// Zasoby wątku odłączonego są automatycznie zwalniane po zakończeniu
	// Nie można używać pthread_join() na wątku odłączonym
	pthread_detach(tid);

	printf("\twatek glowny: wyslanie sygnalu zabicia watku odlaczonego\n");
	pthread_cancel(tid);

	// Czy wątek został zabity? Jak to sprawdzić?
	// Nie można sprawdzić czy wątek odłączony został anulowany,
	// ponieważ nie można użyć pthread_join() na wątku odłączonym
	// - pthread_join() zwraca błąd dla wątków odłączonych
	
	// Wątek odłączony utworzony z atrybutami
	
	// Inicjalizacja atrybutów wątku
	pthread_attr_init(&attr);

	// Ustawienie typu wątku na odłączony
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	printf("watek glowny: tworzenie odlaczonego watku potomnego nr 3\n");
	// Argumenty:
	// &tid - wskaźnik na identyfikator tworzonego wątku
	// &attr - wskaźnik na atrybuty wątku (ustawione jako odłączony)
	// zadanie_watku - funkcja wykonywana przez wątek
	// NULL - argument przekazywany do funkcji wątku
	pthread_create(&tid, &attr, zadanie_watku, NULL);

	// Niszczenie atrybutów
	// Zwolnienie zasobów zajmowanych przez obiekt atrybutów wątku
	pthread_attr_destroy(&attr);

	printf("\twatek glowny: koniec pracy, watek odlaczony pracuje dalej\n");
	// Zakończenie wątku głównego
	pthread_exit(NULL); // Co stanie się gdy użyjemy exit(0)?
	// Użycie exit(0) zakończyłoby cały proces wraz ze wszystkimi wątkami
	// pthread_exit(NULL) pozwala innym wątkom kontynuować działanie
	// po zakończeniu wątku głównego
}
