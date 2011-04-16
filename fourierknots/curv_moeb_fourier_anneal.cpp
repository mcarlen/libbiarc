#include "fourier_syn.h"
#include "fourier_3_1.h"
#include "fourier_4_1.h"
#include "fourier_5_1.h"
#include "fourier_8_18.h"
#include "../experimental/annealing/my_anneal.cpp"
#include "../include/algo_helpers.h"
#include <iomanip>


float adjust51(float x) {
  float shift = 0.2;
  float lx = x + shift;
  if (lx>1.0) lx -= 1.0;
  return (lx+0.7/(5.*2.*M_PI)*cos(5.*2.*M_PI*lx));
}

template <class FK>
class FKAnneal: public BasicAnneal {
protected:
  int hinti, hintj;
public:
  bool thickness_fast;
  int NODES, intnodes, display_all;
  float step_size_factor;
  float length_penalty, eps_me, eps_ce;
  FK knot, best_knot;


  FKAnneal(const char* knot_filename, const char* params = "") {
    std_init(params);
    knot = FK(knot_filename);
    best_knot = knot;
    for (uint i=0; i<knot.csin.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.csin[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*fabs(knot.csin[i][j])));
      }
    }
    for (uint i=0; i<knot.ccos.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
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
    step_size_factor = 0.1;
    thickness_fast = 0;
    length_penalty = 0;
    eps_me = 0.1;
    eps_ce = 1;
    display_all = 0;
    map<string,string> param_map;
    str2hash(params, param_map);
    extract_i(NODES,param_map);
    intnodes = 4 * NODES;
    extract_f(step_size_factor,param_map);
    extract_i(thickness_fast,param_map);
    extract_f(length_penalty,param_map);
    extract_i(intnodes,param_map);
    extract_i(display_all,param_map);
    extract_f(eps_me,param_map);
    extract_f(eps_ce,param_map);

    // Set the hint values to -1
    hinti = hintj = -1;
  }

  virtual ostream & show_config(ostream &out) {
    BasicAnneal::show_config(out) << "NODES: " << NODES << endl
        << "step_size_factor: " << step_size_factor << endl 
        << "length_penalty: " << length_penalty << endl 
        << "intnodes: " << intnodes<< endl 
        << "eps_me: " << eps_me << endl
        << "eps_ce: " << eps_ce << endl;
    return out;
  }

  virtual bool stop() {
    return (max_step < 1e-15);
  }

  void best_found() {
    BasicAnneal::best_found();
    best_knot = knot;
    ofstream out(best_filename.c_str());
    out.precision(16);
    out << knot; 
    out.close();
  }

  virtual float energy() {
    /* here: ropelength */
    float penalty = 0;
    float D,L, me = 0., curv_e = 0.;
    Vector3 v;
    Curve<Vector3> curve;
    knot.toCurve(NODES,&curve);
    curve.link();
    curve.make_default();
    if (display_all) {
         curve.normalize();
         curve.make_default();
    }
    L = curve.length();
    //D = curve.thickness();
    penalty += length_penalty*fabs(L-1.);
    //penalty += length_penalty*pow(L-1.,2);
    //penalty += length_penalty*L;
    //calculate moebius-energy
    if (display_all) { cout << "Length:" << L << endl; }
    if (eps_me > 0.) {
      Vector3 vl[intnodes];
      float step_size = L/intnodes;
      for (int i=0; i<intnodes;i++) {
        vl[i]=curve.pointAt(i*step_size);
      } 
      for (int i=0; i<intnodes;i++) {
        for (int j=0; j<intnodes;j++) {
          if (i==j) continue;
          me += (1./(vl[i]-vl[j]).norm2()*step_size*step_size
                  - 1./pow( min( (i-j) % intnodes, (j-i) % intnodes),2));
         }
      }
      if (display_all) {cout << "Moebius Energy:" << me << endl; }
    }
    //calculate curvature energy 
    if (eps_ce > 0.) {
      for (vector<Biarc<Vector3> >::iterator current=curve.begin();current!=curve.end();current++) {
        curv_e += 1./pow(current->radius0(),2)*current->arclength0();
        curv_e += 1./pow(current->radius1(),2)*current->arclength1();
      }
      if (display_all) {cout << "Bending Energy:" << curv_e << endl;}
    }

    return penalty + eps_me*me + L*eps_ce*curv_e;
  }
};


class TrefFKAnneal: public FKAnneal<TrefoilFourierKnot> {
public:
  TrefFKAnneal(const char* knot_filename, const char* params = ""):FKAnneal<TrefoilFourierKnot>(knot_filename, params) {
    std_init(params);
    knot = TrefoilFourierKnot(knot_filename);
    best_knot = knot;
    for (uint i=0; possible_moves.size(); ++i)
      delete possible_moves[i];
    possible_moves.clear();
    for (uint i=0; i<knot.csin.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.csin[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*fabs(knot.csin[i][j])));
      }
    }
  }

  virtual float energy() {
    float penalty = 0;
    Curve<Vector3> curve;
    knot.toCurve(adjust,NODES,&curve);
    curve.link();
    curve.make_default();

    // Get/Set hint for thickness
    curve.set_hint(hinti,hintj);
    float D = curve.thickness();
    curve.get_hint(&hinti,&hintj);

    float L = curve.length();
    penalty += length_penalty*L;
    return L/D + penalty;
  }
};

