package org.example;

import java.util.Arrays;
import java.util.concurrent.ForkJoinPool;

public class MergeSortDemo {

    public static int[] sortWithForkJoin(int[] array) {
        ForkJoinPool pool = new ForkJoinPool();
        int[] result = pool.invoke(new DivideTask(array));
        pool.shutdown();
        return result;
    }

    public static void demonstrateSorting(int[] array) {
        System.out.println("Original array: " + Arrays.toString(array));

        long startTime = System.currentTimeMillis();
        int[] sorted = sortWithForkJoin(array);
        long endTime = System.currentTimeMillis();

        System.out.println("Sorted array:   " + Arrays.toString(sorted));
        System.out.println("Time taken: " + (endTime - startTime) + " ms");
    }
}

