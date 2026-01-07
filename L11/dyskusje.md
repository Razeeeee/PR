# Dyskusja - Podstawy MPI

## Tematy do dyskusji

### 1. Komunikacja punkt-punkt

**MPI_Send / MPI_Recv - operacje blokujace:**

- MPI_Send blokuje do momentu gdy dane sa bezpiecznie skopiowane do bufora systemowego
- MPI_Recv blokuje dopoki nie otrzyma calej wiadomosci
- Nie gwarantuja ze druga strona juz zakonczyla operacje
- Przyklad: Jesli proces wywoluje MPI_Recv, program stoi i czeka na dane

**MPI_Isend / MPI_Irecv - operacje nieblokujace:**

- Funkcja wraca natychmiast, nie czeka na zakonczenie
- Zwraca "request" ktory sprawdzamy przez MPI_Wait lub MPI_Test
- Pozwala na kontynuacje obliczen podczas przesylania danych
- Zalety: mozliwosc nakladania komunikacji i obliczen, unikanie zakleszczen
- UWAGA: Nie mozna uzywac bufora dopoki nie wywolamy MPI_Wait!

**Elementy komunikacji:**

- **Rank** - unikalny numer procesu w komunikatorze (0 do size-1)
- **Tag** - etykieta wiadomosci pozwalajaca na filtrowanie/identyfikacje
- **Komunikator** - grupa procesow (np. MPI_COMM_WORLD)
- **Status** - struktura z informacja o nadawcy (status.MPI_SOURCE), tagu itp.
- **MPI_ANY_SOURCE** - odbierz od dowolnego procesu
- **MPI_ANY_TAG** - akceptuj dowolny tag wiadomosci

### 2. Pamiec rozproszona vs dzielona

**MPI - pamiec rozproszona:**

- Kazdy proces ma wlasna, niezalezna przestrzen pamieci
- Jeden proces NIE MOZE bezposrednio czytac/pisac do pamieci innego procesu
- Komunikacja tylko przez jawne wywolania MPI (Send/Recv)
- Bezpieczne - proces nie moze zniszczyc danych innego procesu
- Dziala na klastrach (wiele komputerow) i lokalnie
- System moze optymalizowac przesylanie (shared memory pod spodem) ale programista tego nie widzi

**Watki (pthread/OpenMP) - pamiec dzielona:**

- Wspolna pamiec dla wszystkich watkow
- Bezposredni dostep do zmiennych
- Szybsze ale wymaga synchronizacji (mutex, bariera)
- Dziala tylko na jednym komputerze

**Uruchamianie MPI lokalnie:**
Gdy uruchamiamy mpiexec -np 4 na jednym PC, tworzone sa 4 osobne procesy. Kazdy ma wlasna pamiec, mimo ze dzialaja na tym samym komputerze.

### 3. Program sztafeta

**Podzial procesow na 3 grupy:**

1. **Proces poczatkowy (rank == 0):**

   - Inicjalizuje dane (np. data = 100)
   - Wysyla do nastepnego procesu (rank + 1)

2. **Procesy srodkowe (rank > 0 && rank < size-1):**

   - Odbieraja dane od poprzednika (rank - 1)
   - Wykonuja wlasna operacje (np. data += rank)
   - Wysylaja dalej do nastepnika (rank + 1)

3. **Proces koncowy (rank == size-1):**
   - Odbiera dane od ostatniego procesu srodkowego
   - Wyswietla wynik koncowy

**Przyklad dzialania:**

- 5 procesow: 0 wysyla 100 → 1 dodaje 1 (101) → 2 dodaje 2 (103) → 3 dodaje 3 (106) → 4 odbiera wynik 106
- Dane przechodza sekwencyjnie jak paleczka w sztafecie

### 4. MPI_PACKED - pakowanie struktur

**Dlaczego pakowanie?**

- Struktury w C maja "padding" - dodatkowe bajty dla wyrownania pamieci
- Rozne komputery/kompilatory moga miec rozne wyrownania
- Bezposrednie wyslanie struktury moze nie dzialac miedzy roznymi architekturami
- MPI_PACKED zapewnia standardowy format niezalezny od platformy

**Proces pakowania:**

1. **MPI_Pack** - pakowanie do bufora:

   ```c
   char buffer[100];
   int position = 0;
   MPI_Pack(&data.id, 1, MPI_INT, buffer, 100, &position, comm);
   MPI_Pack(&data.value, 1, MPI_DOUBLE, buffer, 100, &position, comm);
   ```

   - Kazde pole pakujemy osobno z odpowiednim typem MPI
   - `position` automatycznie rosnie (in/out parameter)
   - Dane ukladane sekwencyjnie bez paddingu

2. **MPI_Send** - wysylamy buffer:

   ```c
   MPI_Send(buffer, position, MPI_PACKED, dest, tag, comm);
   ```

   - Wysylamy `position` bajtow (nie caly buffer!)

3. **MPI_Recv** - odbieramy buffer

4. **MPI_Unpack** - rozpakowywanie:
   ```c
   position = 0;  // reset!
   MPI_Unpack(buffer, size, &position, &received.id, 1, MPI_INT, comm);
   ```
   - WAZNE: Rozpakowywac w TEJ SAMEJ KOLEJNOSCI co pakowanie!

**Zalety:** Dziala miedzy roznymi architekturami, jawna kontrola  
**Wady:** Wiecej kodu, trzeba recznie okreslic kazde pole

