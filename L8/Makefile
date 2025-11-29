# Makefile dla L8 - Zadania dotyczące synchronizacji
# Kompilacja i uruchamianie programów

CC = gcc
CFLAGS = -Wall -pthread -finput-charset=UTF-8 -fexec-charset=UTF-8
JAVAC = javac
JAVA = java
BINDIR = bin

# Cele do kompilacji i uruchamiania poszczególnych zadań

# Zadanie 1: Mechanizm bariery
1: $(BINDIR)/zadanie1_bariera
	@echo "=== Uruchamianie Zadania 1: Mechanizm bariery ==="
	$(BINDIR)/zadanie1_bariera

$(BINDIR)/zadanie1_bariera: zadanie1_bariera.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/zadanie1_bariera zadanie1_bariera.c

# Zadanie 2: Śledzenie liczby czytelników i pisarzy
2: $(BINDIR)/zadanie2_sledzenie
	@echo "=== Uruchamianie Zadania 2: Śledzenie czytelników i pisarzy ==="
	$(BINDIR)/zadanie2_sledzenie

$(BINDIR)/zadanie2_sledzenie: zadanie2_sledzenie.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/zadanie2_sledzenie zadanie2_sledzenie.c

# Zadanie 3: Implementacja czytelni (monitor)
3: $(BINDIR)/zadanie3_czytelnia
	@echo "=== Uruchamianie Zadania 3: Czytelnia (monitor) ==="
	$(BINDIR)/zadanie3_czytelnia

$(BINDIR)/zadanie3_czytelnia: zadanie3_czytelnia.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/zadanie3_czytelnia zadanie3_czytelnia.c

# Zadanie 4: Czytelnia z pthread_rwlock
4: $(BINDIR)/zadanie4_rwlock
	@echo "=== Uruchamianie Zadania 4: Czytelnia z pthread_rwlock ==="
	$(BINDIR)/zadanie4_rwlock

$(BINDIR)/zadanie4_rwlock: zadanie4_rwlock.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/zadanie4_rwlock zadanie4_rwlock.c

# Zadanie 5: Czytelnia w Javie
5: $(BINDIR)/Zadanie5Czytelnia.class
	@echo "=== Uruchamianie Zadania 5: Czytelnia w Javie ==="
	cd $(BINDIR) && $(JAVA) Zadanie5Czytelnia

$(BINDIR)/Zadanie5Czytelnia.class: Zadanie5Czytelnia.java | $(BINDIR)
	$(JAVAC) -d $(BINDIR) Zadanie5Czytelnia.java

# Kompilacja wszystkich programów
all: $(BINDIR)/zadanie1_bariera $(BINDIR)/zadanie2_sledzenie $(BINDIR)/zadanie3_czytelnia $(BINDIR)/zadanie4_rwlock $(BINDIR)/Zadanie5Czytelnia.class

# Utworzenie katalogu bin
$(BINDIR):
	mkdir -p $(BINDIR)

# Uruchomienie wszystkich zadań po kolei
run-all: all
	@echo ""
	@$(MAKE) 1
	@echo ""
	@$(MAKE) 2
	@echo ""
	@$(MAKE) 3
	@echo ""
	@$(MAKE) 4
	@echo ""
	@$(MAKE) 5

# Czyszczenie skompilowanych plików
clean:
	rm -rf $(BINDIR)

.PHONY: all clean run-all 1 2 3 4 5
