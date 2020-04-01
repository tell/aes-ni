#!/usr/bin/bash

echo "Current directory = $(pwd)"
bench_marks=$(find . -maxdepth 1 -iname "bench_*" -not -iname "bench_shuffle_*" -perm -100 | sort)
echo "Current benchmarks:"
echo "${bench_marks}"
echo "Running..."

for bench in ${bench_marks}; do
    echo "Execute: ${bench}"
    ${bench} > ./${bench}_result.csv
done