### 5. Przetwarzanie potokowe SPMD

**SPMD = Single Program Multiple Data**

- Jeden program (ten sam kod) uruchomiony na wielu procesach
- Kazdy proces wykonuje inne operacje (rozne galezie if/else dla rank)

**Idea potoku:**

```
Proces 0         Proces 1         Proces 2         Proces 3
inicjalizacja → operacja A    → operacja B    → wynik koncowy
(×2)            (+10)           (×1.5)          (+5)
```

**Przyklad:**

- Start: 10
- Proces 0: ×2 = 20
- Proces 1: +10 = 30
- Proces 2: ×1.5 = 45
- Proces 3: +5 = 50 (wynik)

**Cechy:**

- Kazdy proces ma swoja role (swoj etap przetwarzania)
- Dane przeplywaja przez procesy jak na tasmie produkcyjnej
- Mozna rozszerzyc do przetwarzania wielu porcji danych rownolegle

### 6. Wlasne typy danych - MPI_Type_create_struct

**Zamiast recznego pakowania:**  
Zamiast recznie pakowac kazde pole struktury, tworzymy wlasny typ MPI ktory odpowiada strukturze.

**Kroki tworzenia:**

1. **Definicja struktury i parametrow:**

   ```c
   int blocklengths[3] = {1, 1, 10};  // ile elementow kazdego pola
   MPI_Datatype types[3] = {MPI_INT, MPI_DOUBLE, MPI_CHAR};
   MPI_Aint displacements[3];  // offsety pol
   ```

2. **MPI_Get_address** - pobieranie adresow pol:

   ```c
   SensorData sample;
   MPI_Get_address(&sample.id, &displacements[0]);
   MPI_Get_address(&sample.temp, &displacements[1]);
   ```

   - Potrzebne do obliczenia offsetow (przesuniec) miedzy polami
   - Uwzglednia padding automatycznie

3. **Obliczenie wzglednych przesuniec:**

   ```c
   displacements[1] -= displacements[0];
   displacements[0] = 0;
   ```

4. **MPI_Type_create_struct** - tworzenie typu:

   ```c
   MPI_Type_create_struct(3, blocklengths, displacements, types, &sensor_type);
   ```

5. **MPI_Type_commit** - rejestracja:

   ```c
   MPI_Type_commit(&sensor_type);
   ```

   - Typ musi byc zatwierdzony przed uzyciem!

6. **Uzycie:**

   ```c
   MPI_Send(&struktura, 1, sensor_type, dest, tag, comm);
   MPI_Recv(&struktura, 1, sensor_type, src, tag, comm, &status);
   ```

   - Wysylamy cala strukture jednym wywolaniem
   - "1" oznacza 1 strukture (mozna wysylac tablice struktur)

7. **MPI_Type_free** - sprzatanie:
   ```c
   MPI_Type_free(&sensor_type);
   ```
   - Przed MPI_Finalize

**Zalety vs MPI_PACKED:**

- Czystszy kod (jedno Send/Recv zamiast Pack/Unpack)
- MPI automatycznie dba o szczegoly
- Bardziej wydajne
- Latwiejsze w utrzymaniu
- Gdy czesto przesylamy ta sama strukture

---

## Zagadnienia do kolokwium

1. **Komunikacja punkt-punkt**

   - MPI_Send, MPI_Recv (blokujace) - czekaja na zakonczenie operacji
   - MPI_Isend, MPI_Irecv (nieblokujace) - zwracaja natychmiast, sprawdzamy przez MPI_Wait
   - MPI_ANY_SOURCE - odbierz od dowolnego procesu
   - MPI_ANY_TAG - akceptuj dowolny tag wiadomosci
   - Status - zawiera informacje o nadawcy (status.MPI_SOURCE)

2. **Praca na pamieci rozproszonej**

   - Kazdy proces ma wlasna, oddzielna przestrzen pamieci
   - Brak bezposredniego dostepu do pamieci innych procesow
   - Komunikacja tylko przez jawne wywolania MPI (Send/Recv)
   - Bezpieczniejsze niz pamiec dzielona (brak wyscigów)
   - Dziala na klastrach (wiele komputerow) i lokalnie

3. **Rank i Size**

   - `rank` - unikalny numer procesu w komunikatorze, pobierany przez MPI_Comm_rank
   - `size` - calkowita liczba procesow, pobierana przez MPI_Comm_size
   - MPI_COMM_WORLD - domyslny komunikator zawierajacy wszystkie procesy
   - Rank okreslany od 0 do size-1
   - Uzywane do rozrozniania procesow i podzialu zadan (if rank == 0, else if...)

4. **Pakowanie danych do przeslania**

   - **Sposob 1 - MPI_PACKED:**

     - MPI_Pack - pakuje kolejne pola do bufora
     - MPI_Send(buffer, position, MPI_PACKED, ...) - wysyla
     - MPI_Unpack - rozpakowuje w TEJ SAMEJ kolejnosci
     - Wiecej kodu ale pelna kontrola

   - **Sposob 2 - MPI_Type_create_struct:**
     - Tworzymy wlasny typ MPI odpowiadajacy strukturze
     - MPI_Get_address - pobiera offsety pol
     - MPI_Type_create_struct + MPI_Type_commit - rejestruje typ
     - MPI_Send(&struktura, 1, nasz_typ, ...) - wysyla cala strukture
     - Czystszy kod, bardziej wydajne
     - MPI_Type_free - zwalnia typ przed MPI_Finalize
