package org.example;

import java.util.Arrays;
import java.util.Random;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n=== MENU DEMONSTRACJI PROGRAMOWANIA RÓWNOLEGŁEGO ===");
            System.out.println("3.0 - Obliczanie całki (sekwencyjne i równoległe)");
            System.out.println("4.0 - Sortowanie przez scalanie (ForkJoinPool)");
            System.out.println("4.5 - Obliczanie całki z interfejsem Runnable");
            System.out.println("5.0 - Histogram równoległy (pula wątków)");
            System.out.println("0   - Wyjście");
            System.out.print("\nWybierz opcję: ");

            String choice = scanner.nextLine();

            try {
                switch (choice) {
                    case "3.0":
                        demonstrateIntegralCalculation(scanner);
                        break;
                    case "4.0":
                        demonstrateMergeSort(scanner);
                        break;
                    case "4.5":
                        demonstrateIntegralWithRunnable(scanner);
                        break;
                    case "5.0":
                        demonstrateParallelHistogram(scanner);
                        break;
                    case "0":
                        System.out.println("Koniec programu.");
                        scanner.close();
                        return;
                    default:
                        System.out.println("Nieprawidłowa opcja. Spróbuj ponownie.");
                }
            } catch (Exception e) {
                System.out.println("Wystąpił błąd: " + e.getMessage());
                e.printStackTrace();
            }
        }
    }

    private static void demonstrateIntegralCalculation(Scanner scanner) throws Exception {
        System.out.println("\n=== OBLICZANIE CAŁKI: ∫(x²)dx ===");

        System.out.print("Podaj dolną granicę całkowania (a): ");
        double a = Double.parseDouble(scanner.nextLine());

        System.out.print("Podaj górną granicę całkowania (b): ");
        double b = Double.parseDouble(scanner.nextLine());

        System.out.print("Podaj krok całkowania (dx): ");
        double dx = Double.parseDouble(scanner.nextLine());

        // Sequential calculation
        System.out.println("\n--- Wariant sekwencyjny ---");
        CalkaSequential sequential = new CalkaSequential(a, b, dx);
        long startTime = System.currentTimeMillis();
        double resultSeq = sequential.calculate();
        long endTime = System.currentTimeMillis();
        System.out.println("Wynik: " + resultSeq);
        System.out.println("Czas: " + (endTime - startTime) + " ms");

        // Parallel calculation with ExecutorService
        System.out.print("\nPodaj liczbę wątków dla obliczeń równoległych: ");
        int numThreads = Integer.parseInt(scanner.nextLine());

        System.out.println("\n--- Wariant równoległy (ExecutorService + Callable + Future) ---");
        CalkaParallel parallel = new CalkaParallel(a, b, dx, numThreads);
        startTime = System.currentTimeMillis();
        double resultPar = parallel.calculate();
        endTime = System.currentTimeMillis();
        System.out.println("Wynik: " + resultPar);
        System.out.println("Czas: " + (endTime - startTime) + " ms");

        System.out.println("\nRóżnica między wynikami: " + Math.abs(resultSeq - resultPar));
        System.out.println("Wartość analityczna (x³/3): " + ((b*b*b/3) - (a*a*a/3)));
    }

    private static void demonstrateMergeSort(Scanner scanner) {
        System.out.println("\n=== SORTOWANIE PRZEZ SCALANIE (ForkJoinPool) ===");

        System.out.print("Podaj rozmiar tablicy do posortowania: ");
        int size = Integer.parseInt(scanner.nextLine());

        System.out.print("Podaj zakres wartości (max): ");
        int maxValue = Integer.parseInt(scanner.nextLine());

        // Generate random array
        Random random = new Random();
        int[] array = new int[size];
        for (int i = 0; i < size; i++) {
            array[i] = random.nextInt(maxValue);
        }

        if (size <= 20) {
            System.out.println("Tablica przed sortowaniem: " + Arrays.toString(array));
        } else {
            System.out.println("Pierwsze 20 elementów przed sortowaniem: " +
                Arrays.toString(Arrays.copyOf(array, 20)));
        }

        long startTime = System.currentTimeMillis();
        int[] sorted = MergeSortDemo.sortWithForkJoin(array);
        long endTime = System.currentTimeMillis();

        if (size <= 20) {
            System.out.println("Tablica po sortowaniu:    " + Arrays.toString(sorted));
        } else {
            System.out.println("Pierwsze 20 elementów po sortowaniu:  " +
                Arrays.toString(Arrays.copyOf(sorted, 20)));
        }

        System.out.println("Czas sortowania: " + (endTime - startTime) + " ms");

        // Verify if sorted
        boolean isSorted = true;
        for (int i = 0; i < sorted.length - 1; i++) {
            if (sorted[i] > sorted[i + 1]) {
                isSorted = false;
                break;
            }
        }
        System.out.println("Tablica posortowana poprawnie: " + (isSorted ? "TAK" : "NIE"));
    }

    private static void demonstrateIntegralWithRunnable(Scanner scanner) throws Exception {
        System.out.println("\n=== OBLICZANIE CAŁKI Z INTERFEJSEM RUNNABLE ===");
        System.out.println("(Wyniki przekazywane przez pole w obiekcie zamiast Future)");

        System.out.print("Podaj dolną granicę całkowania (a): ");
        double a = Double.parseDouble(scanner.nextLine());

        System.out.print("Podaj górną granicę całkowania (b): ");
        double b = Double.parseDouble(scanner.nextLine());

        System.out.print("Podaj krok całkowania (dx): ");
        double dx = Double.parseDouble(scanner.nextLine());

        System.out.print("Podaj liczbę wątków: ");
        int numThreads = Integer.parseInt(scanner.nextLine());

        CalkaRunnableParallel runnableCalc = new CalkaRunnableParallel(a, b, dx, numThreads);
        long startTime = System.currentTimeMillis();
        double result = runnableCalc.calculate();
        long endTime = System.currentTimeMillis();

        System.out.println("\nWynik: " + result);
        System.out.println("Czas: " + (endTime - startTime) + " ms");
        System.out.println("Wartość analityczna (x³/3): " + ((b*b*b/3) - (a*a*a/3)));
    }

    private static void demonstrateParallelHistogram(Scanner scanner) throws Exception {
        System.out.println("\n=== HISTOGRAM RÓWNOLEGŁY (ThreadPoolExecutor) ===");

        System.out.print("Podaj liczbę wątków: ");
        int numThreads = Integer.parseInt(scanner.nextLine());

        System.out.print("Podaj rozmiar obrazu - liczba wierszy: ");
        int numberOfRows = Integer.parseInt(scanner.nextLine());

        System.out.print("Podaj rozmiar obrazu - liczba kolumn: ");
        int numberOfColumns = Integer.parseInt(scanner.nextLine());

        // Utwórz obraz z pełnym zakresem 94 znaków ASCII (podobnie jak w Histogram_test)
        Obraz image = new Obraz(numberOfRows, numberOfColumns, 94);

        System.out.println("\nGenerowanie histogramu dla obrazu " + numberOfRows + "x" + numberOfColumns + "...");

        // Wariant równoległy - podział cykliczny wierszowy (podobny do wariantu 3 z Histogram_test)
        Thread[] threads = new Thread[numThreads];

        long startTime = System.currentTimeMillis();

        // Utwórz wątki - każdy przetwarza co numThreads-ty wiersz
        for (int i = 0; i < numThreads; i++) {
            threads[i] = new Thread(new Watek3_Runnable(i, numThreads, image));
            threads[i].start();
        }

        // Czekaj na zakończenie wszystkich wątków
        for (int i = 0; i < numThreads; i++) {
            threads[i].join();
        }

        long endTime = System.currentTimeMillis();

        // Wyświetl wyniki
        image.print_histogram_filtered();

        System.out.println("\nCzas obliczeń równoległych: " + (endTime - startTime) + " ms");
        System.out.println("Liczba wątków: " + numThreads);

        // Weryfikacja - policz sumę wszystkich znaków
        int[] histogram = image.getHistogram();
        int total = 0;
        for (int count : histogram) {
            total += count;
        }
        int expected = numberOfRows * numberOfColumns;
        System.out.println("Suma elementów w histogramie: " + total + " (oczekiwano: " + expected + ")");
        System.out.println("Weryfikacja: " + (total == expected ? "POPRAWNA" : "BŁĄD!"));
    }
}
