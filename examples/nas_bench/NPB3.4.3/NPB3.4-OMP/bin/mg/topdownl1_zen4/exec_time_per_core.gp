set terminal pngcairo size 900,500 noenhanced
set output 'exec_time_per_core.png'
set title 'Execution time per core'
set xlabel 'Cores used'
set ylabel 'Duration (ms)'
set grid
set xtics ('1' 1, '1' 1, '1' 1, '2' 2, '2' 2, '2' 2, '4' 4, '4' 4, '4' 4, '8' 8, '8' 8, '8' 8, '16' 16, '16' 16, '16' 16, '32' 32, '32' 32, '32' 32)
set xrange [0:35.2]
plot 'exec_time_per_core.dat' using 1:2 with linespoints lw 2 title 'Duration'
