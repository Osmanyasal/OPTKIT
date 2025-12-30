set terminal pngcairo size 1400,420 noenhanced font 'Arial,11'
set output 'heatmap_energy_pkg.png'
set title 'energy-pkg vs Core/Uncore Frequency'
set xlabel 'Core Frequency (GHz)'
set ylabel 'Uncore Frequency (GHz)'
set cblabel 'energy_pkg' offset 2,0
set key off
set grid xtics
set palette rgbformulae 22,13,-31
set xtics ('0.545' 0.545, '0.683' 0.683, '0.883' 0.883, '1.083' 1.083, '1.283' 1.283, '1.483' 1.483, '1.683' 1.683, '1.883' 1.883, '2.083' 2.083, '2.283' 2.283, '2.483' 2.483, '2.683' 2.683, '2.883' 2.883, '3.083' 3.083, '3.283' 3.283, '3.483' 3.483, '3.683' 3.683, '3.883' 3.883, '4.083' 4.083, '4.283' 4.283, '4.483' 4.483, '4.683' 4.683, '4.883' 4.883, '5.083' 5.083, '5.283' 5.283, '5.483' 5.483, '5.683' 5.683, '5.883' 5.883) rotate by -45
set ytics ('0.000' 0.000)
set yrange [-0.108:0.108]
unset cbrange
set autoscale cbfix
w = 0.065550
h = 0.090000
plot 'heatmap_energy_pkg.dat' using 1:2:($1-w):($1+w):($2-h):($2+h):3 with boxxyerrorbars palette fs solid 1.0 border lc rgb 'black' notitle, \
     'heatmap_energy_pkg_min.dat' using 1:2 with points pt 7 ps 2.5 lc rgb 'white' lw 3 notitle, \
     'heatmap_energy_pkg_min.dat' using 1:2 with points pt 6 ps 2.0 lc rgb 'black' lw 2 notitle, \
     'heatmap_energy_pkg_min.dat' using 1:2:(sprintf('MIN=%.3f', $3)) with labels offset 0,1.5 tc rgb 'white' font ',12' notitle
