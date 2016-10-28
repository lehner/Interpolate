set terminal postscript color enhanced
set output 'test2d.ps'
splot 'testsimple.dat' using 1:2:3, '' using 1:2:4
splot 'testnlinear.dat' using 1:2:3, '' using 1:2:4
set zrange [-0.1:0.1]
splot 'testnlinear.dat' using 1:2:(($3-$4)/($3+$4))

f(x)=a*x + b*x**2

set xrange [0:0.01]
fit f(x) 'test-conv.dat' using (1/$1**2):2 via a,b
set xlabel "1/N^2"
set ylabel "eps/val"
plot 'test-conv.dat' using (1/$1**2):2 title "Convergence of integral using nlinear interpolation (./test-conv)", \
f(x) title "Linear plus quadratic in 1/N^2"
