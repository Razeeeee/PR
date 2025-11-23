package org.example;

public class CalkaSequential {

    private double dx;
    private double a; // lower bound
    private double b; // upper bound

    public CalkaSequential(double a, double b, double dx) {
        this.a = a;
        this.b = b;
        this.dx = dx;
    }

    public double calculate() {
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

