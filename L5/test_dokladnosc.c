#include<stdio.h>
#include<math.h>
#include"pomiar_czasu.h"

#ifndef M_PI // standardy C99 i C11 nie wymagają definiowania stałej M_PI
#define M_PI (3.14159265358979323846)
#endif


double funkcja ( double x );

double funkcja ( double x ){ return( sin(x) ); }

double calka_sekw(double a, double b, double dx);

int main( int argc, char *argv[] ){

  double a, b, dx, calka;
  double t1;
  
  // Wartość dokładna całki z sin(x) od 0 do π to 2.0
  double wartosc_dokladna = 2.0;
  
  a = 0.0;
  b = M_PI;

  // Tablica wartości dx do przetestowania
  double dx_values[] = {0.1, 0.01, 0.0001, 0.00001, 0.0000001};
  int num_tests = 5;
  
  // Format CSV: dx, błąd bezwzględny
  printf("dx,blad_bezwzgledny\n");

  int i;
  for(i = 0; i < num_tests; i++){
    dx = dx_values[i];
    
    t1 = czas_zegara();
    calka = calka_sekw(a, b, dx);
    t1 = czas_zegara() - t1;
    
    double blad = fabs(calka - wartosc_dokladna);
    
    printf("%.10lf,%.15lf\n", dx, blad);
  }

  return 0;
}

double calka_sekw(double a, double b, double dx){

  int N = ceil((b-a)/dx);
  double dx_adjust = (b-a)/N;

  //printf("Obliczona liczba trapezów: N = %d, dx_adjust = %lf\n", N, dx_adjust);
  
  int i;
  double calka = 0.0;
  for(i=0; i<N; i++){

    double x1 = a + i*dx_adjust;
    calka += 0.5*dx_adjust*(funkcja(x1)+funkcja(x1+dx_adjust));

  }

  return(calka);
}
