#include "../../experimental/annealing/my_anneal.cpp"
#include <algorithm>

/*!
	\defgroup FittingProblemGroup fitting
	\ingroup AnnealingGroup
	\class Vec2
	\ingroup FittingProblemGroup
	\brief 2D Vector class.
*/
class Vec2 {
public:
  float x, y;

  Vec2() { x=0; y=0; }
  Vec2(const float _x, const float _y) { x = _x; y = _y; }
  Vec2(const Vec2& v) { x = v.x; y = v.y; }

  Vec2 operator+(const Vec2& v) const {
    return Vec2(v.x + x, v.y + y);
  }

  Vec2 operator-(const Vec2& v) const {
    return Vec2(x - v.x, y - v.y);
  }

  bool operator<(const Vec2& v) const {
    return (this->x < v.x);
	}

  // ang in rad
  Vec2 rot(const float ang) const {
    return Vec2(cos(ang)*x - sin(ang)*y,
                sin(ang)*x + cos(ang)*y);
  }

  friend ostream &operator<<(ostream &out, const Vec2 &v);
};

inline ostream &operator<<(ostream &out, const Vec2 &v) {
  out << v.x << " " << v.y;
  return out;
}

/*!
  \class FittingAnneal
	\ingroup FittingProblemGroup
	\brief Fit a curve through given points.

	Here we try to fit a monotonic function through
	a given set of points with periodic boundary
	conditions at both sides (x and y).
*/
class FittingAnneal : public BasicAnneal {
public:

  string resume_from;
	int resample;
  int no_of_nodes;
	// in nodes we store the offsets (we want a monotone function)
  vector<Vec2> orig, nodes, best_nodes;

  // XXX it's not a real interpolation. as soon as we passed the right
	// spot we return the y value there.
  float interpolate(float s) {
		float y = nodes[0].y, xim;
		int i = 1;
    while (s > nodes[i].x  && i < no_of_nodes) {
			y += fabs(nodes[i].y);
			++i;
		}
	  return (y + (s - nodes[i-1].x)/(nodes[i].x - nodes[i-1].x)*(fabs(nodes[i].y)));
	}

	void dumpVectors() {
		cerr << "Orig\n";
		for (uint i=0;i<orig.size();++i)
			cerr << orig[i].x << " " << orig[i].y << endl;

		cerr << "Fit nodes\n";
		for (int i=0;i<no_of_nodes;++i)
			cerr << nodes[i].x << " " << nodes[i].y << endl;

		cerr << "Fit nodes 2\n";
		for (int i=0;i<no_of_nodes;++i)
			cerr << nodes[i].x << " " << interpolate(nodes[i].x) << endl;
	}

  ostream &show_config(ostream &out) {
		out << "CONFIG PARAMETERS :\n";
    BasicAnneal::show_config(out);
		out << "no_of_nodes=" << no_of_nodes << endl
		    << "resume_from=" << resume_from << endl
				<< "resample=" << resample << endl;
		return out;
	}

  void log() {
    logline(cout);
		cout << endl;
		/*
		char file[1024];
		sprintf(file,"%010d",log_counter);
		ofstream out(file);
		float y = 0;
		for (int i=0;i<no_of_nodes;++i) {
			y += fabs(nodes[i].y);
			if (y>1.)
				out << nodes[i].x << " " << y-1. << endl;
			else
				out << nodes[i].x << " " << y << endl;
		}
		out.close();
		*/
	}

