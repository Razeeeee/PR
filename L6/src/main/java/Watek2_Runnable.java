// Znaki ASCII są dzielone na bloki. Każdy wątek przetwarza
// ciągły zakres znaków (np. wątek 0: znaki 0-23, wątek 1: znaki 24-47, itd.)
public class Watek2_Runnable implements Runnable {
    private char[][] imageTable;              // Tablica 2D ze znakami obrazu
    private char[] symbolsArray;              // Tablica symboli ASCII (94 znaki)
    private int[] histogram;                  // Wspólny histogram (tablica wyników)
    private int startCharacterIndex;          // Indeks pierwszego znaku do zliczenia
    private int endCharacterIndex;            // Indeks ostatniego znaku
    private int numberOfRows;                 // Liczba wierszy tablicy
    private int numberOfColumns;              // Liczba kolumn tablicy

    // startCharacterIndex - indeks pierwszego znaku do przetworzenia
    // endCharacterIndex - indeks ostatniego znaku
    // image - obiekt obrazu zawierający dane do przetworzenia
    public Watek2_Runnable(int startCharacterIndex, int endCharacterIndex, Obraz image) {
        this.startCharacterIndex = startCharacterIndex;
        this.endCharacterIndex = endCharacterIndex;
        this.imageTable = image.getTab();
        this.symbolsArray = image.getTab_symb();
        this.histogram = image.getHistogram();
        this.numberOfRows = image.getSize_n();
        this.numberOfColumns = image.getSize_m();
    }

    @Override
    public void run() {
        // Przetwarzaj każdy znak z przydzielonego zakresu
        for (int charIndex = startCharacterIndex; charIndex < endCharacterIndex; charIndex++) {
            char searchedCharacter = symbolsArray[charIndex];
            int count = 0;

            // Przeszukaj całą tablicę dla bieżącego znaku
            for (int row = 0; row < numberOfRows; row++) {
                for (int col = 0; col < numberOfColumns; col++) {
                    if (imageTable[row][col] == searchedCharacter) {
                        count++;
                    }
                }
            }

            // Synchronizacja: bezpieczny zapis wyniku do wspólnego histogramu
            synchronized (histogram) {
                histogram[charIndex] = count;
            }
        }
    }
}
