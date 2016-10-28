#include <stdio.h>
#include "Interpolate.h"
#include <complex>

typedef std::complex<double> Complex;
#define PI 3.14159265359

Complex fnc(double* x) {
  return cos(x[0]) * sin(x[1]) * cos(x[2]) * exp( - (x[3]*x[3] - x[4]*x[4]) / 100.0 );
}

#ifndef MOD
#define MOD nlinear_int
#define SMOD "nlinear"
#endif

int main(int argc, char* argv[]) {
  Interpolate<Complex,MOD<Complex> > ip;

  // angles
  //               i=0  i=31
  ip.Dimension(16, 0.0, 2.0*PI);
  ip.Dimension(16, 0.0, 2.0*PI);
  ip.Dimension(16, 0.0, 2.0*PI);

  // lengths
  ip.Dimension(32, 0, 31.0);
  ip.Dimension(32, 0, 31.0);


  // fill in test data
  int i;
  for (i=0;i<ip.size;i++) {
    double x[5];
    ip.coor(i,x);
    ip.get(i) = fnc(x);
  }

  // test output
  FILE* ftest = fopen("test" SMOD ".dat","wt");
  double pos2[5] = { 0.0, 0.2, 0.4, 13.0, 12.0 };
  //double pos2[5] = { 0.3, 0.2, 0.4, 13.5, 12.5 };
  for (i=0;i<100.0;i++) {
    pos2[2] = 2.0*PI * i / 100.0;
    int j;
    for (j=0;j<100.0;j++) {
      pos2[1] = 2.0*PI * j / 100.0;
      fprintf(ftest,"%g %g %g %g\n",pos2[1],pos2[2],fnc(pos2).real(),ip.get(pos2).real());
    }
    fprintf(ftest,"\n");
  }
  fclose(ftest);
  


}
