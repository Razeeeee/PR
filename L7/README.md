### 3.0
- **CalkaSequential.java** - wariant sekwencyjny z dx przekazanym do konstruktora
- **CalkaCallable.java** - implementacja Callable dla równoległego obliczania segmentu całki
- **CalkaParallel.java** - równoległe obliczanie z ExecutorService, zwracanie wyników przez Future

### 4.0
- **MergeSortDemo.java** - demonstracja sortowania z wykorzystaniem ForkJoinPool
- **scal_tab.java** - implementacja DivideTask z warunkiem stopu

### 4.5
- **CalkaRunnable.java** - implementacja Runnable z przekazywaniem wyników przez pole obiektu
- **CalkaRunnableParallel.java** - zarządzanie pulą wątków z Runnable

### 5.0
- **HistogramRunnable.java** - lokalne obliczanie histogramu przez wątek
- **HistogramParallel.java** - równoległe obliczanie histogramu z pulą wątków

````
mvn clean compile
mvn exec:java
````

### 3.0, 4.5
- a = 0
- b = 10
- dx = 0.0001
- thread count = 4

### 4.0
- table size = 10000
- value range = 1000

### 5.0
- thread count = 4
- a = 100
- b = 100

### ExecutorService
Używany w CalkaParallel.java do zarządzania pulą wątków:
```java
ExecutorService executor = Executors.newFixedThreadPool(numThreads);
```

### Callable i Future
Pozwala na pobieranie wyników z zadań asynchronicznych:
```java
Future<Double> future = executor.submit(task);
double result = future.get(); // Blokuje do otrzymania wyniku
```

### Runnable z przekazywaniem wyników
Alternatywne podejście bez Future:
```java
CalkaRunnable task = new CalkaRunnable(a, b, dx);
executor.execute(task);
executor.awaitTermination(1, TimeUnit.MINUTES);
double result = task.getResult();
```

### ForkJoinPool
```java
ForkJoinPool pool = new ForkJoinPool();
int[] result = pool.invoke(new DivideTask(array));
```
