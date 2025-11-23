package org.example;

public class Watek2_Runnable implements Runnable {
    private int startCharIndex;
    private int endCharIndex;
    private Obraz image;

    public Watek2_Runnable(int startCharIndex, int endCharIndex, Obraz image) {
        this.startCharIndex = startCharIndex;
        this.endCharIndex = endCharIndex;
        this.image = image;
    }

    @Override
    public void run() {
        image.calculate_histogram_block_chars(startCharIndex, endCharIndex);
    }
}
