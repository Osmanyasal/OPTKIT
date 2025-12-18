set terminal pngcairo size 900,500 noenhanced
set output 'exec_time_per_core.png'
set title 'Execution time per core'
set xlabel 'Cores used'
set ylabel 'Duration (ms)'
set grid
set xtics ('0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0, '0' 0)
set xrange [0:1]
plot 'exec_time_per_core.dat' using 1:2 with linespoints lw 2 title 'Duration'
