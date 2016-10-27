all: main-simple main-nlinear test-conv

main-simple: main.C
	g++ -o main-simple -DMOD=simple_int -DSMOD='"simple"' -O3 main.C

main-nlinear: main.C
	g++ -o main-nlinear -DMOD=nlinear_int -DSMOD='"nlinear"' -O3 main.C

test-conv: test-conv.C
	g++ -o test-conv -O3 test-conv.C