  /*!
     params  -  like BasicAnneal. 
                Addionally 
								filename    -  curve to be fitted
                no_of_nodes -  Number of points
								resume_from -  File containing a previous run.
								resample    -  If we resume from a file. Resample it with that num of points.
  */
  FittingAnneal(const char *filename, const char* params = "") {

    std_init(params);

    resume_from = string("");
    no_of_nodes = 600;
		resample = -1;

    map<string,string> param_map;
    str2hash(params, param_map);
    extract_i(no_of_nodes,param_map);
		extract_i(resample,param_map);
		extract(resume_from,param_map);

    cout << "no_of_nodes " << no_of_nodes << endl;

    // Load points to fit from file
		ifstream in(filename,ios::in);
		float s, t, dummy;
    while(in >> s >> t >> dummy) {
			if (s>t)
				orig.push_back(Vec2(s,t+1.));
			else
        orig.push_back(Vec2(s,t));
		}
    // sort this vector
		in.close();
    sort(orig.begin(), orig.end());

    // Init fitting nodes
		if (resume_from.size()!=0) {
			cout << "RESUME from " << resume_from << endl;
      in.open(resume_from.c_str(),ios::in);
		  float s, t0, t;
			in >> s >> t0;
			nodes.push_back(Vec2(s,t0));
      while(in >> s >> t) {
				if (s>t) t += 1.;
        nodes.push_back(Vec2(s,t-t0));
				t0 = t;
			}
  		in.close();
			no_of_nodes = nodes.size();

			if (resample>0) {
				cout << "RESAMPLE " << resample << endl;
        vector<Vec2> tmp;
				float x= 0.0;
				float dy = interpolate(x);
				float y, yold = dy;
				tmp.push_back(Vec2(x,dy));
				for (int i=1;i<resample;++i) {
          x = (float)i/(float)(resample-1);
					y = interpolate(x);
					dy = yold - y;
					yold = y;
					tmp.push_back(Vec2(x,dy));
				}
				nodes = tmp;
				no_of_nodes = resample;
        for (int i=1;i<resample;++i) {
          x = (float)i/(float)(resample-1);	
					cerr << x << " " << interpolate(x) << " " << nodes[i].x << " " << nodes[i].y << endl;
				}
			}
		}
		else {
      nodes.push_back(Vec2(0,orig[0].y));
			// We wrap y only for the output!
      for (int i=1;i<no_of_nodes;++i) {
				float x = (float)i/(float)(no_of_nodes-1), y = 0; // (float)1/(float)(no_of_nodes-1);
        nodes.push_back(Vec2(x,y));
      }
		}

    best_nodes = nodes;

    for (int i=0;i<no_of_nodes;++i)
			// XXX moves should be > 0
      possible_moves.push_back(new SimpleFloatMove(&(nodes[i].y),0.01));

  }

/*
  void std_init(const char* params = "") {
    BasicAnneal::std_init(params);
  }
*/

  bool stop() {
		//if (log_counter==20000) return true;
		// XXX I need a better criterium
		//     if 2 boxes are stuck (let's say finished)
    if (Temp < 0.00001/(float)no_of_nodes) return true;
    // if (min_step<1e-12 || max_step > 1e12 ) return true;
    return false;
  }

  void best_found() {
    best_nodes = nodes;
    BasicAnneal::best_found();
		ofstream out(best_filename.c_str());
		float y = 0;
    for (int i=0;i<no_of_nodes;++i) {
			y += fabs(best_nodes[i].y);
			while (y>1.) y-=1.;
      out << best_nodes[i].x << " " << y << endl;
		}
    out.close();

		// Fix the step sizes if it gets tooo large
		for (uint i=0;i<possible_moves.size();++i) {
      if (possible_moves[i]->step_size > 100) possible_moves[i]->step_size = 100;
		}
		update_minmax_step();
  }

  /*!
	 The energy is \f$\sum_{i=1}^N (y_i - inter(f(x_i)))^2\f$, where
	 y_i are the sample points, inter is a linear interpolation of the
	 fitted points we currently have.
	*/
  float energy() {
    float e = 0, val, in;
    for (uint i=0;i<orig.size();++i) {
			in = interpolate(orig[i].x);
      val = orig[i].y - in;
			//cout << orig[i].y << " " << in << " val= " << val << endl;
      e += (val*val);
		}
		// penalize if first and last point are too far apart
		float endpoint = interpolate(1.)-1.;
		e += exp(fabs(nodes[0].y - endpoint));
		return e;
  }
};

// #define TEST
#ifndef TEST
int main(int argc, char** argv) {

  srand(time(NULL));

  FittingAnneal* ba;

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " tointerpolate.txt params\n";
    exit(0);
  }

  ba = new FittingAnneal(argv[1], argv[2]);
	
  float e = ba->energy();
	cout << "Energy " << e << endl;

  ba->do_anneal();

	// write that down
	ba->best_found();

  return 0;
}

#else
int main() {

  vector<Vec2> orig;
  // Load points to fit from file
  ifstream in("test.txt",ios::in);
	float s, t;
  while(in >> s >> t)
    orig.push_back(Vec2(s,t));
  // sort this vector
	in.close();
  sort(orig.begin(), orig.end());

  for (int i=0;i<orig.size();++i)
		cout << orig[i].x << " " << orig[i].y << " " <<  << endl;

}
#endif
