#include "Curve.h"
#include "my_anneal.cpp"
#include "algo_helpers.h"

// XXX we do not use the 2nd symmetry generator (rotation about x-Axis).
// this means we could only wiggle on angles between 1,0,0 and location
// where curve goes to another torus. since rotation about 1,0,0 projects
// the side let's say to the left of 1,0,0 to its right.
//
// try to find the xoffset analytically in function of the tilt angle.

// Torus with both radii = 1
// This torus has an offset of z_dist from the z axis
// and is tilted with angle tilt around the x axis
// u=0,v=0 is vector 1,0,0
inline Vector3 torus(float u, float v, float tilt, float xoffset) {
  Vector3 vec((1+cos(v))*cos(u), (1+cos(v))*sin(u), sin(v));
  Matrix3 mat;
  mat.rotAround(Vector3(1,0,0), tilt);
  return (mat*vec+Vector3(xoffset,0,0));
}

/*!
  \class TrefoilTorusAnneal
	\ingroup TrefoilTorusGroup
	\brief Anneal a trefoil running on 3 torii.

*/
class TrefoilTorusAnneal : public BasicAnneal {
  float best_xoffset, best_tilt;
  vector<float> best_angles;
public:

  int N;
  float xoffset, tilt;
  vector<float> angles;

  /*!
     params  -  like BasicAnneal. 
                Addionally 
                N       - Number of angles
                tilt    - Torus tilt angle
                xoffset - Distance from z-Axis in x direction
  */
  void std_init(const char* params = "") {
    BasicAnneal::std_init(params);

    N = 30;
    xoffset = 1.0;
    tilt = M_PI/6.;

    map<string,string> param_map;
    str2hash(params, param_map);
    extract_i(N,param_map);
    extract_f(xoffset,param_map);
    extract_f(tilt,param_map);
  }


  TrefoilTorusAnneal(const char* params = "") {

    std_init(params);
    for (int i=0;i<N;++i) {
      angles.push_back((2.*(float)rand()/(float)RAND_MAX-1.)/100.);
    }

    // init moves on angles
    for (int i=0;i<N;++i) {
      possible_moves.push_back(new SimpleFloatMove(&(angles[i]),0.001));
    }
    // move on tilt angle
    possible_moves.push_back(new SimpleFloatMove(&tilt,0.001));
    // move on xoffset
    possible_moves.push_back(new SimpleFloatMove(&xoffset,0.001));

  }

  /*!
	  Reimplemented stop criterion.
	*/
  bool stop() {
		// XXX I need a better criterium
		//     if 2 boxes are stuck (let's say finished)
    if (Temp < 0.00001/N) return true;
    // if (min_step<1e-12 || max_step > 1e12 ) return true;
    return false;
  }

  void best_found() {
    best_angles = angles;
    best_tilt = tilt;
    best_xoffset = xoffset;
    BasicAnneal::best_found();
		ofstream out(best_filename.c_str());
    out << N << " " << tilt << " " << xoffset << endl;
    for (int i=0;i<N;++i)
      out << angles[i] << endl;
    out.close();
  }

  /*!
    Build PKF curve and compute its thickness with the parameters for
    the 3 torii.
	*/
  float energy() {
    Curve<Vector3> c;
    for (int i=0;i<N;++i) {
      float t = -5.*M_PI/6. + (float)i/(float)(N-1)*5.*M_PI/6.;
      c.append(torus(t, angles[i], tilt, xoffset), Vector3());
    }

    // x-Axis symmetrie gives us 2nd part of this ear
    // leave out the first and last point since they
    // get completed by rotation
    for (int i=N-2;i>0;--i) {
      c.append(c[i].getPoint().rotPtAroundAxis(M_PI,Vector3(1,0,0)),Vector3());
    }

    // Take this piece of curve, rotate it twice by 120 degrees
    // and append it to get the final trefoil.
    Curve<Vector3> c2 = c;
    c2.rotAroundAxis(-2.*M_PI/3., Vector3(0,0,1));
    for (int i=0;i<c2.nodes();++i)
      c.append(c2[i]);
    c2.rotAroundAxis(-2.*M_PI/3., Vector3(0,0,1));
    for (int i=0;i<c2.nodes();++i)
      c.append(c2[i]);

    c.link();
    c.computeTangents();

    char buf[1024];
    sprintf(buf,"%08d.pkf",log_counter);

    if (log_counter%100==0)
      c.writePKF(buf);

    c.make_default();

    float e= c.length()/c.thickness();
    return e;
  }
};

// #define TEST
#ifndef TEST
int main(int argc, char** argv) {

  if (argc!=2) {
    cout << "Usage : " << argv[0] << " params\n";
    exit(0);
  }

  TrefoilTorusAnneal anneal(argv[1]);
  anneal.show_config(cout);
  anneal.do_anneal();

  return 0;
}
#else
int main() {
  Curve<Vector3> c;
  for (int i=0;i<100;++i)
    c.append(torus((float)i/99.*2.*M_PI, 0, 0, 0),Vector3());
  c.computeTangents();
  c.writePKF("out.pkf");
  return 0;
}
#endif
