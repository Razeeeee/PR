// Zadanie 2: Calka OpenMP - pomiary wydajnosci
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

double f(double x) { return x * x * x; }

int main(int argc, char *argv[]) {
    double a = 0.0, b = 1.0;
    int N = 100000000;
    double dx = (b - a) / N;
    int nt = (argc > 1) ? atoi(argv[1]) : 1;
    
    // omp_set_num_threads(n): void - ustawia liczbe watkow
    omp_set_num_threads(nt);
    // omp_get_wtime(): double - aktualny czas w sekundach
    double t1 = omp_get_wtime();
    
    double calka = 0.0;
    // #pragma omp parallel for reduction(+:var) - rownolegle for z redukcja
    #pragma omp parallel for reduction(+:calka)
    for (int i = 0; i < N; i++) {
        double x = a + i * dx;
        calka += 0.5 * dx * (f(x) + f(x + dx));
    }
    
    double czas = omp_get_wtime() - t1;
    printf("OMP: watki=%d, czas=%.6lf, wynik=%.15lf\n", nt, czas, calka);
    
    FILE *fp = fopen("wyniki_omp.csv", "a");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) == 0) fprintf(fp, "watki,czas,wynik\n");
        fprintf(fp, "%d,%.9lf,%.15lf\n", nt, czas, calka);
        fclose(fp);
    }
    return 0;
}
