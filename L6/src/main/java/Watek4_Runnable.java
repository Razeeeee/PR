// Kolumny są dzielone na ciągłe bloki. Każdy wątek przetwarza
// przydzielony zakres kolumn dla wszystkich wierszy.
public class Watek4_Runnable implements Runnable {
  private char[][] imageTable;              // Tablica 2D ze znakami obrazu
  private char[] symbolsArray;              // Tablica symboli ASCII (94 znaki)
  private int[] histogram;                  // Wspólny histogram (tablica wyników)
  private int startColumn;                  // Indeks pierwszej kolumny do przetworzenia
  private int endColumn;                    // Indeks ostatniej kolumny
  private int numberOfRows;                 // Liczba wierszy tablicy
  private int numberOfColumns;              // Liczba kolumn tablicy
  private int[] localHistogram;             // Lokalny histogram dla tego wątku

  // startColumn - indeks pierwszej kolumny do przetworzenia
  // endColumn - indeks ostatniej kolumny
  // image - obiekt obrazu zawierający dane do przetworzenia
  public Watek4_Runnable(int startColumn, int endColumn, Obraz image) {
    this.startColumn = startColumn;
    this.endColumn = endColumn;
    this.imageTable = image.getTab();
    this.symbolsArray = image.getTab_symb();
    this.histogram = image.getHistogram();
    this.numberOfRows = image.getSize_n();
    this.numberOfColumns = image.getSize_m();
    this.localHistogram = new int[image.getNumChars()];
  }

  @Override
  public void run() {
    // Podział blokowy kolumnowy: przetwarzaj wszystkie wiersze dla przydzielonych kolumn
    for (int row = 0; row < numberOfRows; row++) {
      // Przetwórz tylko kolumny z przydzielonego zakresu
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
