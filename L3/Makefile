# Makefile dla programów pthread

# Kompilator
CC = gcc

# Flagi kompilacji
CFLAGS = -Wall -Wextra -std=c99 -g

# Biblioteki
LIBS = -lpthread

# Pliki źródłowe i wykonywalne
SOURCE1 = pthreads_detach_kill.c
TARGET1 = pthreads_detach_kill

SOURCE2 = thread_ids.c
TARGET2 = thread_ids

SOURCE3 = struct_thread.c
TARGET3 = struct_thread

SOURCE4 = test_2_watki.c
TARGET4 = test_2_watki

# Domyślny cel - kompiluje wszystkie programy
all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

# Kompilacja pierwszego programu
$(TARGET1): $(SOURCE1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SOURCE1) $(LIBS)

# Kompilacja drugiego programu
$(TARGET2): $(SOURCE2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SOURCE2) $(LIBS)

# Kompilacja trzeciego programu
$(TARGET3): $(SOURCE3)
	$(CC) $(CFLAGS) -o $(TARGET3) $(SOURCE3) $(LIBS)

# Kompilacja czwartego programu
$(TARGET4): $(SOURCE4)
	$(CC) $(CFLAGS) -o $(TARGET4) $(SOURCE4) $(LIBS)

# Uruchomienie pierwszego programu
run1: $(TARGET1)
	@echo "=== Uruchamianie $(TARGET1) ==="
	./$(TARGET1)

# Uruchomienie drugiego programu
run2: $(TARGET2)
	@echo "=== Uruchamianie $(TARGET2) ==="
	./$(TARGET2)

# Uruchomienie trzeciego programu
run3: $(TARGET3)
	@echo "=== Uruchamianie $(TARGET3) ==="
	./$(TARGET3)

# Uruchomienie czwartego programu
run4: $(TARGET4)
	@echo "=== Uruchamianie $(TARGET4) ==="
	./$(TARGET4)

# Uruchomienie wszystkich programów
run: run1 run2 run3 run4

# Czyszczenie plików
clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

# Reinstalacja (czyszczenie i kompilacja)
rebuild: clean all

# Informacje o celach
help:
	@echo "Dostępne cele:"
	@echo "  all      - kompiluje wszystkie programy (domyślne)"
	@echo "  $(TARGET1) - kompiluje tylko program detach/kill"
	@echo "  $(TARGET2) - kompiluje tylko program thread IDs"
	@echo "  $(TARGET3) - kompiluje tylko program struct_thread"
	@echo "  $(TARGET4) - kompiluje tylko program test_2_watki"
	@echo "  run      - kompiluje i uruchamia wszystkie programy"
	@echo "  run1     - kompiluje i uruchamia program detach/kill"
	@echo "  run2     - kompiluje i uruchamia program thread IDs"
	@echo "  run3     - kompiluje i uruchamia program struct_thread"
	@echo "  run4     - kompiluje i uruchamia program test_2_watki"
	@echo "  clean    - usuwa pliki wykonywalne"
	@echo "  rebuild  - czyści i kompiluje ponownie"
	@echo "  help     - wyświetla tę pomoc"

# Deklaracja celów, które nie tworzą plików
.PHONY: all run run1 run2 clean rebuild help
