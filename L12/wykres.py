#!/usr/bin/env python3
import pandas as pd # type: ignore
import matplotlib.pyplot as plt # type: ignore
import os

def main():
    for plik, col_p, nazwa in [('wyniki_omp.csv', 'watki', 'OpenMP'), 
                                ('wyniki_mpi.csv', 'procesy', 'MPI')]:
        if not os.path.exists(plik):
            print(f"Brak {plik}")
            continue
        
        df = pd.read_csv(plik).sort_values(col_p)
        t1 = df[df[col_p] == 1]['czas'].values[0]
        df['S'] = t1 / df['czas']
        df['E'] = df['S'] / df[col_p]
        
        print(f"\n{nazwa}: p  czas      S(p)    E(p)")
        for _, r in df.iterrows():
            print(f"       {int(r[col_p]):2d}  {r['czas']:.4f}  {r['S']:.3f}  {r['E']*100:.1f}%")
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))
        p = df[col_p].values
        
        ax1.plot(p, df['S'], 'bo-', label='S(p)')
        ax1.plot(p, p, 'r--', label='idealne')
        ax1.set_xlabel('p'); ax1.set_ylabel('S(p)'); ax1.legend(); ax1.grid(True)
        ax1.set_title(f'{nazwa} - Przyspieszenie')
        
        ax2.bar(p, df['E']*100, color='g', alpha=0.7)
        ax2.axhline(100, color='r', linestyle='--')
        ax2.set_xlabel('p'); ax2.set_ylabel('E(p) [%]'); ax2.set_ylim(0, 120)
        ax2.set_title(f'{nazwa} - Efektywnosc')
        
        plt.tight_layout()
        plt.savefig(f'wykres_{nazwa.lower()}.png', dpi=150)
        plt.close()
        print(f"Wykres: wykres_{nazwa.lower()}.png")

if __name__ == '__main__':
    main()
