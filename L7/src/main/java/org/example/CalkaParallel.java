package org.example;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class CalkaParallel {
    
    private double a; // lower bound
    private double b; // upper bound
    private double dx;
    private int numThreads;
    
    public CalkaParallel(double a, double b, double dx, int numThreads) {
        this.a = a;
        this.b = b;
        this.dx = dx;
        this.numThreads = numThreads;
    }
    
    public double calculate() throws InterruptedException, ExecutionException {
        ExecutorService executor = Executors.newFixedThreadPool(numThreads);
        List<Future<Double>> futures = new ArrayList<>();
        
        // Divide the integration range into segments
        double segmentSize = (b - a) / numThreads;
        
        for (int i = 0; i < numThreads; i++) {
            double segmentA = a + i * segmentSize;
            double segmentB = (i == numThreads - 1) ? b : a + (i + 1) * segmentSize;
            
            CalkaCallable task = new CalkaCallable(segmentA, segmentB, dx);
            Future<Double> future = executor.submit(task);
            futures.add(future);
        }
        
        // Collect results from all threads
        double totalResult = 0.0;
        for (Future<Double> future : futures) {
            totalResult += future.get(); // This blocks until the result is available
        }
        
        executor.shutdown();
        
        return totalResult;
    }
}

