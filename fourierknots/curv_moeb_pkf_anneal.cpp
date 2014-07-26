// Vector3.h does "#define float double" => include early
#include <Vector3.h>
#include "../experimental/annealing/my_anneal.cpp"
#include "../include/algo_helpers.h"
#include <iomanip>
#include <sstream>


class BiarcMovePoint : public BasicMove {
public:
  
  Curve<Vector3> *knot;
  int node;
  Vector3 p, old_p;

   BiarcMovePoint(Curve<Vector3> *k,int n, float step_size = 1e-6, float STEP_CHANGE = 0.01) :
    BasicMove(step_size,STEP_CHANGE) {
    knot = k;
    node = n;
    old_p = (*knot)[node].getPoint();
  }

  virtual ~BiarcMovePoint() {}

  void move() {
    p = old_p = (*knot)[node].getPoint();
    // cout << "Before p:" << p << endl;
    p += step_size * Vector3(2.*rand01()-1.,2.*rand01()-1.,2.*rand01()-1.);
    // cout << "After p:" << p << endl << flush;
    (*knot)[node].setPoint(p);
    if (!(*knot)[node].isProper()||!(*knot)[node].getPrevious().isProper()) {
      reject(); //bad move
    }
  }
  /*!
          Reject a move and reset to old value.
  */
  void reject() {
    BasicMove::reject();
    (*knot)[node].setPoint(old_p);
  }

  /*!
          Accept move and backup new value.
  */
  void accept() {
    BasicMove::accept();
    old_p = p;
  }
};


class BiarcMoveTangent : public BasicMove {
public:
  
  Curve<Vector3> *knot;
  int node;
  Vector3 t, old_t;

   BiarcMoveTangent(Curve<Vector3> *k,int n, float step_size = 1e-6, float STEP_CHANGE = 0.01) :
    BasicMove(step_size,STEP_CHANGE) {
    knot = k;
    node = n;
    old_t = (*knot)[node].getTangent();
  }

  virtual ~BiarcMoveTangent() {}

  void move() {
    t = old_t = (*knot)[node].getTangent();
    // cout << "Before t:" << t << endl;
    t += step_size * Vector3(2.*rand01()-1.,2.*rand01()-1.,2.*rand01()-1.);
    t.normalize();
    (*knot)[node].setTangent(t);
    if (!(*knot)[node].isProper()||!(*knot)[node].getPrevious().isProper()) {
      reject(); //bad move
    }
  }
  /*!
          Reject a move and reset to old value.
  */
  void reject() {
    BasicMove::reject();
    (*knot)[node].setTangent(old_t);
  }

  /*!
          Accept move and backup new value.
        */
  void accept() {
    BasicMove::accept();
    old_t = t;
  }
};



class PKFAnneal: public BasicAnneal {
protected:
  int hinti, hintj;
public:
  Curve<Vector3> knot, best_knot;
  bool thickness_fast;
  int intnodes, display_all, animate;
  float step_size_factor;
  float length_penalty, eps_me, eps_ce, eps_thick;

  PKFAnneal(const char* knot_filename, const char* params = "") {
    std_init(params);
    if (!knot.readPKF(knot_filename)) {
       cout << "Error reading knotfile '" << knot_filename << "'!" << endl;
       };
    knot.link();
    best_knot = knot;
    for( int i =0 ; i<knot.nodes(); i++) {
       possible_moves.push_back(new BiarcMovePoint(&knot, i, step_size_factor));
       possible_moves.push_back(new BiarcMoveTangent(&knot, i, step_size_factor));
       }
    /* FIXME
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
    */
  }

  void std_init(const char* params) {
    BasicAnneal::std_init(params);
    step_size_factor = 0.1;
    thickness_fast = 0;
    length_penalty = 0;
    eps_me = 0.1;
    eps_ce = 1;
    eps_thick = 0;
    display_all = 0;
    animate = 0;
    map<string,string> param_map;
    str2hash(params, param_map);
    intnodes = 4 * 200;
    extract_f(step_size_factor,param_map);
    extract_i(thickness_fast,param_map);
    extract_f(length_penalty,param_map);
    extract_i(intnodes,param_map);
    extract_i(display_all,param_map);
    extract_i(animate,param_map);
    extract_f(eps_me,param_map);
    extract_f(eps_ce,param_map);
    extract_f(eps_thick,param_map);

    // Set the hint values to -1
    hinti = hintj = -1;
  }

