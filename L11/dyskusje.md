# Dyskusja - Podstawy MPI

## Tematy do dyskusji

### 1. Komunikacja punkt-punkt
- **MPI_Send / MPI_Recv** - blokujace operacje (czekaja na zakonczenie)
- **MPI_Isend / MPI_Irecv** - nieblokujace (zwracaja natychmiast, sprawdzamy przez MPI_Wait)
- **Rank** - unikalny numer procesu (0 do size-1)
- **Tag** - etykieta wiadomosci do identyfikacji
- **Status** - struktura z informacja o nadawcy (status.MPI_SOURCE)

### 2. Pamiec rozproszona vs dzielona
- MPI - kazdy proces ma wlasna pamiec (bezpieczne, dziala na klastrach)
- Watki (pthread/OpenMP) - wspolna pamiec (szybsze, tylko jeden komputer)
- W MPI komunikacja tylko przez Send/Recv, nie ma bezposredniego dostepu

### 3. Program sztafeta
- Podział: proces poczatkowy (rank 0) → procesy srodkowe (1..size-2) → proces koncowy (size-1)
- Dane przechodza sekwencyjnie przez wszystkie procesy

### 4. MPI_PACKED
- Pakowanie roznych typow danych do jednego bufora (MPI_Pack)
- Wysylanie jako MPI_PACKED
- Rozpakowywanie w tej samej kolejnosci (MPI_Unpack)
- Dziala miedzy roznymi architekturami

### 5. Przetwarzanie potokowe SPMD
- Single Program Multiple Data - jeden program, kazdy proces robi cos innego
- Dane przeplywaja przez procesy jak na tasme produkcyjnej

### 6. Wlasne typy danych
- **MPI_Type_create_struct** - definiuje typ odpowiadajacy strukturze C
- **MPI_Get_address** - pobiera adresy pol do obliczenia offsetow
- **MPI_Type_commit** - rejestruje typ przed uzyciem
- **MPI_Type_free** - zwalnia typ
- Zamiast recznie pakowac, wysylamy cala strukture jednym MPI_Send

---

## Zagadnienia do kolokwium

1. **Komunikacja punkt-punkt**
   - MPI_Send, MPI_Recv (blokujace)
   - MPI_Isend, MPI_Irecv (nieblokujace)
   - MPI_ANY_SOURCE, MPI_ANY_TAG

2. **Praca na pamieci rozproszonej**
   - Kazdy proces ma wlasna przestrzen pamieci
   - Komunikacja tylko przez MPI (Send/Recv)
   - Brak bezposredniego dostepu do pamieci innych procesow

3. **Rank i Size**
   - `rank` - numer procesu w komunikatorze (MPI_Comm_rank)
   - `size` - liczba wszystkich procesow (MPI_Comm_size)
   - MPI_COMM_WORLD - domyslny komunikator

4. **Pakowanie danych do przeslania**
   - **MPI_PACKED** - MPI_Pack/Unpack dla roznych typow
   - **MPI_Type_create_struct** - wlasny typ dla struktur
   - Koniecznosc zachowania kolejnosci przy rozpakowywaniu

