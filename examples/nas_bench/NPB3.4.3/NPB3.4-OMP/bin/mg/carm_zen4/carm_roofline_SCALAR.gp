# Cache-Aware Roofline Model (CARM) - SCALAR
set terminal pngcairo size 1000,640 enhanced font 'Arial,12'
set output 'carm_roofline_SCALAR.png'

set title "CARM - SCALAR ISA"
set xlabel "Arithmetic Intensity [FLOPs/Byte]"
set ylabel "Performance [GFLOPs/s]"
set logscale xy
set grid

# Margins and layout
set tmargin 3
set bmargin 3
set lmargin 8
set rmargin 4
set key left top spacing 1.2 box samplen 3 width 0
set samples 400

# Hardware parameters from CARM measurements
compute_peak = 22  # GFLOPs/s
l1_bw = 129         # GB/s
l2_bw = 118         # GB/s
l3_bw = 51         # GB/s
mem_bw = 29       # GB/s

# Roofline functions
roof_bw(bw, x) = (bw * x <= compute_peak) ? (bw * x) : compute_peak
compute_line(x) = compute_peak

# AI knees (where bandwidth roof meets compute roof)
ai_knee_l1  = 0.170543
ai_knee_l2  = 0.186441
ai_knee_l3  = 0.431373
ai_knee_mem = 0.758621

format_knee(x) = sprintf("AI=%.3g", x)

set xrange [1e-3:5e1]
set yrange [0.1:compute_peak*6]

# Plot rooflines and measured data
plot \
    roof_bw(l1_bw, x)  w l lw 2 lc rgb "#00BB00" title sprintf("L1 (%.0f GB/s)", l1_bw), \
    roof_bw(l2_bw, x)  w l lw 2 lc rgb "#0000FF" title sprintf("L2 (%.0f GB/s)", l2_bw), \
    roof_bw(l3_bw, x)  w l lw 2 lc rgb "#9900CC" title sprintf("L3 (%.0f GB/s)", l3_bw), \
    roof_bw(mem_bw, x) w l lw 2 lc rgb "#FF9900" title sprintf("DRAM (%.0f GB/s)", mem_bw), \
    'roofline.dat' using 1:2 w p pt 7 ps 2 lw 2 lc rgb "#FF0000" title "Measured", \
    'roofline.dat' using 1:2:(sprintf('Cores=%d, AI=%.3f, GFlops=%.2f', column(3), column(4), column(5))) with labels offset 0,-1.0 tc rgb "#000000" font ',9' notitle

# Knee markers and labels
set arrow from ai_knee_l1, graph 0 to ai_knee_l1, compute_peak nohead dt 3 lc rgb "#00BB00"
set label 1 format_knee(ai_knee_l1) at ai_knee_l1, compute_peak*0.45 center tc rgb "#00BB00"

set arrow from ai_knee_l2, graph 0 to ai_knee_l2, compute_peak nohead dt 3 lc rgb "#0000FF"
set label 2 format_knee(ai_knee_l2) at ai_knee_l2, compute_peak*0.6 center tc rgb "#0000FF"

set arrow from ai_knee_l3, graph 0 to ai_knee_l3, compute_peak nohead dt 3 lc rgb "#9900CC"
set label 3 format_knee(ai_knee_l3) at ai_knee_l3, compute_peak*0.75 center tc rgb "#9900CC"

set arrow from ai_knee_mem, graph 0 to ai_knee_mem, compute_peak nohead dt 3 lc rgb "#FF9900"
set label 4 format_knee(ai_knee_mem) at ai_knee_mem, compute_peak*0.25 center tc rgb "#FF9900"

# Replot to apply labels
replot
