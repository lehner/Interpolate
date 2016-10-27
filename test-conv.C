#include <stdio.h>
#include "Interpolate.h"
#include <complex>
#include <memory.h>
#include <omp.h>

typedef std::complex<double> Complex;
#define PI 3.14159265359

Complex fnc(double* x) {
  return cos(x[0]) * exp( - (x[3]*x[3] - x[4]*x[4]) / 100.0 );
}

double get_residuum(int N, int Ntest) {

  Interpolate<Complex,nlinear_int<Complex> > ip;

  // angles
  ip.Dimension(N, 0.0, 2.0*PI);
  ip.Dimension(N, 0, 31.0);
  ip.Dimension(N, 0, 31.0);

  // fill in test data
  {
    int i;
    double x[5];
    for (i=0;i<ip.size;i++) {
      ip.coor(i,x);
      ip.get(i) = fnc(x);
    }
  }

  // calculate residuum

#define UPDATE(n)							\
  x[n] = ip.dims[n].x0 + (double)j[n] / (double)Ntest * (ip.dims[n].x1 - ip.dims[n].x0);

  double res_glb = 0.0;
  double eps_glb = 0.0;

#define FOR(n)					\
  for (j[n]=0;j[n]<Ntest;j[n]++)



  int nthreads;
#pragma omp parallel
  {
    nthreads = omp_get_num_threads();
  }

  //printf("Nthreads = %d, Ntest = %d\n",nthreads,Ntest);
  double* res = new double[nthreads];
  double* eps = new double[nthreads];
  memset(res,0,sizeof(double)*nthreads);
  memset(eps,0,sizeof(double)*nthreads);

#pragma omp parallel
  {
    int ithread = omp_get_thread_num();
    int idx = 0;

    int j[3];
    double x[3];
    FOR(0) { UPDATE(0);
      FOR(1) { UPDATE(1);
	FOR(2) { UPDATE(2);

	  if (idx % nthreads == ithread) {
	    
	    Complex f = fnc(x);
	    Complex delta = f - ip.get(x);
	    
	    eps[ithread] += norm(delta);
	    res[ithread] += norm(f);
	  }
	  
	  idx++;

	}
      }
    }
  }

  for (int i=0;i<nthreads;i++) {
    res_glb += res[i];
    eps_glb += eps[i];
  }

  delete[] res;
  delete[] eps;

  return sqrt(eps_glb/res_glb);
}

int main(int argc, char* argv[]) {
  int Ntest = 100;
      
  for (int N=4;N<=64;N+=4) {
    printf("%d %g\n",N,get_residuum(N,Ntest));
  }

  return 0;
}
