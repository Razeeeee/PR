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

# Domyślny cel - kompiluje oba programy
all: $(TARGET1) $(TARGET2)

# Kompilacja pierwszego programu
$(TARGET1): $(SOURCE1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SOURCE1) $(LIBS)

# Kompilacja drugiego programu
$(TARGET2): $(SOURCE2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SOURCE2) $(LIBS)

# Uruchomienie pierwszego programu
run1: $(TARGET1)
	@echo "=== Uruchamianie $(TARGET1) ==="
	./$(TARGET1)

# Uruchomienie drugiego programu
run2: $(TARGET2)
	@echo "=== Uruchamianie $(TARGET2) ==="
	./$(TARGET2)

# Uruchomienie obu programów
run: run1 run2

# Czyszczenie plików
clean:
	rm -f $(TARGET1) $(TARGET2)

# Reinstalacja (czyszczenie i kompilacja)
rebuild: clean all

# Informacje o celach
help:
	@echo "Dostępne cele:"
	@echo "  all      - kompiluje oba programy (domyślne)"
	@echo "  $(TARGET1) - kompiluje tylko program detach/kill"
	@echo "  $(TARGET2) - kompiluje tylko program thread IDs"
	@echo "  run      - kompiluje i uruchamia oba programy"
	@echo "  run1     - kompiluje i uruchamia program detach/kill"
	@echo "  run2     - kompiluje i uruchamia program thread IDs"
	@echo "  clean    - usuwa pliki wykonywalne"
	@echo "  rebuild  - czyści i kompiluje ponownie"
	@echo "  help     - wyświetla tę pomoc"

# Deklaracja celów, które nie tworzą plików
.PHONY: all run run1 run2 clean rebuild help
