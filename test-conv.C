#include <stdio.h>
#include "Interpolate.h"
#include <complex>

typedef std::complex<double> Complex;
#define PI 3.14159265359

Complex fnc(double* x) {
  return cos(x[0]) * cos(x[1]) * cos(x[2]) * exp( - (x[3]*x[3] - x[4]*x[4]) / 100.0 );
}

double get_residuum(int N, int Ntest) {

  Interpolate<Complex,nlinear_int<Complex> > ip;

  // angles
  ip.Dimension(N, 0.0, 2.0*PI);
  ip.Dimension(N, 0.0, 2.0*PI);
  ip.Dimension(N, 0.0, 2.0*PI);

  // lengths
  ip.Dimension(N, 0, 31.0);
  ip.Dimension(N, 0, 31.0);

  // fill in test data
  int i;
  double x[5];
  for (i=0;i<ip.size;i++) {
    ip.coor(i,x);
    ip.get(i) = fnc(x);
  }

  // calculate residuum
  int j[5];
#define FOR(n)					\
  for (j[n]=0;j[n]<Ntest;j[n]++)
#define UPDATE(n)							\
  x[n] = ip.dims[n].x0 + (double)j[n] / (double)Ntest * (ip.dims[n].x1 - ip.dims[n].x0);

  double res = 0.0;
  double eps = 0.0;

  FOR(0) { UPDATE(0);
    FOR(1) { UPDATE(1);
      FOR(2) { UPDATE(2);
	FOR(3) { UPDATE(3);
	  FOR(4) { UPDATE(4);

	    Complex f = fnc(x);
	    Complex delta = f - ip.get(x);

	    eps += norm(delta);
	    res += norm(f);
	  }
	}
      }
    }
  }

  return sqrt(eps/res);
}

int main(int argc, char* argv[]) {
  int Ntest = 16;
  for (int N=2;N<8;N+=2) {
    printf("%d %g\n",N,get_residuum(N,Ntest));
  }
  return 0;
}
