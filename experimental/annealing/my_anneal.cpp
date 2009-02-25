#ifndef _ANNEAL_H_
#define _ANNEAL_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <math.h>

using namespace std;

#define rand01() ((float)rand()/(float)RAND_MAX)
#define randint(val) (rand()%val)

void Tokenize(const string& str, vector<string>& tokens,
              const char delimiters = ',') {

  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos) {

    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

void Tokenize2(const string &str, vector<string>& tokens,
               const char delim1 = ',', const char delim2 = '=') {
  vector<string> first_step;
  Tokenize(str, first_step, delim1);
  for (vector<string>::iterator it = first_step.begin();
       it!=first_step.end();++it)
    Tokenize(*it, tokens, delim2);
}


/*!
  Basic class for annealing move.
*/
class BasicMove {
public:

  float step_size;
  float STEP_CHANGE;

  /*!
    Constructor. Sets step_size and STEP_CHANGE.
  */
  BasicMove(float step_size = 1e-6, float STEP_CHANGE = 0.01) {
    this->step_size = step_size;
    this->STEP_CHANGE = STEP_CHANGE;
  }  
  virtual ~BasicMove() {}

  /*!
    Perform the move.
  */
  // XXX make pure virtual
  virtual void move() { cout << "Nothing\n"; }

  /*!
    Reject the move. Decrease step_size. Restore backup.
    Moves must be rejected in reverse order!
  */
  virtual void reject() {
    step_size *= 1. - STEP_CHANGE;
  }

  /*!
    Accept the move. Increase step_size. Discard backup.
    Moves must be accepted in reverse order!
  */
  virtual void accept() {
    this->step_size *= 1. + this->STEP_CHANGE;
  }

};

/*!
  Change the float entry of anneal.nodes[addr] by a
  random number within (-step_size, +step_size).
*/
class SimpleFloatMove : public BasicMove {
public:

  float *node, old_value;

  /*!
    node - address of the node in anneal.
  */
  SimpleFloatMove(float *node, float step_size = 1e-6, float STEP_CHANGE = 0.01) :
    BasicMove(step_size,STEP_CHANGE) {
    this->node = node; 
    this->old_value = *node;
  }

  virtual ~SimpleFloatMove() {}

  void move() {
    old_value = *node;
    // cout << "Before:" << *node << endl;
    *node += step_size * (2.*rand01()-1.);
    // cout << "After:" << *node << endl << flush;
  }

  void reject() {
    BasicMove::reject();
    *node = old_value;
  }

  void accept() {
    BasicMove::accept();
    old_value = *node;
  }
};

// Extract strings
#define extract(Var,params) if (params[#Var].compare("")) Var = params[#Var]
#define extract2(Var,Str,params) if (params[Str].compare("")) Var = params[Str]

// Extract integers
#define extract_i(Var,params) if (params[#Var].compare("")) Var = atoi(params[#Var].c_str())
#define extract_i2(Var,Str,params) if (params[Str].compare("")) Var = atoi(params[Str].c_str())

// Extract floats
#define extract_f(Var,params) if (params[#Var].compare("")) Var = atof(params[#Var].c_str())
#define extract_f2(Var,Str,params) if (params[Str].compare("")) Var = atof(params[Str].c_str())


class BasicAnneal {
public:

  float min_step, max_step;
  int   min_move, max_move;
  float Temp, Cooling;
  float best_energy, curr_energy, candidate_energy;

  vector<BasicMove*> curr_moves, possible_moves;

  int success, trys;

  int log_counter, log_freq;

  string best_filename;

  BasicAnneal(const char *params = "") {
    this->std_init(params);
  }  

  virtual ~BasicAnneal() {}

  static void str2hash(string str, map<string,string> &params) {
    vector<string> tokens;
    vector<string>::iterator it;

    Tokenize2(str, tokens);
    for (it=tokens.begin();it!=tokens.end();it+=2)
      params[*it] = *(it+1);
  }

  /*!
    Parse params string and initialize defaults.
  */
  void std_init(const char* params = "") {
    min_step=max_step = 666;
    min_move=max_move = 666;
    Temp = 0.1;
    Cooling = 0.00001;

    curr_moves.clear();
    possible_moves.clear();

    success = 0;
    trys    = 1;

    log_counter = 0;
    log_freq = 10000;

    best_filename = string("bla.bla");

    map<string,string> param_map;
    str2hash(params, param_map);

    extract_f2(Temp,"T",param_map);
    extract_f2(Cooling,"C",param_map);
    extract_i2(log_freq,"logfreq",param_map);
    extract(best_filename,param_map);
  }

  /*!
    Show the configuration parameters.
  */
  virtual ostream & show_config(ostream &out) {
    out << "Current Energy: " << curr_energy << endl
        << "possible_moves.size(): " << possible_moves.size() << endl 
        << "Temperature (T): " << Temp << endl
        << "Cooling (C): " << Cooling << endl
        << "Log frequency (logfreq): " << log_freq << endl
        << "best_filename: "<< best_filename << endl;
    return out;
  }

  /*!
    Accept all current moves.
  */
  virtual void accept_curr_moves() {
    BasicMove* m;
    while (curr_moves.size() > 0) {
      m = curr_moves.back();
      curr_moves.pop_back();
      m->accept();
    }
    success += 1;
    trys    += 1;
  }

  /*!
    Reject all current moves.
  */
  virtual void reject_curr_moves() {
    BasicMove* m;
    while (curr_moves.size() > 0) {
      m = curr_moves.back();
      curr_moves.pop_back();
      m->reject();
    }
    trys    += 1;
  }

  /*!
    Make random moves and store them in curr_moves.
  */
  virtual void wiggle() {
    int r;
    if (possible_moves.size()>0) {
      r = randint(possible_moves.size());
      possible_moves[r]->move();
      curr_moves.push_back(possible_moves[r]);
    }
    else {
      cerr << "BasicAnneal::wiggle : No available moves.\n";
    }
  }

  /*!
    Returns true when annealing considers to have
    converged.
  */
  virtual bool stop() {
    return false; // we never stop
  }

  /*!
    The energy we want to optimize.
  */
  virtual float energy() {
    return 1.;
  }
 
  /*!
    This gets called each time anneal finds a new
    best energy.
  */
  virtual void best_found() {
    cout << "!" << best_energy << "!";
  }
 
  virtual void update_minmax_step() {
    min_step = max_step = possible_moves[0]->step_size;
    min_move = max_move = 0;
    for (uint i=1;i<possible_moves.size();++i) {
      if (possible_moves[i]->step_size < min_step) {
        min_step = possible_moves[i]->step_size;
        min_move =i;
      }
      if (possible_moves[i]->step_size > max_step) {
        max_step = possible_moves[i]->step_size;
        max_move =i;
      }
    }
  }

  virtual ostream& logline(ostream &out) {
    update_minmax_step(); 
    out << log_counter
        << " T=" << Temp
        << " E=" << curr_energy
        << " Emin=" << best_energy
        << " S=" << (float) success/(float) trys
        << " min/max=" << min_step << "/" << max_step;
    trys = 1;
    success = 0;

    return out;
  }

  virtual void log(){
    logline(cout);
    cout << endl << flush;
  }

  virtual void do_anneal() {
    best_energy      = energy();
    curr_energy      = best_energy;
    candidate_energy = best_energy;
    show_config(cout) << flush;

    while (!stop()) {
      if (log_counter % log_freq == 0) log();
      ++log_counter;
      wiggle();
      candidate_energy = energy();
      if (candidate_energy < best_energy) { 
        best_energy = candidate_energy;
        curr_energy = candidate_energy;
        best_found();
        accept_curr_moves();
      }
      else {
        if (rand01() <= exp(-(candidate_energy-curr_energy)/Temp)) {
          curr_energy = candidate_energy;
          accept_curr_moves();
          // print "+",
        }
        else {
          candidate_energy = curr_energy;
          reject_curr_moves();
          // print "-",
        }
      }
      Temp *= (1.-Cooling);
    }
    cout << "Done" << endl << flush;
  }
};

class SampleAnneal : public BasicAnneal {

  static float dist(float x1, float x2,
                    float y1, float y2) {
    return sqrt((x1-y1)*(x1-y1)+(x2-y2)*(x2-y2));
  }

public:

  int no_of_nodes, N;
  vector<float> nodes, best_nodes;

  /*!
     params  -  like BasicAnneal. 
                Addionally 
                N -  Number of points 
  */
  SampleAnneal(const char* params = "") {

    std_init(params);
    N = 20;

    map<string,string> param_map;
    str2hash(params, param_map);
    extract_i(N,param_map);

    no_of_nodes = 2*N;

    for (int i=0;i<no_of_nodes;++i)
      nodes.push_back(rand01()-.5);

    best_nodes = nodes;

    for (int i=0;i<no_of_nodes;++i)
      possible_moves.push_back(new SimpleFloatMove(&(nodes[i]),0.1));

  }

/*
  void std_init(const char* params = "") {
    BasicAnneal::std_init(params);
  }
*/

  bool stop() {
    if (Temp < 0.0005/N)
      return true;
    return false;
  }

  void best_found() {
    best_nodes = nodes;
    BasicAnneal::best_found();
    ofstream out(best_filename.c_str());
    for (int i=0;i<N;++i)
      out << nodes[2*i] << " " << nodes[2*i+1] << endl;
    out.close();
  }

  float energy() {
    float e = 0.0;
    for (int i=0;i<N;++i) {
      for (int j=0;j<N;++j) {
         e += pow(2.-dist(nodes[2*i],nodes[2*i+1],nodes[2*j],nodes[2*j+1]),2);
      }
    }
    return e;
  }
};
 
/*
def main(N):
  print "%d points starting" % N
  a = SampleAnneal("N=%d" % N)
  a.best_filename = "bla%03d.bla" % N
  a.Temp = 0.001 * N
  a.Cooling = 0.001/float(N)
  a.do_anneal()
  a.nodes = a.best_nodes
  a.best_found(True)

if __name__ == "__main__":
   import sys
   main(int(sys.argv[1]))
*/

#ifdef MAIN
int main(int argc, char** argv) {

  SampleAnneal* sa;
  const char* def = "T=0.001,N=200,best_filename=best.txt";

  if (argc>1) sa = new SampleAnneal(argv[1]);
  else {
    cout << "Use default params : " << def << endl;
    sa = new SampleAnneal(def);
  }

  sa->do_anneal();

  // we're done? set center of mass at 0,0
	float x = 0, y = 0;
	for (int i=0;i<sa->no_of_nodes/2;++i) {
    x += sa->nodes[2*i];
		y += sa->nodes[2*i+1];
	}
	x /= (float)(sa->no_of_nodes/2);
	y /= (float)(sa->no_of_nodes/2);

	for (int i=0;i<sa->no_of_nodes/2;++i) {
    sa->nodes[2*i]   = sa->nodes[2*i]   - x;
		sa->nodes[2*i+1] = sa->nodes[2*i+1] - y;
	}

	sa->best_found();

  return 0;
}
#endif

#endif //_ANNEAL_H_
