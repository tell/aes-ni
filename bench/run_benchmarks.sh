#!/usr/bin/bash

echo "Current directory = $(pwd)"
bench_marks=$(ls -1 bench_*)
echo "Current benchmarks:"
echo "${bench_marks}"
echo "Running..."
for bench in ${bench_marks}; do
    echo "Execute: ${bench}"
    ./${bench} > ./${bench}_result.txt
done