  virtual ostream & show_config(ostream &out) {
    BasicAnneal::show_config(out)
        << "step_size_factor: " << step_size_factor << endl 
        << "length_penalty: " << length_penalty << endl 
        << "intnodes: " << intnodes<< endl 
        << "eps_me: " << eps_me << endl
        << "eps_ce: " << eps_ce << endl
        << "eps_thick: " << eps_thick << endl;
    return out;
  }

  virtual bool stop() {
    return (max_step < 1e-15);
  }

  void best_found() {
    time_t t;
    string fn = best_filename;
    stringstream buf;
    if (animate) {
       buf << time(&t);
       fn += ".";
       fn += buf.str() ;
       fn += ".pkf";
    }
    BasicAnneal::best_found();
    
    best_knot = knot;
    knot.writePKF(fn.c_str());
  }

  virtual float energy() {
    /* here: ropelength */
    float penalty = 0;
    float D,L, me = 0., curv_e = 0., thick_e=0;
    knot.make_default();
    L = knot.length();
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
        vl[i]=knot.pointAt(i*step_size);
      } 
      for (int i=0; i<intnodes;i++) {
        for (int j=0; j<intnodes;j++) {
          if (i==j) continue;
          me += (1./(vl[i]-vl[j]).norm2()*step_size*step_size
                  - 1./pow( min( (i-j+intnodes) % intnodes, 
                                 (j-i+intnodes) % intnodes),2));
         }
      }
      if (display_all) {cout << "Moebius Energy:" << me << endl; }
    }

    //calculate thickness energy
    if (eps_thick > 0.) {
      thick_e = L / knot.thickness(NULL,NULL);
      if (display_all) {cout << "Thickness Energy:" << thick_e << endl; }
    }
    if (display_all) { 
      //calc inf_{s,t\in\R/\Z} \frac{|\g(s)-\g(t)|}{D_\g(s,t)}
      Vector3 vl[intnodes];
      float step_size = L/intnodes, infimum, current;
      int i=1,j=2;
      for (int i=0; i<intnodes;i++) {
        vl[i]=knot.pointAt(i*step_size);
      } 
      infimum = sqrt((vl[i]-vl[j]).norm2())/
                    ( min( (i-j+intnodes) % intnodes, 
                      (j-i+intnodes) % intnodes)*step_size );
      for (i=0; i<intnodes;i++) {
        for (j=0; j<intnodes;j++) {
          if (i==j) continue;
          current = (vl[i]-vl[j]).norm()/
                    ( min( (i-j+intnodes) % intnodes, 
                           (j-i+intnodes) % intnodes)*step_size );
          if (current < infimum) { 
                       infimum = current; 
             }
         }
      }
    cout << "inf_{s,t} {|g(s)-g(t)|}/{D_g(s,t)}:" 
         << infimum << endl;
    }
    //calculate curvature energy 
    if (eps_ce > 0.) {
      for (vector<Biarc<Vector3> >::iterator current=knot.begin();current!=knot.end();current++) {
        curv_e += 1./pow(current->radius0(),2)*current->arclength0();
        curv_e += 1./pow(current->radius1(),2)*current->arclength1();
      }
      if (display_all) {cout << "Bending Energy:" << curv_e << endl;}
    }

    return penalty + eps_me*me + L*eps_ce*curv_e + eps_thick*thick_e;
  }
};


// FIXME: (HG) Yes! I'm ashamed of the NOMAIN-construction. ;)
#ifndef NOMAIN
int main(int argc, char** argv) {
  PKFAnneal *a;
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " filename params " << endl;
    exit(1);
  }
  a= new PKFAnneal(argv[1],argv[2]); 
  a->do_anneal();
  return 0;
}
#endif
