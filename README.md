# monte carlo coverage

this was assignment 1 for cis3090 (parallel programming) at guelph, fall 2026. you give it a rectangular field and a bunch of circles, and it estimates how much of the field the circles cover. it does that by throwing a lot of random points at the field and counting how many land inside a circle.

the assignment was to write two versions, a normal serial one and a parallel one using pthreads, and then timed them against each other.

## how it works

- read the field size and the list of circles (x, y, radius) from a file
- pick n random points inside the field
- a point counts as a hit if it's inside at least one circle. i compare squared distances so i don't need sqrt
- coverage % = hits / n × 100, and covered area = that fraction × width × height

if a circle goes past the edge of the field that's fine, since points only ever get picked inside the field anyway.

## the parallel part

the n points get split between p threads. if n doesn't divide evenly, the leftover points go to some of the threads so the total still comes out to exactly n.

each thread gets its own seed (baseseed + thread number) and uses `rand_r()`, so no two threads share random number state. each thread also counts its hits in its own local variable and writes the total into its own spot in an array. main adds everything up after joining the threads. since threads never write to the same memory, i didn't need a mutex, and valgrind and drd both came back clean (no leaks, no races).

## files

- `coverageSerial.c`: serial version
- `coverageParallel.c`: pthreads version
- `Makefile`
- `batchscript.sh`: runs all the experiments and saves them to `results.csv`
- `results.csv`: data from every run
- `sample1.txt`, `sample2.txt`, `sample3.txt`: test inputs

## running it

```bash
make
./coverageSerial sample1.txt 1000000 1000
./coverageParallel sample1.txt 1000000 4 1000
```

the arguments are `<file> <N> <baseSeed>` for serial and `<file> <N> <threads> <baseSeed>` for parallel.

it prints this to stdout:

```
SAMPLES: <N>
HITS: <hits>
COVERAGE: <area> <percent>
```

the time goes to stderr as `TIME: <seconds>`. it only measures the sampling part, not loading the file.

if you give it bad input (a missing file, a messed up file, letters instead of numbers, n or p ≤ 0), it prints an error and exits.

input files look like this:

```
<width> <height>
<number of circles>
<x> <y> <r>
...
```

## experiments

`batchscript.sh` runs both versions on `sample1.txt` with n = 10⁴, 10⁶, 10⁸ and 1, 2, 4, 8 threads, 36 runs each (run k uses seed 1000·k). everything ends up in `results.csv`, which i used to look at accuracy, speedup and how much the timing jumped around between runs.

## checking for leaks and races

```bash
valgrind --leak-check=full ./coverageParallel sample1.txt 100000 4 1000
valgrind --tool=drd ./coverageParallel sample1.txt 100000 8 1000
```