class K41FKAnneal: public FKAnneal<K41FourierKnot> {
public:
  K41FKAnneal(const char* knot_filename, const char* params = ""):FKAnneal<K41FourierKnot>(knot_filename, params) {
    std_init(params);
    knot = K41FourierKnot(knot_filename);
    best_knot = knot;
    for (uint i=0; possible_moves.size(); ++i)
      delete possible_moves[i];
    possible_moves.clear();

    for (uint i=0; i<knot.csin.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.csin[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*fabs(knot.csin[i][j])));
      }
    }
    for (uint i=0; i<knot.ccos.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.ccos[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*fabs(knot.ccos[i][j])));
      }
    }
  }

  virtual float energy() { // ropelength(K41FourierKnot &fk) {
    Curve<Vector3> curve;
    knot.toCurve(adjust,NODES,&curve);
    curve.link();
    curve.make_default();

    // Get/Set hint for thickness
    curve.set_hint(hinti,hintj);
    float D = curve.thickness();
    curve.get_hint(&hinti,&hintj);

    float L = curve.length();
    return L/D;
  }
};


class K51FKAnneal: public FKAnneal<K51FourierKnot> {
public:
  K51FKAnneal(const char* knot_filename, const char* params = ""):FKAnneal<K51FourierKnot>(knot_filename, params) {
    std_init(params);
    knot = K51FourierKnot(knot_filename);
    best_knot = knot;
    for (uint i=0; possible_moves.size(); ++i)
      delete possible_moves[i];
    possible_moves.clear();

    for (uint i=0; i<knot.csin.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.csin[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*fabs(knot.csin[i][j])));
      }
    }
    for (uint i=0; i<knot.ccos.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.ccos[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*fabs(knot.ccos[i][j])));
      }
    }
  }

  virtual float energy() {
    Curve<Vector3> curve;
    knot.toCurve(adjust51,NODES,&curve);
    curve.link();
    curve.make_default();

    // Get/Set hint for thickness
    curve.set_hint(hinti,hintj);
    float D = curve.thickness();
    curve.get_hint(&hinti,&hintj);

    float L = curve.length();
    return L/D;
  }
};

class K818FKAnneal: public FKAnneal<K818FourierKnot> {
public:
  K818FKAnneal(const char* knot_filename, const char* params = ""):FKAnneal<K818FourierKnot>(knot_filename, params) {
    std_init(params);
    knot = K818FourierKnot(knot_filename);
    best_knot = knot;
    for (uint i=0; possible_moves.size(); ++i)
      delete possible_moves[i];
    possible_moves.clear();

    for (uint i=0; i<knot.csin.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.csin[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.csin[i][j]), step_size_factor*fabs(knot.csin[i][j])));
      }
    }
    for (uint i=0; i<knot.ccos.size(); ++i) {
      for (uint j=0; j<3; ++j) { 
        if (fabs(knot.ccos[i][j]) < 1e-8)
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*(1e-8)));
        else
          possible_moves.push_back(new SimpleFloatMove(&(knot.ccos[i][j]), step_size_factor*fabs(knot.ccos[i][j])));
      }
    }
  }

  virtual float energy() {
    Curve<Vector3> curve;
    knot.toCurve(NODES,&curve);
    curve.link();
    curve.make_default();
    curve.normalize();
    curve.make_default();

    // Get/Set hint for thickness
    // curve.set_hint(hinti,hintj);
    float D;
    if (thickness_fast)
      D = curve.thickness_fast();
    else
      D = curve.thickness();
    // curve.get_hint(&hinti,&hintj);

    float L = curve.length();
    return L/D;
  }
};


// FIXME: (HG) Yes! I'm ashemed of the NOMAIN-construction. ;)
#ifndef NOMAIN
int main(int argc, char** argv) {
  FKAnneal<FourierKnot> *a;
  TrefFKAnneal* t;
  K41FKAnneal* k41;
  K51FKAnneal* k51;
  K818FKAnneal* k818;
  if (argc != 4) {
    cout << "Usage: " << argv[0] << "  [n|3|4|5|8] filename params " << endl;
    exit(1);
  }
  switch(argv[1][0]) {
  case 'n':
    a = new FKAnneal<FourierKnot>(argv[2],argv[3]); 
    a->do_anneal();
    break;
  case '3':
    t = new TrefFKAnneal(argv[2],argv[3]); 
    t->do_anneal();
    break;
  case '4':
    k41 = new K41FKAnneal(argv[2],argv[3]);
    k41->do_anneal();
    break;
  case '5':
    k51=new K51FKAnneal(argv[2],argv[3]);
    k51->do_anneal();
    break;
  case '8':
    k818=new K818FKAnneal(argv[2],argv[3]);
    k818->do_anneal();
    break;
  default:
    cerr << "Wrong Fourier Knot type [n|3|4|5|8]\n";
    exit(1);
  }
  return 0;
}
#endif
