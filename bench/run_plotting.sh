#!/usr/bin/bash

echo "Current directory = $(pwd)"
bench_marks=$(find . -maxdepth 1 -iname "bench_*_result.csv" -not -iname "bench_shuffle_*" | sort)
echo "Current benchmark results:"
echo "${bench_marks}"

plotfile="autogen_plotting.plot"
rm -f ${plotfile}
touch ${plotfile}

echo "plot \\" > ${plotfile}

for bench in ${bench_marks}; do
    plot_title=$(echo ${bench} | sed -e 's/\.\/bench_//' | sed -e 's/_result\.csv//')
    echo ${plot_title}
    echo "\"${bench}\" using (column(2)):(valid(4) ? column(4) : NaN) with linespoints title \"${plot_title}\", \\" >> ${plotfile}
done

gnuplot benchmark.plot
