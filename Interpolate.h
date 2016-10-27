/*
  Simple interpolation library

  Authors: Christoph Lehner

  Date: 2016
*/
#include <vector>

#define _EPS_NUM 1e-14

template<class C,class I> class Interpolate {
 public:
  
  struct _dim {
    int N;
    double x0;
    double x1;
  };

  std::vector<_dim> dims;

  void get_icoor(int i, std::vector<int>& icoor) {
    size_t j;
    for (j=0;j<dims.size();j++) {
      icoor[j] = i % dims[j].N;
      i /= dims[j].N;
    }
  }

  int get_index(const std::vector<int>& icoor) {
    int i = 0;

    int j;
    for (j=(int)dims.size()-1;j>=0;j--) {
      i *= dims[j].N;
      i += icoor[j];
    }

    return i;

  }

  std::vector<int> index_plus(std::vector<int>& input, int i) {
    std::vector<int> ret = input;
    ret[i] += 1;
    if (ret[i] == dims[i].N) // this is intentional for use below
      ret[i] -= 1;
    return ret;
  }

  void icoor_to_coor(std::vector<int>& icoor, double* x) {
    size_t j;
    for (j=0;j<dims.size();j++) {
      _dim* d = &dims[j];
      double lam = (double)icoor[j] / (double)(d->N-1);
      x[j] = d->x0 + lam * (d->x1 - d->x0);
    }
  }

  size_t size;
  C* data;

  // constructor / destructor
  Interpolate() {
    size = 1;
    data = 0;
  }

  ~Interpolate() {
    if (data)
      delete[] data;
  }

  // use N-grid in next dimension with i=0 <> x0 and i=N-1 <> x1
  void Dimension(int N, double x0, double x1) {
    _dim d = { N,x0,x1 };
    dims.push_back(d);
    size *= N;
    if (data)
      delete [] data;
    data = new C[size];
  }

  // get coordinate of grid point i
  void coor(int i, double* x) {
    std::vector<int> icoor(dims.size());
    get_icoor(i, icoor);
    icoor_to_coor(icoor,x);

    //int jjj = get_index(icoor);
    //if (jjj != i)
    //  throw "Logic bomb";
  }

  // get value on grid point i
  C& get(int i) {
    return data[i];
  }

  // get interpolated value at point x
  C get(double* x) {
    
    std::vector<int> il(dims.size());
    std::vector<double> vl(dims.size());

    // get coordinate left of x in each dimension
    size_t j;
    for (j=0;j<dims.size();j++) {
      _dim* d = &dims[j];
      double fj = (x[j] - d->x0) / (d->x1 - d->x0); // 0 <= fj <= 1
      if (fj < 0.0 - _EPS_NUM || fj > 1.0 + _EPS_NUM)
	throw "Out of bounds";
      int ileft = (int)(fj * (d->N-1)); // fringe case for x[j] == d->x1, ileft=d->N-1
      int iright = (ileft == d->N-1) ? ileft : ileft + 1;
      double dj = (d->x1 - d->x0) / (double)(d->N - 1);
      double lam = (x[j] - d->x0 - dj*ileft) / dj;
      if (lam < 0.0 - _EPS_NUM || lam > 1.0 + _EPS_NUM)
	throw "Logic bomb";

      il[j] = ileft;
      vl[j] = lam;
    }

    return I()(this,il,vl);
  }

};




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
