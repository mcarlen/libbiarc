#include "../include/Vector3.h"
#include "../experimental/annealing/my_anneal.cpp"
#include "../include/algo_helpers.h"

class VectorMove: public BasicMove {
public:
  Vector3 old_value;
  int n;
  int flag;
  Curve<Vector3> *c;
  VectorMove(Curve<Vector3> *curve, int i, int flag, float step_size = 1e-6, float STEP_CHANGE = 0.01) :
    BasicMove(step_size,STEP_CHANGE) {
    this->n = i;
    this->flag = flag;
    this->c = curve;
    if (flag)
      this->old_value = (*c)[i].getPoint();
    else
      this->old_value = (*c)[i].getTangent();
  }

  virtual ~VectorMove() {}

  void move() {
    if (flag)
      this->old_value = (*c)[n].getPoint();
    else
      this->old_value = (*c)[n].getTangent();

    Vector3 v = old_value;
    v[0] += step_size * (2.*rand01()-1.);
    v[1] += step_size * (2.*rand01()-1.);
    v[2] += step_size * (2.*rand01()-1.);
    
    if (flag)
      (*c)[n].setPoint(v);
    else
      (*c)[n].setTangent(v);
  }

  void reject() {
    BasicMove::reject();
    if (flag)
      (*c)[n].setPoint(old_value);
    else
      (*c)[n].setTangent(old_value);
  }

  void accept() {
    BasicMove::accept();
    if (flag)
      old_value = (*c)[n].getPoint();
    else
      old_value = (*c)[n].getTangent();
  }
};

class LinkAnneal: public BasicAnneal {
public:
//  bool thickness_fast;
  float step_size_factor;
  float length_penalty;
  CurveBundle<Vector3> knot, best_knot;

  LinkAnneal(const char* knot_filename, const char* params = "") {
    std_init(params);
    knot = CurveBundle<Vector3>(knot_filename);
    knot.link();
    knot.make_default();
    best_knot = knot;
    for (uint i=0; i<knot.curves();++i) {
      Curve<Vector3> *c = &(knot[i]);
      for (int k=0;k<c->nodes();++k) {
        possible_moves.push_back(new VectorMove(c,k,1,step_size_factor));
        possible_moves.push_back(new VectorMove(c,k,0,step_size_factor));
      }
    }
  }

  void std_init(const char* params) {
    BasicAnneal::std_init(params);
    step_size_factor = 0.1;
    // thickness_fast = 0;
    length_penalty = 0;
    map<string,string> param_map;
    str2hash(params, param_map);
    extract_f(step_size_factor,param_map);
//    extract_i(thickness_fast,param_map);
    extract_f(length_penalty,param_map);
  }

  virtual ostream & show_config(ostream &out) {
    BasicAnneal::show_config(out) << "step_size_factor: " << step_size_factor << endl 
        << "length_penalty: " << length_penalty << endl ;
//        << "thickness_fast: " << thickness_fast << endl;
    return out;
  }

  virtual bool stop() {
    return (max_step < 1e-15);
  }

  void best_found() {
    BasicAnneal::best_found();
    best_knot = knot;
    knot.writePKF(best_filename.c_str());
  }

  virtual float energy() {
    /* here: ropelength */
    float penalty = 0;
    float L=0,D;
    knot.make_default();
    for (int i=0;i<knot.curves();++i)
      L += knot[i].length();
    penalty += length_penalty*L;
    /*
    if (thickness_fast)
      D = curve.thickness_fast() + penalty;
    else
    */
      D = compute_thickness(&knot) + penalty;
    return L/D;
  }
};


int main(int argc, char** argv) {
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " filename params " << endl;
    exit(1);
  }
  LinkAnneal a(argv[1],argv[2]);
  a.do_anneal();

  return 0;
}

