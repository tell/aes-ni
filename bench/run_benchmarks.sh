#!/usr/bin/bash

echo "Current directory = $(pwd)"
bench_marks=$(find . -maxdepth 1 -iname "bench_*" -not -iname "bench_shuffle_*" -perm -100)
echo "Current benchmarks:"
echo "${bench_marks}"
echo "Running..."

plotfile="autogen_plotting.plot"
rm -f ${plotfile}
touch ${plotfile}

echo "plot \\" > ${plotfile}

for bench in ${bench_marks}; do
    echo "Execute: ${bench}"
    ${bench} > ./${bench}_result.csv
    echo "\"${bench}_result.csv\" using (column(2)):(valid(4) ? column(4) : NaN) with linespoints title \"${bench}\" noenhanced, \\" >> ${plotfile}
done

gnuplot benchmark.plot
