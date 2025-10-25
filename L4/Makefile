# kompilator c
CCOMP = gcc

# konsolidator
LOADER = gcc

# opcje optymalizacji:
# wersja do debugowania
OPT = -g -DDEBUG
# wersja zoptymalizowana
#OPT = -O3

# biblioteki dla pthread
LIB = -pthread

# zaleznosci i komendy dla pub_sym_0
pub_sym_0: pub_sym_0.o 
	$(LOADER) $(OPT) pub_sym_0.o -o pub_sym_0 $(LIB)

# jak uzyskac plik pub_sym_0.o ?
pub_sym_0.o: pub_sym_0.c
	$(CCOMP) -c $(OPT) pub_sym_0.c 

# zaleznosci i komendy dla pub_sym1
pub_sym1: pub_sym1.o 
	$(LOADER) $(OPT) pub_sym1.o -o pub_sym1 $(LIB)

# jak uzyskac plik pub_sym1.o ?
pub_sym1.o: pub_sym1.c
	$(CCOMP) -c $(OPT) pub_sym1.c 

# zaleznosci i komendy dla pub_sym_1
pub_sym_1: pub_sym_1.o 
	$(LOADER) $(OPT) pub_sym_1.o -o pub_sym_1 $(LIB)

# jak uzyskac plik pub_sym_1.o ?
pub_sym_1.o: pub_sym_1.c
	$(CCOMP) -c $(OPT) pub_sym_1.c 

# zaleznosci i komendy dla pub_sym2
pub_sym2: pub_sym2.o 
	$(LOADER) $(OPT) pub_sym2.o -o pub_sym2 $(LIB)

# jak uzyskac plik pub_sym2.o ?
pub_sym2.o: pub_sym2.c
	$(CCOMP) -c $(OPT) pub_sym2.c 

# zaleznosci i komendy dla pub_sym_2
pub_sym_2: pub_sym_2.o 
	$(LOADER) $(OPT) pub_sym_2.o -o pub_sym_2 $(LIB)

# jak uzyskac plik pub_sym_2.o ?
pub_sym_2.o: pub_sym_2.c
	$(CCOMP) -c $(OPT) pub_sym_2.c 

# zaleznosci i komendy dla pub_sym_3
pub_sym_3: pub_sym_3.o 
	$(LOADER) $(OPT) pub_sym_3.o -o pub_sym_3 $(LIB)

# jak uzyskac plik pub_sym_3.o ?
pub_sym_3.o: pub_sym_3.c
	$(CCOMP) -c $(OPT) pub_sym_3.c 

# zaleznosci i komendy dla pub_sym_4
pub_sym_4: pub_sym_4.o 
	$(LOADER) $(OPT) pub_sym_4.o -o pub_sym_4 $(LIB)

# jak uzyskac plik pub_sym_4.o ?
pub_sym_4.o: pub_sym_4.c
	$(CCOMP) -c $(OPT) pub_sym_4.c 

# regula do kompilacji wszystkich programow
all: pub_sym_0 pub_sym1 pub_sym_1 pub_sym2 pub_sym_2 pub_sym_3 pub_sym_4

clean:
	rm -f *.o pub_sym_0 pub_sym1 pub_sym_1 pub_sym2 pub_sym_2 pub_sym_3 pub_sym_4

# uruchomienie testu dla pub_sym_0
test_pub_sym_0: pub_sym_0
	@echo "=== Test pub_sym_0 (bez zabezpieczenia dostępu do S0) ==="
	@echo "5\n10" | ./pub_sym_0

# uruchomienie testu dla pub_sym_1
test_pub_sym_1: pub_sym_1
	@echo "=== Test pub_sym_1 (z zabezpieczeniem mutex dla S1) ==="
	@echo "5\n20" | ./pub_sym_1

# uruchomienie testu dla pub_sym_2
test_pub_sym_2: pub_sym_2
	@echo "=== Test pub_sym_2 (aktywne czekanie na zasób - S2) ==="
	@echo "4\n2\n3" | ./pub_sym_2

