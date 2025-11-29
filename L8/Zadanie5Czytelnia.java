// Implementacja czytelni w Javie z wykorzystaniem monitorow
// Wyklad 6, slajdy 10-12

class Czytelnia {
    private int liczba_czyt = 0;
    private int liczba_pisz = 0;
    private int czekajacy_pisarze = 0; // Zastepuje empty(pisarze)

    // Monitor - metody synchronized

    public synchronized void chce_pisac() {
        // 1. prosty protokol wejscia
        czekajacy_pisarze++;
        while (liczba_czyt + liczba_pisz > 0) {
            try {
                wait(); // czeka na pisarzy
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
        czekajacy_pisarze--;
        liczba_pisz++;
    }

    public synchronized void koniec_pisania() {
        // 3. protokol wyjscia - uczciwy wzgledem czytelnikow
        liczba_pisz--;
        notifyAll(); // budzi wszystkich (czytelnicy i pisarze)
    }

    public synchronized void chce_czytac() {
        // 4. protokol wejscia: bezpieczny i uczciwy wzgledem pisarzy
        // JEZELI( liczba_pisz > 0 lub ~empty( pisarze ) ) wait( czytelnicy );
        while (liczba_pisz > 0 || czekajacy_pisarze > 0) {
            try {
                wait();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
        liczba_czyt++;

        // 6. czytelnicy czekajacy na wejscie budza sie kolejno nawzajem
        notifyAll();
    }

    public synchronized void koniec_czytania() {
        // 5. protokol wyjscia - budzi tylko pisarzy
        liczba_czyt--;
        if (liczba_czyt == 0) {
            notifyAll();
        }
    }

    public synchronized void wyswietl_stan() {
        System.out.println("  [Stan: czytelnicy=" + liczba_czyt +
                ", pisarze=" + liczba_pisz +
                ", czekajacy pisarze=" + czekajacy_pisarze + "]");
    }
}

class Czytelnik extends Thread {
    private Czytelnia czytelnia;
    private int id;

    public Czytelnik(Czytelnia c, int id) {
        this.czytelnia = c;
        this.id = id;
    }

    public void run() {
        for (int i = 0; i < 3; i++) {
            try {
                System.out.println("Czytelnik " + id + ": chce czytac");
                czytelnia.chce_czytac();

                System.out.println("Czytelnik " + id + ": CZYTAM (iteracja " + (i + 1) + ")");
                czytelnia.wyswietl_stan();
                Thread.sleep(1000);

                czytelnia.koniec_czytania();
                System.out.println("Czytelnik " + id + ": koniec czytania");

                Thread.sleep((long) (Math.random() * 1000));
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}

class Pisarz extends Thread {
    private Czytelnia czytelnia;
    private int id;

    public Pisarz(Czytelnia c, int id) {
        this.czytelnia = c;
        this.id = id;
    }

    public void run() {
        for (int i = 0; i < 3; i++) {
            try {
                System.out.println("Pisarz " + id + ": chce pisac");
                czytelnia.chce_pisac();

                System.out.println("Pisarz " + id + ": PISZE (iteracja " + (i + 1) + ")");
                czytelnia.wyswietl_stan();
                Thread.sleep(2000);

                czytelnia.koniec_pisania();
                System.out.println("Pisarz " + id + ": koniec pisania");

                Thread.sleep((long) (Math.random() * 1000));
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}

public class Zadanie5Czytelnia {
    public static void main(String[] args) {
        final int LICZBA_CZYTELNIKOW = 5;
        final int LICZBA_PISARZY = 2;

        System.out.println("=== Czytelnia w Javie (monitory) ===");
        System.out.println("Wyklad 6, slajdy 10-12");
        System.out.println("Czytelnikow: " + LICZBA_CZYTELNIKOW + ", Pisarzy: " + LICZBA_PISARZY);
        System.out.println();

        Czytelnia czytelnia = new Czytelnia();

        // Tworzenie i uruchamianie czytelników
        Czytelnik[] czytelnicy = new Czytelnik[LICZBA_CZYTELNIKOW];
        for (int i = 0; i < LICZBA_CZYTELNIKOW; i++) {
            czytelnicy[i] = new Czytelnik(czytelnia, i + 1);
            czytelnicy[i].start();
        }

        // Tworzenie i uruchamianie pisarzy
        Pisarz[] pisarze = new Pisarz[LICZBA_PISARZY];
        for (int i = 0; i < LICZBA_PISARZY; i++) {
            pisarze[i] = new Pisarz(czytelnia, i + 1);
            pisarze[i].start();
        }

        // Oczekiwanie na zakończenie wszystkich wątków
        try {
            for (int i = 0; i < LICZBA_CZYTELNIKOW; i++) {
                czytelnicy[i].join();
            }
            for (int i = 0; i < LICZBA_PISARZY; i++) {
                pisarze[i].join();
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }

        System.out.println();
        System.out.println("=== Wszystkie watki zakonczone ===");
    }
}
