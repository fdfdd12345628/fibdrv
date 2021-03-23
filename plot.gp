set title "perf fib"
set xlabel "Number"
set ylabel "ns"
set terminal png font " Times_New_Roman,12 "
set output "statistic.png"
set xtics 0 ,10 ,100
set key left 
set datafile separator ","

plot \
"output.csv" using 1:2 with linespoints linewidth 2 title "kernel", \
"output.csv" using 1:3 with linespoints linewidth 2 title "user", \
"output.csv" using 1:4 with linespoints linewidth 2 title "diff", \