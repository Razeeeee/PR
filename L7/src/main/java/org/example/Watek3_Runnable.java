package org.example;

public class Watek3_Runnable implements Runnable {
    private int threadId;
    private int numberOfThreads;
    private Obraz image;

    public Watek3_Runnable(int threadId, int numberOfThreads, Obraz image) {
        this.threadId = threadId;
        this.numberOfThreads = numberOfThreads;
        this.image = image;
    }

    @Override
    public void run() {
        image.calculate_histogram_cyclic_rows(threadId, numberOfThreads);
    }
}

