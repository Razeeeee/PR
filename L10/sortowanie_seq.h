/*************************************************************************
Plik sort.h, naglowki procedur sortowania z pliku sortowanie.c
*************************************************************************/
#ifndef _sort_
#define _sort_


/*---------------------------------------------------------
  sortowanie_przez_wstawianie - sortowanie tablicy n-elementowej,
    o wyrazach od A[1] do A[n] (wymaga alokacji tablicy rozmiaru n+1)
----------------------------------------------------------*/
extern void sortowanie_przez_wstawianie(
  double* A,    /* tablica do posortowania */
  int p,        /* in: poczatkowy indeks tablicy */
  int k         /* in: koncowy indeks tablicy */
  );

/*---------------------------------------------------------
  sortowanie_przez_scalanie(A,p,r) - procedura sortuje czesc tablicy A
                                     od indeksu p do indeksu q 
----------------------------------------------------------*/
extern void sortowanie_przez_scalanie(
  double* A,    /* tablica fragment ktorej ma zostac posortowany */
  int p,        /* poczatkowy indeks fragmentu do posortowania */
  int r         /* koncowy indeks fragmentu do posortowania */
  );

/*---------------------------------------------------------
  sortowanie_szybkie - procedura sortuje czesc tablicy A
                       od indeksu p do indeksu q
---------------------------------------------------------*/
extern void sortowanie_szybkie(
  double* A,    /* tablica fragment ktorej ma zostac posortowany */
  int p,        /* poczatkowy indeks fragmentu do posortowania */
  int r         /* koncowy indeks fragmentu do posortowania */
  );


/*** Deklaracje pomocniczych procedur wewnetrznych ***/

/*---------------------------------------------------------
  scal - procedura scala dwa posortowane fragmenty tablicy
         w posortowana calosc
----------------------------------------------------------*/
void scal(
  double* A,    /* in: tablica, dwa fragmenty ktorej maja zostac scalone */
  int p,        /* in: poczatkowy indeks pierwszego fragmentu do scalenia */
  int q,        /* in: koncowy indeks pierwszego fragmentu do scalenia */
  int r         /* in: koncowy indeks drugiego fragmentu do scalenia */
  );

/*---------------------------------------------------------
  podziel - procedura dzieli tablice A[p..k] na dwie podtablice 
            A[p..q] i A[q+1..k] tak, zeby kazdy element lewej podtablicy
            byl mniejszy lub rowny dowolnemu elementowi prawej podtablicy
---------------------------------------------------------*/
int podziel(    /* out: procedura zwraca indeks q dzielacy tablice */
  double* A,    /* in: tablica fragment ktorej ma zostac podzielony */
  int p,        /* in: poczatkowy indeks fragmentu do podzielenia */
  int k         /* in: koncowy indeks fragmentu do podzielenia */
  );


#endif

/*************************************************************************
koniec sort.h
*************************************************************************/
