################################################################################
# Makefile dla programów OpenMP - Laboratorium 10
# 
# Użycie:
#   make 1    - kompiluje i uruchamia zadanie 1 (analiza zależności)
#   make 2    - kompiluje i uruchamia zadanie 2 (threadprivate)
#   make 3    - kompiluje i uruchamia zadanie 3 (sortowanie z task)
#   make 4    - kompiluje i uruchamia zadanie 4 (wyszukiwanie hybrydowe)
#   make 5    - kompiluje i uruchamia zadanie 5 (mnożenie macierz-wektor)
#   make all  - kompiluje wszystkie programy
#   make clean - usuwa pliki wykonywalne i obiekty
################################################################################

CC = gcc
CFLAGS = -fopenmp -O3 -Wall -g
LDFLAGS = -fopenmp -lm

# Katalog na pliki binarne
BINDIR = bin

# Pliki wykonywalne
EXECUTABLES = $(BINDIR)/zadanie1 $(BINDIR)/zadanie2 $(BINDIR)/zadanie3 $(BINDIR)/zadanie4 $(BINDIR)/zadanie5

# Pliki obiektowe dla bibliotek
SORT_OBJS = $(BINDIR)/sortowanie_seq.o

################################################################################
# Reguła domyślna
################################################################################

.PHONY: all
all: $(BINDIR) $(EXECUTABLES)
	@echo "================================"
	@echo "Wszystkie programy skompilowane!"
	@echo "================================"
	@echo "Uruchom: make 1, make 2, make 3, make 4, lub make 5"

# Tworzenie katalogu bin
$(BINDIR):
	mkdir -p $(BINDIR)

# Zadanie 1: Analiza i eliminacja zależności
$(BINDIR)/zadanie1: $(BINDIR)/zadanie1.o | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/zadanie1.o: zadanie1.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: 1
1: clean $(BINDIR)/zadanie1
	@echo "================================"
	@echo "Uruchamianie zadania 1..."
	@echo "================================"
	$(BINDIR)/zadanie1

################################################################################

# Zadanie 2: Wykorzystanie threadprivate
$(BINDIR)/zadanie2: $(BINDIR)/zadanie2.o | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/zadanie2.o: zadanie2.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: 2
2: clean $(BINDIR)/zadanie2
	@echo "================================"
	@echo "Uruchamianie zadania 2..."
	@echo "================================"
	$(BINDIR)/zadanie2

################################################################################

