# kompilator c
CCOMP = gcc

# konsolidator
LOADER = gcc

# opcje optymalizacji:
# wersja do debugowania
OPT = -g -DDEBUG -no-pie
# wersja zoptymalizowana do mierzenia czasu
# OPT = -O3 -no-pie

# pliki naglowkowe
INC = -I.

# biblioteki
LIB = -L. -lpomiar_czasu -lm -lpthread

# nazwy programów
PROGRAM1 = pthreads_suma
PROGRAM2 = pthreads_suma_2
PROGRAM3 = test_dokladnosc
PROGRAM4 = dekompozycja_petli
PROGRAM5 = dekompozycja_petli_blokowa
PROGRAM6 = dekompozycja_obszaru
PROGRAM7 = dekompozycja_obszaru_pthread_exit

# target 'dev' - clean, kompilacja i uruchomienie z argumentem (domyślnie program 1)
dev: dev1

# target 'dev1' - clean, kompilacja i uruchomienie programu 1
dev1: clean $(PROGRAM1)
	@echo "=========================================="
	@echo "Uruchamianie programu $(PROGRAM1)..."
	@echo "=========================================="
	./$(PROGRAM1) $(ARGS)

# target 'dev2' - clean, kompilacja i uruchomienie programu 2 (pomiary CSV)
dev2: clean $(PROGRAM2)
	@echo "=========================================="
	@echo "Uruchamianie programu $(PROGRAM2)..."
	@echo "=========================================="
	./$(PROGRAM2) $(ARGS)

# target 'dev3' - clean, kompilacja i uruchomienie programu 3 (test dokładności całki)
dev3: clean $(PROGRAM3)
	@echo "=========================================="
	@echo "Uruchamianie programu $(PROGRAM3)..."
	@echo "=========================================="
	./$(PROGRAM3) $(ARGS)

# target 'dev4' - clean, kompilacja i uruchomienie programu 4 (dekompozycja pętli - cykliczna)
dev4: clean $(PROGRAM4)
	@echo "=========================================="
	@echo "Uruchamianie programu $(PROGRAM4)..."
	@echo "=========================================="
	./$(PROGRAM4) $(ARGS)

# target 'dev5' - clean, kompilacja i uruchomienie programu 5 (dekompozycja pętli - blokowa)
dev5: clean $(PROGRAM5)
	@echo "=========================================="
	@echo "Uruchamianie programu $(PROGRAM5)..."
	@echo "=========================================="
	./$(PROGRAM5) $(ARGS)

# target 'dev6' - clean, kompilacja i uruchomienie programu 6 (dekompozycja obszaru)
dev6: clean $(PROGRAM6)
	@echo "=========================================="
	@echo "Uruchamianie programu $(PROGRAM6)..."
	@echo "=========================================="
	./$(PROGRAM6) $(ARGS)

# target 'dev7' - clean, kompilacja i uruchomienie programu 7 (dekompozycja obszaru pthread_exit)
dev7: clean $(PROGRAM7)
	@echo "=========================================="
	@echo "Uruchamianie programu $(PROGRAM7)..."
	@echo "=========================================="
	./$(PROGRAM7) $(ARGS)

# target 'all' - kompilacja wszystkich programów
all: $(PROGRAM1) $(PROGRAM2) $(PROGRAM3) $(PROGRAM4) $(PROGRAM5) $(PROGRAM6) $(PROGRAM7)

# zaleznosci i komendy dla pthreads_suma
$(PROGRAM1): $(PROGRAM1).o
	$(LOADER) $(OPT) $(PROGRAM1).o -o $(PROGRAM1) $(LIB)

# jak uzyskac plik pthreads_suma.o ?
$(PROGRAM1).o: pomiar_czasu.h $(PROGRAM1).c
	$(CCOMP) -c $(OPT) $(PROGRAM1).c $(INC)

# zaleznosci i komendy dla pthreads_suma_2
$(PROGRAM2): $(PROGRAM2).o
	$(LOADER) $(OPT) $(PROGRAM2).o -o $(PROGRAM2) $(LIB)

