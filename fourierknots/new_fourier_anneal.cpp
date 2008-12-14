#include "fourier_syn.h"
#include "fourier_3_1.h"
#include "fourier_4_1.h"
#include "../experimental/annealing/my_anneal.cpp"
#include "../include/algo_helpers.h"
#include <iomanip>


template <class FK>
class FKAnneal: public BasicAnneal {

public:

  float ropelength(FK &fk) {
    Curve<Vector3> curve;
    fk.toCurve(adjust,NODES,&curve);
    //  fk.toCurve(adjust_with_spline,NODES,&curve);
    curve.link();
    curve.make_default();
    float D = curve.thickness();
    float L = curve.length();
    return L/D;
  }


  int NODES;
  float step_size_factor;
  FK knot, best_knot;

  FKAnneal(const char* knot_filename, const char* params = "") {
    std_init(params);
    knot = FK(knot_filename);
    best_knot = knot;
    for (int i=0; i<knot.csin.size(); ++i) {
      for (int j=0; j<3; ++j) { 
        if (fabs(knot.csin[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*fabs(knot.csin[i][j])));

        if (fabs(knot.ccos[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*fabs(knot.ccos[i][j])));
      }
    }
  }

  void std_init(const char* params) {
    BasicAnneal::std_init(params);
    NODES = 200;
    step_size_factor=0.1;
    map<string,string> param_map;
    str2hash(params, param_map);
    extract_i(NODES,param_map);
    extract_f(step_size_factor,param_map);
  }

/*
  virtual bool stop() {
    cout << __LINE__ << endl << flush;
    return false;
  }
*/

  void best_found() {
    BasicAnneal::best_found();
    best_knot = knot;
    ofstream out(best_filename.c_str());
    out << knot; 
    out.close();
  }

  float energy() {
    return ropelength(knot);
  }
};


int main(int argc, char** argv) {

  FKAnneal<FourierKnot> * a;
  const char* def = "T=0.001,NODES=200,best_filename=best.txt";

  if (argc>2) a = new FKAnneal<FourierKnot>(argv[1],argv[2]);
  else {
    cout << "Usage:  bla filename params: " << def << endl;
    exit(1);
  }
  a-> do_anneal();

  return 0;
}
