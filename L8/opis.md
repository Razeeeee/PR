## Zadanie 1: Mechanizm bariery (zadanie1_bariera.c)

Implementacja mechanizmu bariery synchronizacyjnej z wykorzystaniem mutex i zmiennych warunkowych.

Funkcje:

- `bariera_init(bariera_t *b, int liczba_watkow)` - inicjalizacja bariery dla okreslonej liczby watkow
- `bariera(bariera_t *b)` - synchronizacja watkow na barierze

Program tworzy 5 watkow, ktore przechodza przez 3 kolejne fazy. Kazda faza konczy sie bariera - wszystkie watki musza zakonczyc dana faze, zanim ktorykolwiek z nich moze przejsc do nastepnej.

## Zadanie 2: Sledzenie czytelnikow i pisarzy (zadanie2_sledzenie.c)

Implementacja czytelni z mechanizmem walidacji i detekcji bledow.

W kazdej funkcji (`chce_pisac`, `koniec_pisania`, `chce_czytac`, `koniec_czytania`) sprawdzana jest poprawnosc stanu:

- Liczba czytelnikow nie moze byc ujemna
- Liczba pisarzy nie moze byc ujemna
- Liczba pisarzy nie moze przekroczyc 1
- Czytelnicy i pisarze nie moga byc w czytelni jednoczesnie

Przy wykryciu bledu wyswietlany jest odpowiedni komunikat.

## Zadanie 3: Implementacja czytelni - monitor (zadanie3_czytelnia.c)

Implementacja klasycznego problemu czytelnikow i pisarzy zgodnie z Wykladem 6 (slajdy 10-12).

Rozwiazanie oparte na monitorze z funkcjami:

- `chce_pisac()` - protokol wejscia dla pisarza
- `koniec_pisania()` - protokol wyjscia dla pisarza
- `chce_czytac()` - protokol wejscia dla czytelnika
- `koniec_czytania()` - protokol wyjscia dla czytelnika

Rozwiazanie jest uczciwe wzgledem obu grup. Zamiast funkcji `empty()` (niedostepnej w C) wykorzystany jest licznik `czekajacy_pisarze`.

## Zadanie 4: Czytelnia z pthread_rwlock (zadanie4_rwlock.c)

Implementacja czytelni z wykorzystaniem dedykowanych zamkow POSIX do odczytu i zapisu.

Wykorzystywane funkcje:

- `pthread_rwlock_rdlock()` - zamek do odczytu (wiele watkow moze czytac jednoczesnie)
- `pthread_rwlock_wrlock()` - zamek do zapisu (tylko jeden watek moze pisac)
- `pthread_rwlock_unlock()` - zwolnienie zamka

To rozwiazanie jest prostsze niz reczna implementacja monitora, poniewaz mechanizm czytelnikow i pisarzy jest wbudowany w biblioteke pthread.

## Zadanie 5: Czytelnia w Javie (Zadanie5Czytelnia.java)

Implementacja czytelni w jezyku Java z wykorzystaniem wbudowanych mechanizmow monitorow.

Klasa `Czytelnia` z metodami `synchronized`:

- `chce_pisac()` - protokol wejscia dla pisarza
- `koniec_pisania()` - protokol wyjscia dla pisarza
- `chce_czytac()` - protokol wejscia dla czytelnika
- `koniec_czytania()` - protokol wyjscia dla czytelnika

W Javie monitor jest realizowany przez:

- Slowo kluczowe `synchronized` (automatyczny mutex)
- Metody `wait()` i `notifyAll()` (zmienne warunkowe)

## Kompilacja i uruchamianie

Wszystkie zadania mozna skompilowac i uruchomic za pomoca Makefile:

```
make 1  # Zadanie 1 - bariera
make 2  # Zadanie 2 - sledzenie
make 3  # Zadanie 3 - czytelnia monitor
make 4  # Zadanie 4 - rwlock
make 5  # Zadanie 5 - Java
```

Lub wszystkie naraz:

```
make run-all
```
