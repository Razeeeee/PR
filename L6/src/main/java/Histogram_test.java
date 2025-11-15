import java.util.Scanner;

// Program główny do testowania różnych wariantów wielowątkowego obliczania histogramu.
//
// Umożliwia wybór spośród 5 różnych strategii zrównoleglenia:
// 1. Thread - jeden wątek na znak
// 2. Runnable - podział blokowy po znakach
// 3. Runnable - podział cykliczny wierszowy
// 4. Runnable - podział kolumnowy blokowy
// 5. Runnable - podział 2D blokowy
public class Histogram_test {

    public static void main(String[] args) {

	Scanner scanner = new Scanner(System.in);
	
	// Wczytaj liczbę wątków
	System.out.println("Set number of threads");
	int numberOfThreads = scanner.nextInt();

	// Wczytaj rozmiar obrazu
	System.out.println("Set image size: n (#rows), m(#columns)");
	int numberOfRows = scanner.nextInt();
	int numberOfColumns = scanner.nextInt();

	// Wyświetl menu wyboru wariantu
	System.out.println("Select variant (1-5):");
	System.out.println("1 - Thread: jeden wątek zlicza jeden znak");
	System.out.println("2 - Runnable: podział 1D blokowy po znakach");
	System.out.println("3 - Runnable: podział cykliczny wierszowy");
	System.out.println("4 - Runnable: podział kolumnowy blokowy");
	System.out.println("5 - Runnable: podział 2D blokowy");
	int selectedVariant = scanner.nextInt();

	// Utwórz obraz z odpowiednią liczbą znaków w zależności od wariantu
	// Wariant 1: ograniczona liczba znaków (numberOfThreads)
	// Warianty 2-5: pełny zakres 94 znaków ASCII
	Obraz image;
	if (selectedVariant == 1) {
	    image = new Obraz(numberOfRows, numberOfColumns, numberOfThreads);  // Ograniczona liczba znaków
	} else {
	    image = new Obraz(numberOfRows, numberOfColumns, 94);  // Pełny zakres znaków
	}

	// Rozpocznij pomiar czasu
	long startTime = System.nanoTime();

	// Uruchom wybrany wariant
	switch(selectedVariant) {
	    case 1:
		runVariant1(numberOfThreads, image);
		break;
	    case 2:
		runVariant2(numberOfThreads, image);
		break;
	    case 3:
		runVariant3(numberOfThreads, image);
		break;
	    case 4:
		runVariant4(numberOfThreads, image);
		break;
	    case 5:
		runVariant5(numberOfThreads, image);
		break;
	    default:
		System.out.println("Invalid variant! Using sequential version.");
		image.calculate_histogram();
	}

	// Zakończ pomiar czasu
	long endTime = System.nanoTime();
	double executionTime = (endTime - startTime) / 1e6; // konwersja na milisekundy

	// Wyświetl wyniki - normalny histogram
	image.print_histogram();

	// Dla wariantów 2-5 dodatkowo wypisz histogram z odfiltrowanymi zerami
	if (selectedVariant >= 2 && selectedVariant <= 5) {
	    // Pozwól metodzie print_histogram_filtered sama wypisać nagłówek
	    image.print_histogram_filtered();
	}

	System.out.println("\nExecution time: " + executionTime + " ms");

	scanner.close();
    }

