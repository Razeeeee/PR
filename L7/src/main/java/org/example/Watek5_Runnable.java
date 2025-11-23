package org.example;

public class Watek5_Runnable implements Runnable {
    private int startRow;
    private int endRow;
    private int startColumn;
    private int endColumn;
    private Obraz image;

    public Watek5_Runnable(int startRow, int endRow, int startColumn, int endColumn, Obraz image) {
        this.startRow = startRow;
        this.endRow = endRow;
        this.startColumn = startColumn;
        this.endColumn = endColumn;
        this.image = image;
    }

    @Override
    public void run() {
        image.calculate_histogram_2D_block(startRow, endRow, startColumn, endColumn);
    }
}
