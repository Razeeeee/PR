// java
package org.example;

import java.util.Arrays;
import java.util.concurrent.RecursiveTask;

class DivideTask extends RecursiveTask<int[]> {

    int[] arrayToDivide;

    public DivideTask(int[] arrayToDivide) {
        this.arrayToDivide = arrayToDivide;
    }

    protected int[] compute() {

        if (arrayToDivide == null || arrayToDivide.length <= 1) {
            return arrayToDivide;
        }

        int mid = arrayToDivide.length / 2;
        int[] left = Arrays.copyOfRange(arrayToDivide, 0, mid);
        int[] right = Arrays.copyOfRange(arrayToDivide, mid, arrayToDivide.length);

        DivideTask task1 = new DivideTask(left);
        DivideTask task2 = new DivideTask(right);

        // ForkJoinTask.invokeAll will fork both tasks and wait for their completion
        java.util.concurrent.ForkJoinTask.invokeAll(task1, task2);
        int[] tab1 = task1.join();
        int[] tab2 = task2.join();

        int[] scal_tab = new int[arrayToDivide.length];

        scal_tab(tab1, tab2, scal_tab);

        return scal_tab;
    }

    private void scal_tab(
            int[] tab1,
            int[] tab2,
            int[] scal_tab) {

        int i = 0, j = 0, k = 0;

        while ((i < tab1.length) && (j < tab2.length)) {

            if (tab1[i] < tab2[j]) {
                scal_tab[k] = tab1[i++];
            } else {
                scal_tab[k] = tab2[j++];
            }

            k++;
        }

        if (i == tab1.length) {

            for (int a = j; a < tab2.length; a++) {
                scal_tab[k++] = tab2[a];
            }

        } else {

            for (int a = i; a < tab1.length; a++) {
                scal_tab[k++] = tab1[a];
            }

        }
    }

}