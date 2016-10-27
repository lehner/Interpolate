/*
  Simple interpolation library

  Authors: Christoph Lehner

  Date: 2016
*/
#include <vector>
#include <iterator>


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
      if (fj < 0.0 || fj > 1.0)
	throw "Out of bounds";
      int ileft = (int)(fj * (d->N-1)); // fringe case for x[j] == d->x1, ileft=d->N-1
      int iright = (ileft == d->N-1) ? ileft : ileft + 1;
      double dj = (d->x1 - d->x0) / (double)(d->N - 1);
      double lam = (x[j] - d->x0 - dj*ileft) / dj;
      if (lam < 0.0 || lam > 1.0)
	throw "Logic bomb";
      il[j] = ileft;
      vl[j] = lam;
    }

    return I()(this,il,vl);
  }

};
