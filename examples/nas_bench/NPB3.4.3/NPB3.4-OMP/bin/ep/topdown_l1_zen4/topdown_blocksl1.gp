set terminal pngcairo size 1100,600 noenhanced
set output 'topdown_blocksl1.png'
set title 'Topdown metrics per execution'
set style data histograms
set style histogram rowstacked
set style fill solid border -1
set boxwidth 0.8
set key outside right
set xlabel 'Core Count'
set ylabel '%'
set yrange [0:100]
set xtics rotate by -30
plot 'topdown_blocksl1.dat' using 2:xtic(1) title 'frontend_bound', '' using 3 title 'bad_speculation', '' using 4 title 'Retiring', '' using 5 title 'backend_bound', '' using 6 title 'smt_contention', '' using 0:($2/2):($2 > 3 ? sprintf('%.1f%%',$2) : '') with labels tc rgb 'black' font ',9' notitle, '' using 0:($2+$3/2):($3 > 3 ? sprintf('%.1f%%',$3) : '') with labels tc rgb 'black' font ',9' notitle, '' using 0:($2+$3+$4/2):($4 > 3 ? sprintf('%.1f%%',$4) : '') with labels tc rgb 'black' font ',9' notitle, '' using 0:($2+$3+$4+$5/2):($5 > 3 ? sprintf('%.1f%%',$5) : '') with labels tc rgb 'black' font ',9' notitle, '' using 0:($2+$3+$4+$5+$6/2):($6 > 3 ? sprintf('%.1f%%',$6) : '') with labels tc rgb 'black' font ',9' notitle
