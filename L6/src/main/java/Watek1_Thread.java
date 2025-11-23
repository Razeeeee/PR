// Każdy wątek odpowiada za zliczanie wystąpień jednego konkretnego
// znaku ASCII w całej tablicy znaków.
public class Watek1_Thread extends Thread {
  private char[][] imageTable;              // Tablica 2D ze znakami obrazu
  private char[] symbolsArray;              // Tablica symboli ASCII (94 znaki)
  private int[] histogram;                  // Wspólny histogram (tablica wyników)
  private int characterIndex;               // Indeks znaku, który zlicza ten wątek
  private int numberOfRows;                 // Liczba wierszy tablicy
  private int numberOfColumns;              // Liczba kolumn tablicy

  // characterIndex - indeks znaku w tablicy symbolsArray, który będzie zliczany
  // image - obiekt obrazu zawierający dane do przetworzenia
  public Watek1_Thread(int characterIndex, Obraz image) {
    this.characterIndex = characterIndex;
    this.imageTable = image.getTab();
    this.symbolsArray = image.getTab_symb();
    this.histogram = image.getHistogram();
    this.numberOfRows = image.getSize_n();
    this.numberOfColumns = image.getSize_m();
  }

  @Override
  public void run() {
    // Pobierz znak, który ma być zliczany przez ten wątek
    char searchedCharacter = symbolsArray[characterIndex];
    int count = 0;

    // Przeszukaj całą tablicę i zlicz wystąpienia szukanego znaku
    for (int row = 0; row < numberOfRows; row++) {
      for (int col = 0; col < numberOfColumns; col++) {
        if (imageTable[row][col] == searchedCharacter) {
          count++;
        }
      }
    }

    // Synchronizacja: bezpieczny zapis wyniku do wspólnego histogramu
    synchronized (histogram) {
      histogram[characterIndex] = count;
    }
  }
}
