package org.example;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class CalkaRunnableParallel {
    
    private double a; // lower bound
    private double b; // upper bound
    private double dx;
    private int numThreads;
    
    public CalkaRunnableParallel(double a, double b, double dx, int numThreads) {
        this.a = a;
        this.b = b;
        this.dx = dx;
        this.numThreads = numThreads;
    }
    
    public double calculate() throws InterruptedException {
        ExecutorService executor = Executors.newFixedThreadPool(numThreads);
        List<CalkaRunnable> tasks = new ArrayList<>();
        
        // Divide the integration range into segments
        double segmentSize = (b - a) / numThreads;
        
        for (int i = 0; i < numThreads; i++) {
            double segmentA = a + i * segmentSize;
            double segmentB = (i == numThreads - 1) ? b : a + (i + 1) * segmentSize;
            
            CalkaRunnable task = new CalkaRunnable(segmentA, segmentB, dx);
            tasks.add(task);
            executor.execute(task);
        }
        
        executor.shutdown();
        executor.awaitTermination(1, TimeUnit.MINUTES);
        
        // Collect results from all threads
        double totalResult = 0.0;
        for (CalkaRunnable task : tasks) {
            totalResult += task.getResult();
        }
        
        return totalResult;
    }
}

