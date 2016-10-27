set terminal postscript color enhanced
set output 'test2d.ps'
plot 'testsimple.dat' using 1:2:3, '' using 1:2:4
plot 'testnlinear.dat' using 1:2:3, '' using 1:2:4
set zrange [-0.1:0.1]
plot 'testnlinear.dat' using 1:2:(($3-$4)/($3+$4))
