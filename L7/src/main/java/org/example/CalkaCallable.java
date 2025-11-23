package org.example;

import java.util.concurrent.Callable;

public class CalkaCallable implements Callable<Double> {
    
    private double a; // lower bound for this segment
    private double b; // upper bound for this segment
    private double dx;
    
    public CalkaCallable(double a, double b, double dx) {
        this.a = a;
        this.b = b;
        this.dx = dx;
    }
    
    @Override
    public Double call() throws Exception {
        double result = 0.0;
        int n = (int) Math.ceil((b - a) / dx);
        
        for (int i = 0; i < n; i++) {
            double x = a + i * dx;
            result += function(x) * dx;
        }
        
        return result;
    }
    
    // Example function: f(x) = x^2
    private double function(double x) {
        return x * x;
    }
}

