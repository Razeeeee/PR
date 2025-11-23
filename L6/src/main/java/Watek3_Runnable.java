// Wiersze są przydzielane wątkom cyklicznie:
// Wątek 0 przetwarza wiersze 0, numberOfThreads, 2*numberOfThreads, ...
// Wątek 1 przetwarza wiersze 1, numberOfThreads+1, 2*numberOfThreads+1, ...
public class Watek3_Runnable implements Runnable {
  private char[][] imageTable;              // Tablica 2D ze znakami obrazu
  private char[] symbolsArray;              // Tablica symboli ASCII (94 znaki)
  private int[] histogram;                  // Wspólny histogram (tablica wyników)
  private int threadId;                     // Identyfikator wątku (0, 1, 2, ...)
  private int totalThreads;                 // Całkowita liczba wątków
  private int numberOfRows;                 // Liczba wierszy tablicy
  private int numberOfColumns;              // Liczba kolumn tablicy
  private int[] localHistogram;             // Lokalny histogram dla tego wątku

  // threadId - identyfikator wątku (określa które wiersze przetwarza)
  // totalThreads - całkowita liczba wątków
  // image - obiekt obrazu zawierający dane do przetworzenia
  public Watek3_Runnable(int threadId, int totalThreads, Obraz image) {
    this.threadId = threadId;
    this.totalThreads = totalThreads;
    this.imageTable = image.getTab();
    this.symbolsArray = image.getTab_symb();
    this.histogram = image.getHistogram();
    this.numberOfRows = image.getSize_n();
    this.numberOfColumns = image.getSize_m();
    this.localHistogram = new int[image.getNumChars()];
  }

  @Override
  public void run() {
    // Podział cykliczny wierszowy: wątek przetwarza co totalThreads-ty wiersz
    for (int row = threadId; row < numberOfRows; row += totalThreads) {
      // Przetwórz wszystkie kolumny w bieżącym wierszu
      for (int col = 0; col < numberOfColumns; col++) {
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
