# Wnioski - Laboratorium 12

## Zadanie 2 - Analiza wydajnosci OpenMP vs MPI (N=10000, p=1,2,4,8)

### Wyniki

| p | OpenMP czas | OpenMP S(p) | OpenMP E(p) | MPI czas | MPI S(p) | MPI E(p) |
|---|-------------|-------------|-------------|----------|----------|----------|
| 1 | 0.047s | 1.00 | 100% | 0.044s | 1.00 | 100% |
| 2 | 0.024s | 1.96 | 98% | 0.023s | 1.90 | 95% |
| 4 | 0.013s | 3.62 | 90% | 0.014s | 3.08 | 77% |
| 8 | 0.009s | 5.22 | 65% | 0.017s | 2.56 | 32% |

### Analiza

**OpenMP** skaluje sie dobrze - przy 8 watkach przyspieszenie wynosi 5.2x, efektywnosc 65%. Spadek efektywnosci wynika z ograniczonej liczby rdzeni fizycznych i narzutu na zarzadzanie watkami.

**MPI** skaluje sie gorzej - przy 4 procesach osiaga S=3.1, ale przy 8 procesach przyspieszenie spada do 2.6x (efektywnosc tylko 32%). Czas wykonania przy 8 procesach jest wiekszy niz przy 4, co oznacza ze overhead komunikacji zdominowal obliczenia.

### Dlaczego OpenMP lepsze?

1. Watki dziela pamiec - brak kopiowania danych
2. Redukcja to jedna szybka operacja na koniec
3. Problem embarrassingly parallel - brak zaleznosci

### Dlaczego MPI gorsze?

1. Kazdy proces ma osobna pamiec - Bcast, Scatter, Gather kopiuja dane
2. Przy 8 procesach komunikacja trwa dluzej niz oszczednosci z rownoleglenia
3. Barrier dodatkowo synchronizuje procesy

### Wniosek

Dla problemow z pamiecia wspoldzielona OpenMP jest lepszym wyborem. MPI oplaca sie przy duzych danych rozproszonych na wiele maszyn, gdzie komunikacja sieciowa jest i tak konieczna.
