import  java.util.Random;

public class Obraz {

    private int numberOfRows;             // Liczba wierszy tablicy
    private int numberOfColumns;          // Liczba kolumn tablicy
    private char[][] imageTable;          // Tablica 2D zawierająca znaki obrazu
    private char[] symbolsArray;          // Tablica wszystkich możliwych symboli ASCII (ograniczona do numberOfCharacters)
    private int[] histogram;              // Histogram - tablica z liczbą wystąpień każdego znaku
    private int numberOfCharacters;       // Liczba różnych znaków używanych (dla wariantu 1)

    // rows - liczba wierszy
    // columns - liczba kolumn
    // numberOfThreads - liczba wątków (określa ile różnych znaków będzie użytych)
    public Obraz(int rows, int columns, int numberOfThreads) {

	this.numberOfRows = rows;
	this.numberOfColumns = columns;

	// Dla wariantu 1: użyj tylko tyle znaków ile wątków (min 1, max 94)
	this.numberOfCharacters = Math.max(1, Math.min(numberOfThreads, 94));

	imageTable = new char[rows][columns];
	symbolsArray = new char[numberOfCharacters];  // Tylko numberOfCharacters znaków zamiast 94

	final Random random = new Random();
	
	// Inicjalizacja tablicy symboli ASCII - tylko numberOfCharacters pierwszych znaków
	// 33 = '!', 126 = '~'
	for(int k=0; k<numberOfCharacters; k++) {
	    symbolsArray[k] = (char)(k+33);
	}

	// Wypełnienie tablicy obrazu losowymi znakami z ograniczonego zbioru
	for(int row=0; row<rows; row++) {
	    for(int col=0; col<columns; col++) {
		imageTable[row][col] = symbolsArray[random.nextInt(numberOfCharacters)];  // losowy znak z numberOfCharacters znaków
		System.out.print(imageTable[row][col]+" ");
	    }
	    System.out.print("\n");
	}
	System.out.print("\n\n"); 
	
	// Inicjalizacja histogramu - tylko dla używanych znaków
	histogram = new int[numberOfCharacters];
   	clear_histogram();
    }
    
    // Zeruje histogram (ustawia wszystkie wartości na 0)
    public void clear_histogram(){
	for(int i=0; i<numberOfCharacters; i++) histogram[i]=0;
    }

    // Sekwencyjna wersja obliczania histogramu.
    // Przeszukuje całą tablicę i zlicza wystąpienia każdego znaku.
    //
    // Uwaga: Ta metoda używa bardziej ogólnego podejścia (porównanie znaków)
    // zamiast optymalnego (bezpośrednie indeksowanie), aby umożliwić
    // zrównoleglenie w dziedzinie znaków ASCII.
    public void calculate_histogram(){

	  for(int row=0; row<numberOfRows; row++) {
	    for(int col=0; col<numberOfColumns; col++) {

		// OPTYMALNA WERSJA (zakomentowana):
		// Bezpośrednie przekształcenie znaku na indeks histogramu
		// histogram[(int)imageTable[row][col]-33]++;

		// OGÓLNA WERSJA (używana):
		// Porównanie z każdym znakiem - pozwala na zrównoleglenie po znakach
		// Wątek może przeszukiwać tablicę dla konkretnego podzbioru znaków
		  for(int charIndex=0; charIndex<numberOfCharacters; charIndex++) {
		    if(imageTable[row][col] == symbolsArray[charIndex]) histogram[charIndex]++;
		  }

	    }
	  }

    }

// UNIWERSALNY WZORZEC dla różnych wariantów zrównoleglenia:
// Można go modyfikować dla różnych strategii dekompozycji albo stosować
// bezpośrednio, zmieniając tylko parametry wywołania w wątkach
//
// calculate_histogram_wzorzec(startRow, endRow, rowStep,
//                              startColumn, endColumn, columnStep,
//                              startCharacter, endCharacter, characterStep){
//
//   for(int row=startRow; row<endRow; row+=rowStep) {
//      for(int col=startColumn; col<endColumn; col+=columnStep) {
//         for(int charIndex=startCharacter; charIndex<endCharacter; charIndex+=characterStep) {
//            if(imageTable[row][col] == symbolsArray[charIndex]) histogram[charIndex]++;
//
// Przykłady użycia wzorca:
// - Podział wierszowy blokowy:    (0, rows/2, 1,   0, columns, 1,   0, 94, 1)
// - Podział wierszowy cykliczny:  (0, rows, 4,     0, columns, 1,   0, 94, 1)  // dla 4 wątków
// - Podział po znakach:           (0, rows, 1,     0, columns, 1,   0, 47, 1)  // pierwsza połowa znaków

    // Wyświetla histogram - dla każdego znaku ASCII wypisuje jego symbol i liczbę wystąpień
    // Format: 5 kolumn w wierszu dla lepszej czytelności
    public void print_histogram(){
	
	int columnsPerRow = 5;  // Liczba kolumn w wierszu

	for(int i=0; i<numberOfCharacters; i++) {
	    // Wypisz znak i jego liczbę wystąpień
	    System.out.printf("%c: %-6d\t", symbolsArray[i], histogram[i]);

	    // Po 5 kolumnach przejdź do nowej linii
	    if ((i + 1) % columnsPerRow == 0) {
		System.out.println();
	    }
	}

	// Jeśli ostatni wiersz nie był pełny, dodaj nową linię
	if (numberOfCharacters % columnsPerRow != 0) {
	    System.out.println();
	}

    }

    // Wyświetla histogram w formacie liniowym (znak po znaku), pomijając znaki z wartością 0
    // Używane dla wariantów 2-5 z pełnym zakresem znaków ASCII
    public void print_histogram_filtered(){

	System.out.println("\nFiltered histogram (non-zero values only):");
	int count = 0;

	for(int i=0; i<numberOfCharacters; i++) {
	    // Wypisz tylko znaki, które występują w obrazie
	    if (histogram[i] > 0) {
		System.out.println(symbolsArray[i] + ": " + histogram[i]);
		count++;
	    }
	}

	System.out.println("Total unique characters found: " + count);
    }

    public int getSize_n() { return numberOfRows; }
    public int getSize_m() { return numberOfColumns; }
    public char[][] getTab() { return imageTable; }
    public char[] getTab_symb() { return symbolsArray; }
    public int[] getHistogram() { return histogram; }
    public int getNumChars() { return numberOfCharacters; }  // Zwraca liczbę różnych znaków

    // Zlicza liczbę unikalnych znaków występujących w obrazie
    public int count_unique_chars() {
        boolean[] found = new boolean[numberOfCharacters];  // Tablica flag dla każdego możliwego znaku
        int count = 0;

        // Sprawdź które znaki występują w tablicy
        for (int row = 0; row < numberOfRows; row++) {
            for (int col = 0; col < numberOfColumns; col++) {
                // Znajdź indeks znaku w tablicy symboli
                for (int charIndex = 0; charIndex < numberOfCharacters; charIndex++) {
                    if (imageTable[row][col] == symbolsArray[charIndex] && !found[charIndex]) {
                        found[charIndex] = true;
                        count++;
                        break;  // Przerwij wewnętrzną pętlę po znalezieniu znaku
                    }
                }
            }
        }

        return count;
    }
}
