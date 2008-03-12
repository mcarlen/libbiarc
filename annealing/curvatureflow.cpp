#include "../include/Curve.h"
#include "../include/algo_helpers.h"

#define it vector<Biarc<Vector3> >::iterator

float TOL = 0.01;
int LogFreq = 5000;

float r(float l,float u) {
  return l+(float)rand()/RAND_MAX*(u-l);
}

float arclength_between(it b0, it b1,Curve<Vector3> *c) {
  float s = 0.0;
  it from = b0, to = b1;
  while(from!=to) {
    s += from->biarclength();
    from++; if (from == c->end()) from = c->begin();
  }
  return s;
}

/*!
  Global radius of curvature check. Tests wheter grc is
  constant (within tolerance tol) at every biarc!
*/
int check_grc(Curve<Vector3> *c, float tol) {
  it b = c->begin();
  float grc = c->radius_global(*b);
  b++;
  for (;b!=c->end();++b) {
    if (fabsf(c->radius_global(*b)-grc)/grc > tol)
      return 0;
  }
  return 1;
}

int main(int argc,char** argv) {

  Curve<Vector3> c(argv[3]);
  c.link();
  c.make_default();

  // Good parameters :
  // delta = 0.1, thick_scale = 0.1;

  // When resolving an overlap, how much should we separate the points!
  float delta = atof(argv[1]);
  // Curvature direction move scale (scale thickness whith it)!
  float thick_scale = atof(argv[2]);

  float thickness = c.thickness_fast();
  float length    = c.length();
  float arclen_diff;

  float scale     = thickness*thick_scale;

  float pp_dist;

  Vector3 nv,direc;

  float ropelength_old = 1e99;

  cout << "L=" << length << ",D=" << thickness << endl;
  int n = 0;
  char buf[1000];

  int ALREADY_SET = 0;

  while (true) {

    // Displace points according to their local curvature
    for (it b=c.begin();b!=c.end();++b) {
      nv     = c.normalVector(b);

      b->setPoint(b->getPoint()+scale*nv);

      // Not midpoint rule. But we do not really care!
      // here we recompute the cache as well!
      b->make(.5);
    }

    int OVERLAPS = 1;
    // Cache neighbors!!!
    while(OVERLAPS>0) {
      OVERLAPS=0;
      for (it b=c.begin();b!=c.end();++b) {
        for (it bp=c.begin();bp!=c.end();++bp) {
          if (bp==b) continue;
          arclen_diff = arclength_between(bp,b,&c);
          if (length-arclen_diff<arclen_diff)
            arclen_diff = length - arclen_diff;
          if (arclen_diff > M_PI*.5*thickness) {
            direc   = (b->getPoint()-bp->getPoint());
            pp_dist = direc.norm();
            direc /= pp_dist;
            if (pp_dist < thickness) {
              // Intersection. We put the point back! We even move
              // it further away to change the knot confirmation
              b->setPoint(b->getPoint()-scale*nv);
              direc.normalize();
              b->setPoint(b->getPoint()+(.5*(thickness-pp_dist)+scale*delta)*direc);
              bp->setPoint(bp->getPoint()-(.5*(thickness-pp_dist)+scale*delta)*direc);

              b->make(.5);
              bp->make(.5);
              OVERLAPS++;
              break;
            }
          }
        }
      }
    }
    ropelength_old = length/thickness;
    c.computeTangents();
    c.make_default();
    c.center();
    c.normalize();

    thickness = c.thickness_fast(); length = c.length();
    scale = thick_scale*thickness;
    
    if (n%LogFreq == 0) {
      cout << "Log " << n << " ( " << length/thickness << " )\n" ;
      sprintf(buf,"/tmp/%08d",n);

//    Resampling might help in notting getting stuck
//    in a local min
//    c.resample((int)(length/thickness));
      if (check_grc(&c,TOL)) {
        c.writePKF("best.pkf");
        cout << "Finished!\n";
        return 0;
      }
      c.writePKF(buf);
    }
    n++;
  }

  return 0;

}
