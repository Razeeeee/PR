package org.example;

import java.util.concurrent.Callable;

public class HistogramCallable implements Callable<int[]> {

    private int[] data;
    private int startIndex;
    private int endIndex;
    private int[] localHistogram;
    private int numBins;
    private int minValue;
    private int maxValue;

    public HistogramCallable(int[] data, int startIndex, int endIndex,
                            int numBins, int minValue, int maxValue) {
        this.data = data;
        this.startIndex = startIndex;
        this.endIndex = endIndex;
        this.numBins = numBins;
        this.minValue = minValue;
        this.maxValue = maxValue;
        this.localHistogram = new int[numBins];
    }

    @Override
    public int[] call() {
        for (int i = startIndex; i < endIndex; i++) {
            int value = data[i];
            if (value >= minValue && value <= maxValue) {
                int bin = (int) (((double)(value - minValue) / (maxValue - minValue + 1)) * numBins);
                if (bin >= numBins) bin = numBins - 1;
                localHistogram[bin]++;
            }
        }

        return localHistogram;
    }
}

