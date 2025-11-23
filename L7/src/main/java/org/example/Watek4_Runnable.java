package org.example;

public class Watek4_Runnable implements Runnable {
    private int startColumn;
    private int endColumn;
    private Obraz image;

    public Watek4_Runnable(int startColumn, int endColumn, Obraz image) {
        this.startColumn = startColumn;
        this.endColumn = endColumn;
        this.image = image;
    }

    @Override
    public void run() {
        image.calculate_histogram_block_columns(startColumn, endColumn);
    }
}