# uruchomienie testu dla pub_sym_3
test_pub_sym_3: pub_sym_3
	@echo "=== Test pub_sym_3 (trylock z liczeniem pracy - S3) ==="
	@echo "4\n2\n3" | ./pub_sym_3

# uruchomienie testu dla pub_sym_4
test_pub_sym_4: pub_sym_4
	@echo "=== Test pub_sym_4 (rozszerzone śledzenie - S4) ==="
	@echo "5\n3\n4" | ./pub_sym_4

# Reguły dev - kompilacja i uruchomienie w jednym
dev0: pub_sym_0
	@echo "=== DEV: Kompilacja i uruchomienie pub_sym_0 ==="
	@echo "Podaj liczbę klientów i kufli:"
	@./pub_sym_0

dev1: pub_sym_1
	@echo "=== DEV: Kompilacja i uruchomienie pub_sym_1 ==="
	@echo "Podaj liczbę klientów i kufli:"
	@./pub_sym_1

dev2: pub_sym_2
	@echo "=== DEV: Kompilacja i uruchomienie pub_sym_2 ==="
	@echo "Podaj liczbę klientów, kufli i kranów:"
	@./pub_sym_2

dev3: pub_sym_3
	@echo "=== DEV: Kompilacja i uruchomienie pub_sym_3 ==="
	@echo "Podaj liczbę klientów, kufli i kranów:"
	@./pub_sym_3

dev4: pub_sym_4
	@echo "=== DEV: Kompilacja i uruchomienie pub_sym_4 ==="
	@echo "Podaj liczbę klientów, kufli i kranów:"
	@./pub_sym_4

# Reguły dev z przykładowymi danymi
dev0-auto: pub_sym_0
	@echo "=== DEV AUTO: pub_sym_0 (5 klientów, 10 kufli) ==="
	@echo -e "5\n10" | ./pub_sym_0

dev1-auto: pub_sym_1
	@echo "=== DEV AUTO: pub_sym_1 (5 klientów, 20 kufli) ==="
	@echo -e "5\n20" | ./pub_sym_1

dev2-auto: pub_sym_2
	@echo "=== DEV AUTO: pub_sym_2 (4 klientów, 2 kufle, 3 krany) ==="
	@echo -e "4\n2\n3" | ./pub_sym_2

dev3-auto: pub_sym_3
	@echo "=== DEV AUTO: pub_sym_3 (4 klientów, 2 kufle, 3 krany) ==="
	@echo -e "4\n2\n3" | ./pub_sym_3

dev4-auto: pub_sym_4
	@echo "=== DEV AUTO: pub_sym_4 (5 klientów, 3 kufle, 4 krany) ==="
	@echo -e "5\n3\n4" | ./pub_sym_4

# Pomoc dev
dev-help:
	@echo "=== DOSTĘPNE KOMENDY DEV ==="
	@echo "make dev0       - kompiluj i uruchom pub_sym_0 (interaktywnie)"
	@echo "make dev1       - kompiluj i uruchom pub_sym_1 (interaktywnie)"  
	@echo "make dev2       - kompiluj i uruchom pub_sym_2 (interaktywnie)"
	@echo "make dev3       - kompiluj i uruchom pub_sym_3 (interaktywnie)"
	@echo "make dev4       - kompiluj i uruchom pub_sym_4 (interaktywnie)"
	@echo "make dev0-auto  - kompiluj i uruchom pub_sym_0 (automatyczne dane)"
	@echo "make dev1-auto  - kompiluj i uruchom pub_sym_1 (automatyczne dane)"
	@echo "make dev2-auto  - kompiluj i uruchom pub_sym_2 (automatyczne dane)"
	@echo "make dev3-auto  - kompiluj i uruchom pub_sym_3 (automatyczne dane)"
	@echo "make dev4-auto  - kompiluj i uruchom pub_sym_4 (automatyczne dane)"
	@echo "make dev-help   - pokaż tę pomoc"

.PHONY: all clean test_pub_sym_0 test_pub_sym_1 test_pub_sym_2 test_pub_sym_3 test_pub_sym_4 dev0 dev1 dev2 dev3 dev4 dev0-auto dev1-auto dev2-auto dev3-auto dev4-auto dev-help
