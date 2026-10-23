#!/bin/bash

echo "program,N,p,seed,hits,percent,time" > results.csv

for N in 10000 1000000 100000000
do
    # 36 runs for serial
    for k in {1..36}
    do
        seed=$((1000 * k))
        echo "serial N=$N run $k"
        ./coverageSerial ../sample1.txt $N $seed > out.txt 2>&1

        hits=$(awk '/HITS/ {print $2}' out.txt)
        percent=$(awk '/COVERAGE/ {print $3}' out.txt)
        time=$(awk '/TIME/ {print $2}' out.txt)
        echo "serial,$N,1,$seed,$hits,$percent,$time" >> results.csv
    done

    # 36 runs for each # threads for parallel
    for p in 1 2 4 8
    do
        for k in {1..36}
        do
            seed=$((1000 * k))
            echo "parallel N=$N p=$p run $k"
            ./coverageParallel ../sample1.txt $N $p $seed > out.txt 2>&1

            hits=$(awk '/HITS/ {print $2}' out.txt)
            percent=$(awk '/COVERAGE/ {print $3}' out.txt)
            time=$(awk '/TIME/ {print $2}' out.txt)
            echo "parallel,$N,$p,$seed,$hits,$percent,$time" >> results.csv
        done
    done
done

rm out.txt
echo "done!"