    // Wariant 1: Thread - jeden wątek zlicza jeden znak
    // Tworzy wątki dla każdego możliwego znaku (ograniczone przez numberOfThreads)
    private static void runVariant1(int numberOfThreads, Obraz image) {
	int numberOfCharacters = image.getNumChars();
	System.out.println("Liczba różnych znaków użytych w obrazie: " + numberOfCharacters);

	// Utwórz tablicę wątków - jeden wątek na każdy znak
	Watek1_Thread[] threads = new Watek1_Thread[numberOfCharacters];

	// Utwórz i uruchom wątki dla wszystkich znaków
	for (int i = 0; i < numberOfCharacters; i++) {
	    threads[i] = new Watek1_Thread(i, image);
	    threads[i].start();
	}

	// Czekaj na zakończenie wszystkich wątków
	for (int i = 0; i < numberOfCharacters; i++) {
	    try {
		threads[i].join();
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
    }

    // Wariant 2: Runnable - podział 1D blokowy po znakach
    // Znaki są dzielone na równe bloki między wątki
    private static void runVariant2(int numberOfThreads, Obraz image) {
	Thread[] threads = new Thread[numberOfThreads];
	int numberOfCharacters = image.getNumChars();
	int charactersPerThread = numberOfCharacters / numberOfThreads;
	int remainder = numberOfCharacters % numberOfThreads;

	int startIndex = 0;
	// Utwórz wątki z podziałem bloków znaków
	for (int i = 0; i < numberOfThreads; i++) {
	    // Rozdziel resztę z dzielenia między pierwsze wątki
	    int extraCharacters = (i < remainder) ? 1 : 0;
	    int endIndex = startIndex + charactersPerThread + extraCharacters;

	    threads[i] = new Thread(new Watek2_Runnable(startIndex, endIndex, image));
	    threads[i].start();

	    startIndex = endIndex;
	}

	// Czekaj na zakończenie wszystkich wątków
	for (int i = 0; i < numberOfThreads; i++) {
	    try {
		threads[i].join();
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
    }

    // Wariant 3: Runnable - podział cykliczny wierszowy
    // Wiersze są przydzielane cyklicznie (round-robin)
    private static void runVariant3(int numberOfThreads, Obraz image) {
	Thread[] threads = new Thread[numberOfThreads];

	// Utwórz wątki - każdy przetwarza co numberOfThreads-ty wiersz
	for (int i = 0; i < numberOfThreads; i++) {
	    threads[i] = new Thread(new Watek3_Runnable(i, numberOfThreads, image));
	    threads[i].start();
	}

	// Czekaj na zakończenie wszystkich wątków
	for (int i = 0; i < numberOfThreads; i++) {
	    try {
		threads[i].join();
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
    }

    // Wariant 4: Runnable - podział kolumnowy blokowy
    // Kolumny są dzielone na równe bloki między wątki
    private static void runVariant4(int numberOfThreads, Obraz image) {
	Thread[] threads = new Thread[numberOfThreads];
	int numberOfColumns = image.getSize_m();
	int columnsPerThread = numberOfColumns / numberOfThreads;
	int remainder = numberOfColumns % numberOfThreads;

	int startColumn = 0;
	// Utwórz wątki z podziałem bloków kolumn
	for (int i = 0; i < numberOfThreads; i++) {
	    // Rozdziel resztę z dzielenia między pierwsze wątki
	    int extraColumns = (i < remainder) ? 1 : 0;
	    int endColumn = startColumn + columnsPerThread + extraColumns;

	    threads[i] = new Thread(new Watek4_Runnable(startColumn, endColumn, image));
	    threads[i].start();

	    startColumn = endColumn;
	}

	// Czekaj na zakończenie wszystkich wątków
	for (int i = 0; i < numberOfThreads; i++) {
	    try {
		threads[i].join();
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
    }

    // Wariant 5: Runnable - podział 2D blokowy
    // Tablica jest dzielona na prostokątne bloki 2D
    // Liczba wątków jest zaokrąglana do najbliższej siatki (np. 7 wątków -> siatka 2x3 = 6 wątków)
    private static void runVariant5(int numberOfThreads, Obraz image) {
	int numberOfRows = image.getSize_n();
	int numberOfColumns = image.getSize_m();

	// Dla prostoty: dzielimy na siatki (np. 2x2, 2x3, itd.)
	// Oblicz optymalny układ wątków w siatce 2D
	int threadsSqrt = (int) Math.sqrt(numberOfThreads);
	int threadsInRows = threadsSqrt;
	int threadsInColumns = (numberOfThreads + threadsSqrt - 1) / threadsSqrt;
	int actualThreadCount = threadsInRows * threadsInColumns;

	Thread[] threads = new Thread[actualThreadCount];

	int rowsPerThread = numberOfRows / threadsInRows;
	int columnsPerThread = numberOfColumns / threadsInColumns;

	int threadIndex = 0;
	// Utwórz wątki dla każdego bloku w siatce 2D
	for (int i = 0; i < threadsInRows; i++) {
	    for (int j = 0; j < threadsInColumns; j++) {
		// Oblicz granice bloku dla tego wątku
		int startRow = i * rowsPerThread;
		int endRow = (i == threadsInRows - 1) ? numberOfRows : (i + 1) * rowsPerThread;
		int startColumn = j * columnsPerThread;
		int endColumn = (j == threadsInColumns - 1) ? numberOfColumns : (j + 1) * columnsPerThread;

		// Utwórz wątek Runnable dla tego bloku
		threads[threadIndex] = new Thread(new Watek5_Runnable(startRow, endRow, startColumn, endColumn, image));
		threads[threadIndex].start();
		threadIndex++;
	    }
	}

	// Czekaj na zakończenie wszystkich wątków
	for (int i = 0; i < actualThreadCount; i++) {
	    try {
		threads[i].join();
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
    }

}
