# Makefile dla L9 - Programowanie Równoległe - OpenMP
# Kompilator i flagi
CC = gcc
CFLAGS = -fopenmp -Wall -O2
LDFLAGS = -fopenmp -lm

# Lista programów
PROGRAMS = zadanie1_sposoby_watkow \
           zadanie2_scheduling \
           zadanie3_reduction_2D \
           zadanie4_kolumnowa_reczna \
           zadanie5_dekompozycja_2D \
           zadanie6_wierszowa_tablica_1D

# Cel domyślny
all: $(PROGRAMS)

# Reguły kompilacji dla każdego programu
zadanie1_sposoby_watkow: zadanie1_sposoby_watkow.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

zadanie2_scheduling: zadanie2_scheduling.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

zadanie3_reduction_2D: zadanie3_reduction_2D.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

zadanie4_kolumnowa_reczna: zadanie4_kolumnowa_reczna.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

zadanie5_dekompozycja_2D: zadanie5_dekompozycja_2D.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

zadanie6_wierszowa_tablica_1D: zadanie6_wierszowa_tablica_1D.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Cele dla make 1, make 2, itd. - czyszczenie, kompilacja i uruchomienie
1: clean zadanie1_sposoby_watkow
	@echo ""
	@echo "=========================================="
	@echo "Uruchamianie Zadania 1: Sposoby ustawiania liczby watkow"
	@echo "=========================================="
	@echo ""
	./zadanie1_sposoby_watkow

2: clean zadanie2_scheduling
	@echo ""
	@echo "=========================================="
	@echo "Uruchamianie Zadania 2: Scheduling - podzial iteracji"
	@echo "=========================================="
	@echo ""
	./zadanie2_scheduling

3: clean zadanie3_reduction_2D
	@echo ""
	@echo "=========================================="
	@echo "Uruchamianie Zadania 3: Reduction dla tablicy 2D"
	@echo "=========================================="
	@echo ""
	./zadanie3_reduction_2D

4: clean zadanie4_kolumnowa_reczna
	@echo ""
	@echo "=========================================="
	@echo "Uruchamianie Zadania 4: Dekompozycja kolumnowa reczna"
	@echo "=========================================="
	@echo ""
	./zadanie4_kolumnowa_reczna

5: clean zadanie5_dekompozycja_2D
	@echo ""
	@echo "=========================================="
	@echo "Uruchamianie Zadania 5: Dekompozycja 2D zagniezdzona"
	@echo "=========================================="
	@echo ""
	./zadanie5_dekompozycja_2D

6: clean zadanie6_wierszowa_tablica_1D
	@echo ""
	@echo "=========================================="
	@echo "Uruchamianie Zadania 6: Dekompozycja wierszowa z tablica 1D"
	@echo "=========================================="
	@echo ""
	./zadanie6_wierszowa_tablica_1D

# Kompilacja wszystkich bez uruchamiania
compile: $(PROGRAMS)
	@echo "Wszystkie programy skompilowane!"

# Uruchomienie wszystkich po kolei
run_all: all
	@echo ""
	@echo "=========================================="
	@echo "ZADANIE 1: Sposoby ustawiania liczby watkow"
	@echo "=========================================="
	@echo ""
	./zadanie1_sposoby_watkow
	@echo ""
	@echo "=========================================="
	@echo "ZADANIE 2: Scheduling - podzial iteracji"
	@echo "=========================================="
	@echo ""
	./zadanie2_scheduling
	@echo ""
	@echo "=========================================="
	@echo "ZADANIE 3: Reduction dla tablicy 2D"
	@echo "=========================================="
	@echo ""
	./zadanie3_reduction_2D
	@echo ""
	@echo "=========================================="
	@echo "ZADANIE 4: Dekompozycja kolumnowa reczna"
	@echo "=========================================="
	@echo ""
	./zadanie4_kolumnowa_reczna
	@echo ""
	@echo "=========================================="
	@echo "ZADANIE 5: Dekompozycja 2D zagniezdzona"
	@echo "=========================================="
	@echo ""
	./zadanie5_dekompozycja_2D
	@echo ""
	@echo "=========================================="
	@echo "ZADANIE 6: Dekompozycja wierszowa z tablica 1D"
	@echo "=========================================="
	@echo ""
	./zadanie6_wierszowa_tablica_1D

# Czyszczenie
clean:
	rm -f $(PROGRAMS) *.o

# Test kompilacji - sprawdzenie czy wszystko się kompiluje
test: clean compile
	@echo ""
	@echo "=========================================="
	@echo "Test kompilacji zakonczony pomyslnie!"
	@echo "=========================================="
	@echo ""
	@echo "Dostepne cele:"
	@echo "  make 1  - zadanie 1 (sposoby ustawiania liczby watkow)"
	@echo "  make 2  - zadanie 2 (scheduling)"
	@echo "  make 3  - zadanie 3 (reduction 2D)"
	@echo "  make 4  - zadanie 4 (dekompozycja kolumnowa reczna)"
	@echo "  make 5  - zadanie 5 (dekompozycja 2D zagniezdzona)"
	@echo "  make 6  - zadanie 6 (dekompozycja wierszowa z tablica 1D)"
	@echo "  make run_all - uruchom wszystkie zadania"
	@echo ""

# Pomoc
help:
	@echo "Dostepne cele Makefile:"
	@echo ""
	@echo "  make all      - kompilacja wszystkich programow"
	@echo "  make compile  - to samo co 'make all'"
	@echo "  make 1        - czysci, kompiluje i uruchamia zadanie 1"
	@echo "  make 2        - czysci, kompiluje i uruchamia zadanie 2"
	@echo "  make 3        - czysci, kompiluje i uruchamia zadanie 3"
	@echo "  make 4        - czysci, kompiluje i uruchamia zadanie 4"
	@echo "  make 5        - czysci, kompiluje i uruchamia zadanie 5"
	@echo "  make 6        - czysci, kompiluje i uruchamia zadanie 6"
	@echo "  make run_all  - kompiluje i uruchamia wszystkie zadania"
	@echo "  make clean    - usuwa skompilowane pliki"
	@echo "  make test     - testuje kompilacje wszystkich programow"
	@echo "  make help     - wyswietla ta pomoc"
	@echo ""
	@echo "Przyklady uzycia:"
	@echo "  make 1           # uruchom zadanie 1"
	@echo "  make test        # sprawdz czy wszystko sie kompiluje"
	@echo "  make run_all     # uruchom wszystkie po kolei"
	@echo ""

.PHONY: all clean compile run_all test help 1 2 3 4 5 6
