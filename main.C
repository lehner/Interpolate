#include <stdio.h>
#include "Interpolate.h"
#include <complex>

typedef std::complex<double> Complex;
#define PI 3.14159265359

Complex fnc(double* x) {
  return cos(x[0]) * cos(x[1]) * cos(x[2]) * exp( - (x[3]*x[3] - x[4]*x[4]) / 100.0 );
}

template<class C> struct simple_int {
  
  typedef Interpolate<C,simple_int<C> > IP;
  C operator()(IP* ip, std::vector<int>& il, std::vector<double>& vl) {
    
    // do interpolation
    C v0 = ip->get(ip->get_index(il));
    size_t  j;
    for (j=0;j<ip->dims.size();j++) {
      C dj = ip->get(ip->get_index(ip->index_plus(il,j))) - v0;
	
      v0 += dj * vl[j];
    }
      
    return v0;
  }
    
};

template<class C> struct nlinear_int {
  
#define BIT(b) ( (1<<b) )

  typedef Interpolate<C,nlinear_int<C> > IP;
  C operator()(IP* ip, std::vector<int>& il, std::vector<double>& vl) {

    //
    // n-dimensional box, interpolate one dimension at a time
    //
    // Q_{000...0} is fundamental vertex of box
    // Q_{100...0} is 1 hop in 0-dimension
    // ....
    // Q_{111...1} is 1 hop in all dimensions
    //
    // for n dimensions this means 2^n points are involved in interpolation
    // for n = 5 this means 32 points
    //

    std::vector<C> Q(1<<il.size());
    size_t points, d;
    for (points=0;points<Q.size();points++) {
	std::vector<int> i = il;
	for (d=0;d<il.size();d++)
	  if (BIT(d) & points)
	    i = ip->index_plus(i,d);

	//printf("Point %d: %d %d %d %d %d\n",points,i[0],i[1],i[2],i[3],i[4]);
        Q[points] = ip->get(ip->get_index(i));
    }

    // exit(1);
    // do interpolation for each dimension, each time reducing Q.size() by a factor of two
    for (d=0;d<il.size();d++) {
	std::vector<C> Qp;
	double a = 1.0 - vl[d];
	double b = vl[d];
	// all even indices in Q are at x0, all odd indices are at x1 ->
	for (points=0;points<Q.size();points+=2)
	  Qp.push_back(Q[points]*a + Q[points+1]*b);
	Q = Qp;
    }

    return Q[0];
  }
    
};

int main(int argc, char* argv[]) {
  Interpolate<Complex,MOD<Complex> > ip;

  // angles
  //               i=0  i=31
  ip.Dimension(16, 0.0, 2.0*PI);
  ip.Dimension(16, 0.0, 2.0*PI);
  ip.Dimension(16, 0.0, 2.0*PI);

  // lengths
  ip.Dimension(32, 0, 64.0);
  ip.Dimension(32, 0, 64.0);


  // fill in test data
  int i;
  for (i=0;i<ip.size;i++) {
    double x[5];
    ip.coor(i,x);
    ip.get(i) = fnc(x);
  }

  // test output
  FILE* ftest = fopen("test" SMOD ".dat","wt");
  double pos2[5] = { 0.0, 0.2, 0.4, 33, 43 };
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
