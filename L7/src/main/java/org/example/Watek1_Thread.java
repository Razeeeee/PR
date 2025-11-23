package org.example;

public class Watek1_Thread extends Thread {
    private int charIndex;
    private Obraz image;

    public Watek1_Thread(int charIndex, Obraz image) {
        this.charIndex = charIndex;
        this.image = image;
    }

    @Override
    public void run() {
        image.calculate_histogram_for_char(charIndex);
    }
}