# jak uzyskac plik pthreads_suma_2.o ?
$(PROGRAM2).o: pomiar_czasu.h $(PROGRAM2).c
	$(CCOMP) -c $(OPT) $(PROGRAM2).c $(INC)

# zaleznosci i komendy dla test_dokladnosc
$(PROGRAM3): $(PROGRAM3).o
	$(LOADER) $(OPT) $(PROGRAM3).o -o $(PROGRAM3) $(LIB)

# jak uzyskac plik test_dokladnosc.o ?
$(PROGRAM3).o: pomiar_czasu.h $(PROGRAM3).c
	$(CCOMP) -c $(OPT) $(PROGRAM3).c $(INC)

# zaleznosci i komendy dla dekompozycja_petli
$(PROGRAM4): $(PROGRAM4).o
	$(LOADER) $(OPT) $(PROGRAM4).o -o $(PROGRAM4) $(LIB)

# jak uzyskac plik dekompozycja_petli.o ?
$(PROGRAM4).o: pomiar_czasu.h $(PROGRAM4).c
	$(CCOMP) -c $(OPT) $(PROGRAM4).c $(INC)

# zaleznosci i komendy dla dekompozycja_petli_blokowa
$(PROGRAM5): $(PROGRAM5).o
	$(LOADER) $(OPT) $(PROGRAM5).o -o $(PROGRAM5) $(LIB)

# jak uzyskac plik dekompozycja_petli_blokowa.o ?
$(PROGRAM5).o: pomiar_czasu.h $(PROGRAM5).c
	$(CCOMP) -c $(OPT) $(PROGRAM5).c $(INC)

# zaleznosci i komendy dla dekompozycja_obszaru
$(PROGRAM6): $(PROGRAM6).o
	$(LOADER) $(OPT) $(PROGRAM6).o -o $(PROGRAM6) $(LIB)

# jak uzyskac plik dekompozycja_obszaru.o ?
$(PROGRAM6).o: pomiar_czasu.h $(PROGRAM6).c
	$(CCOMP) -c $(OPT) $(PROGRAM6).c $(INC)

# zaleznosci i komendy dla dekompozycja_obszaru_pthread_exit
$(PROGRAM7): $(PROGRAM7).o
	$(LOADER) $(OPT) $(PROGRAM7).o -o $(PROGRAM7) $(LIB)

# jak uzyskac plik dekompozycja_obszaru_pthread_exit.o ?
$(PROGRAM7).o: pomiar_czasu.h $(PROGRAM7).c
	$(CCOMP) -c $(OPT) $(PROGRAM7).c $(INC)

# czyszczenie
clean:
	rm -f *.o $(PROGRAM1) $(PROGRAM2) $(PROGRAM3) $(PROGRAM4) $(PROGRAM5) $(PROGRAM6) $(PROGRAM7)

# pomoc - wyświetla dostępne targety
help:
	@echo "=========================================="
	@echo "Dostępne targety Makefile:"
	@echo "=========================================="
	@echo "  make dev1   - kompiluje i uruchamia $(PROGRAM1)"
	@echo "  make dev2   - kompiluje i uruchamia $(PROGRAM2) (pomiary CSV)"
	@echo "  make dev3   - kompiluje i uruchamia $(PROGRAM3) (test dokładności całki, CSV)"
	@echo "  make dev4   - kompiluje i uruchamia $(PROGRAM4) (dekompozycja pętli - cykliczna)"
	@echo "  make dev5   - kompiluje i uruchamia $(PROGRAM5) (dekompozycja pętli - blokowa)"
	@echo "  make dev6   - kompiluje i uruchamia $(PROGRAM6) (dekompozycja obszaru)"
	@echo "  make dev7   - kompiluje i uruchamia $(PROGRAM7) (dekompozycja obszaru pthread_exit)"
	@echo "  make all    - kompiluje wszystkie programy"
	@echo "  make clean  - usuwa pliki obiektowe i wykonywalne"
	@echo "  make help   - wyświetla tę pomoc"
	@echo "=========================================="

.PHONY: clean run dev dev1 dev2 dev3 dev4 dev5 dev6 dev7 all help
