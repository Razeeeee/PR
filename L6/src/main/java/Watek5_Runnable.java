// Wariant 5: Runnable - podział 2D blokowy
//
// Strategia: Tablica jest dzielona na prostokątne bloki 2D. Każdy wątek
// przetwarza przydzielony blok (zakres wierszy i kolumn).
//
// Zalety: Bardzo dobra lokalność danych, optymalne wykorzystanie cache CPU
// Wady: Wymaga starannego doboru liczby wątków (idealnie kwadraty: 4, 9, 16, ...)
public class Watek5_Runnable implements Runnable {
    private char[][] imageTable;              // Tablica 2D ze znakami obrazu
    private char[] symbolsArray;              // Tablica symboli ASCII (94 znaki)
    private int[] histogram;                  // Wspólny histogram (tablica wyników)
    private int startRow;                     // Indeks pierwszego wiersza do przetworzenia
    private int endRow;                       // Indeks ostatniego wiersza (exclusive)
    private int startColumn;                  // Indeks pierwszej kolumny do przetworzenia
    private int endColumn;                    // Indeks ostatniej kolumny (exclusive)
    private int numberOfRows;                 // Liczba wierszy tablicy
    private int numberOfColumns;              // Liczba kolumn tablicy
    private int[] localHistogram;             // Lokalny histogram dla tego wątku

    // Konstruktor wątku
    // startRow - indeks pierwszego wiersza bloku
    // endRow - indeks ostatniego wiersza bloku (exclusive)
    // startColumn - indeks pierwszej kolumny bloku
    // endColumn - indeks ostatniej kolumny bloku (exclusive)
    // image - obiekt obrazu zawierający dane do przetworzenia
    public Watek5_Runnable(int startRow, int endRow, int startColumn, int endColumn, Obraz image) {
        this.startRow = startRow;
        this.endRow = endRow;
        this.startColumn = startColumn;
        this.endColumn = endColumn;
        this.imageTable = image.getTab();
        this.symbolsArray = image.getTab_symb();
        this.histogram = image.getHistogram();
        this.numberOfRows = image.getSize_n();
        this.numberOfColumns = image.getSize_m();
        this.localHistogram = new int[image.getNumChars()];  // Lokalny bufor zmniejsza rywalizację o blokadę
    }

    @Override
    public void run() {
        // Podział 2D blokowy: przetwarzaj tylko przydzielony prostokątny fragment tablicy
        for (int row = startRow; row < endRow; row++) {
            for (int col = startColumn; col < endColumn; col++) {
                // Zlicz wszystkie znaki w tym elemencie tablicy
                int numberOfCharacters = symbolsArray.length;  // Liczba różnych znaków
                for (int charIndex = 0; charIndex < numberOfCharacters; charIndex++) {
                    if (imageTable[row][col] == symbolsArray[charIndex]) {
                        localHistogram[charIndex]++;  // Zapis do lokalnego histogramu
                    }
                }
            }
        }

        // Scalanie wyników: dodaj lokalny histogram do globalnego
        // Synchronizacja tylko raz na koniec, zamiast po każdym znaku
        synchronized (histogram) {
            int numberOfCharacters = symbolsArray.length;
            for (int charIndex = 0; charIndex < numberOfCharacters; charIndex++) {
                histogram[charIndex] += localHistogram[charIndex];
            }
        }
    }
}