# Zadanie 3: Sortowanie z użyciem task
$(BINDIR)/zadanie3: $(BINDIR)/zadanie3.o $(SORT_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/zadanie3.o: zadanie3.c sortowanie_seq.h | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/sortowanie_seq.o: sortowanie_seq.c sortowanie_seq.h | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: 3
3: clean $(BINDIR)/zadanie3
	@echo "================================"
	@echo "Uruchamianie zadania 3..."
	@echo "================================"
	@echo "Podaj rozmiar tablicy (np. 1000000) i liczbę wątków (np. 4):"
	@echo "Domyślnie: 1000000 elementów, 4 wątki"
	$(BINDIR)/zadanie3 10000000 4

# Dodatkowe cele dla zadania 3 z różnymi parametrami
.PHONY: 3-small
3-small: clean $(BINDIR)/zadanie3
	@echo "Test z małą tablicą (100000 elementów)"
	$(BINDIR)/zadanie3 100000 4

.PHONY: 3-large
3-large: clean $(BINDIR)/zadanie3
	@echo "Test z dużą tablicą (5000000 elementów)"
	$(BINDIR)/zadanie3 5000000 4

.PHONY: 3-threads
3-threads: clean $(BINDIR)/zadanie3
	@echo "Test różnej liczby wątków (1, 2, 4, 8)"
	@echo "\n=== 1 wątek ==="
	$(BINDIR)/zadanie3 1000000 1
	@echo "\n=== 2 wątki ==="
	$(BINDIR)/zadanie3 1000000 2
	@echo "\n=== 4 wątki ==="
	$(BINDIR)/zadanie3 1000000 4
	@echo "\n=== 8 wątków ==="
	$(BINDIR)/zadanie3 1000000 8

################################################################################

# Zadanie 4: Wyszukiwanie hybrydowe
$(BINDIR)/zadanie4: $(BINDIR)/zadanie4.o $(SORT_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/zadanie4.o: zadanie4.c sortowanie_seq.h | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: 4
4: clean $(BINDIR)/zadanie4
	@echo "================================"
	@echo "Uruchamianie zadania 4..."
	@echo "================================"
	@echo "UWAGA: Duża tablica - może trwać dłużej"
	@echo "Domyślnie: 10000000 elementów, 4 wątki"
	$(BINDIR)/zadanie4 10000000 4

# Dodatkowe cele dla zadania 4
.PHONY: 4-small
4-small: clean $(BINDIR)/zadanie4
	@echo "Test z mniejszą tablicą (10000000 elementów)"
	$(BINDIR)/zadanie4 10000000 4

.PHONY: 4-medium
4-medium: clean $(BINDIR)/zadanie4
	@echo "Test ze średnią tablicą (50000000 elementów)"
	$(BINDIR)/zadanie4 50000000 4

################################################################################

# Zadanie 5: Mnożenie macierz-wektor
$(BINDIR)/zadanie5: $(BINDIR)/zadanie5.o | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/zadanie5.o: zadanie5.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: 5
5: clean $(BINDIR)/zadanie5
	@echo "================================"
	@echo "Uruchamianie zadania 5..."
	@echo "================================"
	@echo "UWAGA: Duża macierz - może trwać dłużej i zajmować dużo pamięci"
	@echo "Domyślnie: macierz 50000x50000, 4 wątki"
	$(BINDIR)/zadanie5 10000 4

# Dodatkowe cele dla zadania 5
.PHONY: 5-small
5-small: clean $(BINDIR)/zadanie5
	@echo "Test z małą macierzą (1000x1000)"
	$(BINDIR)/zadanie5 1000 4

.PHONY: 5-medium
5-medium: clean $(BINDIR)/zadanie5
	@echo "Test ze średnią macierzą (4000x4000)"
	$(BINDIR)/zadanie5 4000 4

.PHONY: 5-threads
5-threads: clean $(BINDIR)/zadanie5
	@echo "Test różnej liczby wątków dla macierzy 4000x4000"
	@echo "\n=== 1 wątek ==="
	$(BINDIR)/zadanie5 4000 1
	@echo "\n=== 2 wątki ==="
	$(BINDIR)/zadanie5 4000 2
	@echo "\n=== 4 wątki ==="
	$(BINDIR)/zadanie5 4000 4
	@echo "\n=== 8 wątków ==="
	$(BINDIR)/zadanie5 4000 8

################################################################################
# Cele pomocnicze
################################################################################

.PHONY: clean
clean:
	@echo "Czyszczenie plików..."
	rm -rf $(BINDIR)
	@echo "Pliki wyczyszczone."

.PHONY: help
help:
	@echo "================================"
	@echo "Dostępne polecenia:"
	@echo "================================"
	@echo "make 1        - Zadanie 1: Analiza zależności w OpenMP"
	@echo "make 2        - Zadanie 2: Wykorzystanie threadprivate"
	@echo "make 3        - Zadanie 3: Sortowanie z task (1M elementów)"
	@echo "make 3-small  - Zadanie 3: Sortowanie (100K elementów)"
	@echo "make 3-large  - Zadanie 3: Sortowanie (5M elementów)"
	@echo "make 3-threads- Zadanie 3: Test z różną liczbą wątków"
	@echo "make 4        - Zadanie 4: Wyszukiwanie hybrydowe (100M elementów)"
	@echo "make 4-small  - Zadanie 4: Wyszukiwanie (10M elementów)"
	@echo "make 4-medium - Zadanie 4: Wyszukiwanie (50M elementów)"
	@echo "make 5        - Zadanie 5: Mnożenie macierz-wektor (8000x8000)"
	@echo "make 5-small  - Zadanie 5: Mnożenie macierz-wektor (1000x1000)"
	@echo "make 5-medium - Zadanie 5: Mnożenie macierz-wektor (4000x4000)"
	@echo "make 5-threads- Zadanie 5: Test z różną liczbą wątków"
	@echo "make all      - Kompilacja wszystkich programów"
	@echo "make clean    - Usunięcie plików wykonywalnych"
	@echo "make help     - Wyświetlenie tej pomocy"
	@echo "================================"

.PHONY: info
info:
	@echo "================================"
	@echo "Informacje o programach:"
	@echo "================================"
	@echo "Zadanie 1 (zadanie1):"
	@echo "  - Analiza i eliminacja zależności w OpenMP"
	@echo "  - Identyfikacja RAW, WAR, WAW"
	@echo ""
	@echo "Zadanie 2 (zadanie2):"
	@echo "  - Wykorzystanie threadprivate"
	@echo "  - Przenoszenie informacji między blokami równoległymi"
	@echo ""
	@echo "Zadanie 3 (zadanie3):"
	@echo "  - Sortowanie z użyciem task"
	@echo "  - Porównanie wydajności różnych technik"
	@echo "  - Parametry: rozmiar_tablicy liczba_wątków"
	@echo ""
	@echo "Zadanie 4 (zadanie4):"
	@echo "  - Wyszukiwanie binarne z optymalizacją liniową"
	@echo "  - Wyszukiwanie hybrydowe"
	@echo "  - Parametry: rozmiar_tablicy liczba_wątków"
	@echo ""
	@echo "Zadanie 5 (zadanie5):"
	@echo "  - Mnożenie macierz-wektor"
	@echo "  - Analiza cache locality"
	@echo "  - Parametry: wymiar_macierzy liczba_wątków"
	@echo "================================"

# Cel testowy - uruchamia wszystkie programy z małymi danymi
.PHONY: test
test: clean all
	@echo "\n================================"
	@echo "TEST 1: Analiza zależności"
	@echo "================================"
	$(BINDIR)/zadanie1
	@echo "\n================================"
	@echo "TEST 2: Threadprivate"
	@echo "================================"
	$(BINDIR)/zadanie2
	@echo "\n================================"
	@echo "TEST 3: Sortowanie (100K elementów)"
	@echo "================================"
	$(BINDIR)/zadanie3 100000 4
	@echo "\n================================"
	@echo "TEST 4: Wyszukiwanie (10M elementów)"
	@echo "================================"
	$(BINDIR)/zadanie4 10000000 4
	@echo "\n================================"
	@echo "TEST 5: Mnożenie macierz-wektor (1000x1000)"
	@echo "================================"
	$(BINDIR)/zadanie5 1000 4
	@echo "\n================================"
	@echo "Wszystkie testy zakończone!"
	@echo "================================"

################################################################################
# Reguły zależności
################################################################################

.PHONY: rebuild
rebuild: clean all
