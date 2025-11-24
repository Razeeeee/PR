package org.example;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

public class HistogramParallel {

    private int[] data;
    private int numBins;
    private int numThreads;
    private int minValue;
    private int maxValue;

    public HistogramParallel(int[] data, int numBins, int numThreads) {
        this.data = data;
        this.numBins = numBins;
        this.numThreads = numThreads;

        // Find min and max values
        this.minValue = data[0];
        this.maxValue = data[0];
        for (int value : data) {
            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    public int[] calculate() throws InterruptedException {
        ExecutorService executor = Executors.newFixedThreadPool(numThreads);
        List<Future<int[]>> futures = new ArrayList<>();

        int segmentSize = data.length / numThreads;

        for (int i = 0; i < numThreads; i++) {
            int startIndex = i * segmentSize;
            int endIndex = (i == numThreads - 1) ? data.length : (i + 1) * segmentSize;

          HistogramCallable task = new HistogramCallable(
                data, startIndex, endIndex, numBins, minValue, maxValue
            );

            Future<int[]> future = executor.submit(task);
            futures.add(future);
        }

        // Combine local histograms into a global histogram
        int[] globalHistogram = new int[numBins];
        for (Future<int[]> future : futures) {
            try {
                int[] localHistogram = future.get();
                for (int j = 0; j < numBins; j++) {
                    globalHistogram[j] += localHistogram[j];
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return globalHistogram;
    }

    public void printHistogram(int[] histogram) {
        System.out.println("\nHistogram (Min: " + minValue + ", Max: " + maxValue + "):");
        int binWidth = (maxValue - minValue + 1) / numBins;
        for (int i = 0; i < histogram.length; i++) {
            int rangeStart = minValue + i * binWidth;
            int rangeEnd = (i == numBins - 1) ? maxValue : rangeStart + binWidth - 1;
            System.out.printf("[%4d - %4d]: %s (%d)\n",
                rangeStart, rangeEnd, "*".repeat(Math.min(histogram[i], 50)), histogram[i]);
        }
    }
}

