//FIXME: (HG) NOMAIN-construction sucks!
#define NOMAIN 1
#include "new_fourier_anneal.cpp"
#undef NOMAIN

template <class FK>
class ScaleMove : public BasicMove {
public:
  float old_value;
  FK *knot;

  /*!
    knot - address of the knot in anneal.
  */
  ScaleMove(FK *knot, float step_size = 1e-6, float STEP_CHANGE = 0.01) :
    BasicMove(step_size, STEP_CHANGE) {
    this->knot = knot; 
    this->old_value = 1.0;
  }

  virtual ~ScaleMove() {}

  void move() {
    old_value = 1. + step_size * (2.*rand01()-1.);
    // cout << "Before:" << *node << endl;
    knot->mul(old_value);
    // cout << "After:" << *node << endl << flush;
  }

  void reject() {
    BasicMove::reject();
    knot->div(old_value);
  }

  void accept() {
    BasicMove::accept();
    old_value = 1.;
  }
};



class FKAnnealOnS3: public FKAnneal<FourierKnot>{

public:
  bool enable_scale_moves;
  FKAnnealOnS3(const char* knot_filename, const char* params = ""):FKAnneal<FourierKnot>(knot_filename,params) {
    std_init(params);
    knot = FourierKnot(knot_filename);
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

    if (enable_scale_moves) possible_moves.push_back(new ScaleMove<FourierKnot>(&knot, step_size_factor*(1e-5)));
  }

  void std_init(const char* params) {
    FKAnneal<FourierKnot>::std_init(params);
    enable_scale_moves=0;
    map<string,string> param_map;
    str2hash(params, param_map);
    extract_i(enable_scale_moves,param_map);
  }

  virtual ostream & show_config(ostream &out) {
    FKAnneal<FourierKnot>::show_config(out) << "enable_scale_moves: " << enable_scale_moves << endl;
    return out;
  }

  virtual float energy() {
    float penalty = 0.0, e = 0.0;
    Curve<Vector4> curve;
    knot.toCurveOnS3(NODES,&curve);
    curve.link();
    curve.make_default(); 
    if (length_penalty != 0.0) 
      penalty += length_penalty*curve.length();
    curve.set_hint(hinti, hintj);
    if (thickness_fast)
      e = 1./curve.thickness_fast() + penalty;
    else
      e = 1./curve.thickness() + penalty;
    curve.get_hint(&hinti, &hintj);
    return e;
  }
};



class TrefFKAnnealOnS3: public TrefFKAnneal {
public:
  TrefFKAnnealOnS3(const char* knot_filename, const char* params = ""):TrefFKAnneal(knot_filename, params) {
  }

  virtual float energy() {
    float penalty = 0, e=0.0 ;
    Curve<Vector4> curve;
    knot.toCurveOnS3(NODES,&curve);
    curve.link();
    curve.make_default(); 
    if (length_penalty != 0.0) 
      penalty += length_penalty*curve.length();
    curve.set_hint(hinti, hintj);
    if (thickness_fast)
      e = 1./curve.thickness_fast() + penalty;
    else
      e = 1./curve.thickness() + penalty;
    curve.get_hint(&hinti, &hintj);
    return e;
  }
};


class K51FKAnnealOnS3: public K51FKAnneal {
public:
  K51FKAnnealOnS3(const char* knot_filename, const char* params = ""):K51FKAnneal(knot_filename, params) {
  }

  virtual float energy() {
    float penalty = 0, e=0.0;
    Curve<Vector4> curve;
    knot.toCurveOnS3(NODES,&curve);
    curve.link();
    curve.make_default(); 
    if (length_penalty != 0.0) 
      penalty += length_penalty*curve.length();
    curve.set_hint(hinti, hintj);
    if (thickness_fast)
      e = 1./curve.thickness_fast() + penalty;
    else
      e = 1./curve.thickness() + penalty;
    curve.get_hint(&hinti, &hintj);
    return e;
  }
};



int main(int argc, char** argv) {
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " [n|3|5] filename params " << endl;
    exit(1);
  }
  switch(argv[1][0]) {
  case 'n': FKAnnealOnS3 * fkn;
            fkn= new FKAnnealOnS3(argv[2],argv[3]); 
            fkn->do_anneal();
            break;
  case '3': TrefFKAnnealOnS3 * fk3;
            fk3= new TrefFKAnnealOnS3(argv[2],argv[3]); 
            fk3->do_anneal();
            break;
  case '5': K51FKAnnealOnS3 * fk5;
            fk5= new K51FKAnnealOnS3(argv[2],argv[3]); 
            fk5->do_anneal();
            break;
  default:
    cerr << "Wrong Fourier Knot type [n|3|5]\n" << endl;
    exit(1);
  }
  return 0;
}
