package org.example;

public class CalkaRunnable implements Runnable {
    
    private double a; // lower bound for this segment
    private double b; // upper bound for this segment
    private double dx;
    private double result;
    
    public CalkaRunnable(double a, double b, double dx) {
        this.a = a;
        this.b = b;
        this.dx = dx;
        this.result = 0.0;
    }
    
    @Override
    public void run() {
        result = 0.0;
        int n = (int) Math.ceil((b - a) / dx);
        
        for (int i = 0; i < n; i++) {
            double x = a + i * dx;
            result += function(x) * dx;
        }
    }
    
    // Example function: f(x) = x^2
    private double function(double x) {
        return x * x;
    }
    
    public double getResult() {
        return result;
    }
}

