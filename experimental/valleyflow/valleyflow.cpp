#include <stdlib.h>

#include "png.h"
#include "Curve.h"
#include "algo_helpers.h"
#include "../pngmanip/colors.h"
#include "../pngmanip/pngwrite.h"

#ifdef COMPUTE_IN_R4
#include "Vector4.h"
#define TVec Vector4
#else
#define TVec Vector3
#endif

#define TEST

unsigned int i,j,w,h;
unsigned char *px, *src;
char outname[40];
int size = 500;
int Res = 500;
float length,thickness,alen,ptmin,ptmax,cur;
Curve<TVec> *curve;

// Zoom in on some rectangular region
float fromx,fromy,tox,toy;
int HEIGHTMAP;

// Global variable for open/closed curve
// default is closed
int CLOSED;
float **pttable;

struct Particle {
  float x,y;
};

typedef Particle Particle;
typedef Particle Position;

inline float dist2(const Particle& p1, const Particle& p2) {
  float dx,dy;
  dx = (p1.x-p2.x);
  dy = (p1.y-p2.y);
  return dx*dx+dy*dy;
}


// Heuristic, this is the pixel distance allowed
// between 2 trajectories
float TrajectoryConstant = 0.001;

class Trajectory {
  static unsigned int num_traj_saved;
public:

  static void init() {
    Trajectory::num_traj_saved = 0;
  }

  vector<Particle> ps;
  // Trajectory weight number
  int n;

  Trajectory(const Trajectory &t) : ps(t.ps), n(t.n) {}

  Trajectory& operator=(const Trajectory &t) {
    this->ps = t.ps;
    this->n = n;
    return *this;
  }

  // weighted sum of another traj with this one
  Trajectory& operator+=(const Trajectory &t) {

    // Change weigth number
    ++n;

    float fac1 = (float)(n-1)/(float)n;
    float fac2 = 1./(float)n;
    unsigned int min_size = min(ps.size(),t.ps.size());
    for (unsigned int i=0;i<min_size;++i) {
      ps[i].x = (fac1*ps[i].x+fac2*t.ps[i].x);
      ps[i].y = (fac1*ps[i].y+fac2*t.ps[i].y);
    }

    if (t.ps.size()>min_size)
      for (unsigned int i=min_size;i<t.ps.size();++i)
        ps.push_back(t.ps[i]);

    return *this;
  }

  // weighted sum of another traj with this one
  Trajectory operator+(const Trajectory &t) {
    Trajectory sum;

    ++n;

    float fac1 = (float)(n-1)/(float)n;
    float fac2 = 1./(float)n;
    unsigned int min_size = min(ps.size(),t.ps.size());
    for (unsigned int i=0;i<min_size;++i)
      sum.append(fac1*ps[i].x+fac2*t.ps[i].x,
                 fac1*ps[i].y+fac2*t.ps[i].y);

    if (t.ps.size()>min_size)
      for (unsigned int i=min_size;i<t.ps.size();++i)
        sum.ps.push_back(t.ps[i]);

    return sum;
  }


  float norm(const Trajectory& t) {
    float norm = 0;
    // XXX maybe we should not ignore the "longer" part
    int min_size = min(ps.size(),t.ps.size());
    for (int i=0;i<min_size;++i)
      norm += dist2(ps[i],t.ps[i]);
    return norm/(float)(min_size*min_size);
  }


  // Init n = 2, thus the first curve summed with this
  // one gives : 1/2*this + 1/2*other
  Trajectory() { n = 2; }
  ~Trajectory() { ps.clear(); }

  void clear() { ps.clear(); }

  void append(float x, float y) {
    Particle p; p.x = x; p.y = y;
    ps.push_back(p);
  }

  friend ostream& operator<<(ostream &out, const Trajectory t);

  void save() {
    ofstream out; char buf[1024];
    sprintf(buf,"%04d.part",Trajectory::num_traj_saved++);
    out.open(buf);
    out << *this << endl;
    out.close();
  }
};

inline ostream& operator<<(ostream &out, const Trajectory t) {
  for (unsigned int j=0;j<t.ps.size();++j) {
    out << t.ps[j].x << " " << t.ps[j].y << endl;
  }
  return out;
}

#define MAX_ITER 100
// run a trajectory from starting point sp through
// gradient fiel pttable. stop it when we are
// in the vicinity of the circle at p with radius rad
// or break if too many iterations
// Trajectory t needs to have at least an initial point!
bool traj_run(Trajectory &t, Position pos, float rad) {
  assert(t.ps.size()>0);
  int step = 0;
  float vel = 1;
  float gx, gy, norm;
  int px, py;
  Particle part, part_new;

  while((dist2(t.ps.back(),pos)>rad*rad) && (step++ < MAX_ITER)) {
    part = t.ps.back();
    // compute grad
    px = (int)(part.x); py = (int)(part.y);
    if (px>Res-2) gx = 0;
    else
      gx = pttable[px+1][py]-pttable[px][py];
    if (py>Res-2) gy = 0;
    else
      gy = pttable[px][py+1]-pttable[px][py];

    cout << "px="<<px<<",py="<<py<< ",gx="<<gx << ",gy=" << gy << endl << flush;

    norm = sqrt(gx*gx+gy*gy);
    gx /= norm; gy /= norm;
    // run down the gradient
    part_new.x = part.x - gx*vel;
    part_new.y = part.y - gy*vel;

    if (part_new.x>=Res) part_new.x = Res-1;
    if (part_new.y>=Res) part_new.y = Res-1;

    if (part_new.x<0) part_new.x = 0;
    if (part_new.y<0) part_new.y = 0;

    t.ps.push_back(part_new);
  }
  if (step<MAX_ITER)
    return true;
  return false;
}

void plot(const char* name, float ptmax = 1.0) {
//  HEIGHTMAP = 1;
  unsigned char *src;
  RGB c;
  px = new unsigned char[3*Res*Res];
  for (int j=0;j<Res;j++) {
    src = px + j*(3*Res);
    for (int i=0;i<Res;i++) {
      if (!HEIGHTMAP)
        map_color_sine_end(&c,pttable[i][j],0.0,ptmax);
      else
        height_map(&c,pttable[i][j],0.,ptmax);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }

  sp_png_write_rgb(name,px,Res,Res,0.1,0.1,3*Res);

  delete[] px; 
}

void grad_field1() {
  float x,y,gradmax = 0;
  for (int i=0;i<Res;++i) {
    x = (float)i/(float)Res*2.-1.;
    for (int j=0;j<Res;++j) {
      y = (float)j/(float)Res;
      pttable[i][j] = (x-.1*sin(4.*M_PI*y))*(x-.1*sin(4.*M_PI*y))+1-y;
      if (pttable[i][j]>gradmax) gradmax = pttable[i][j];
    }
  }
  plot("field1.png",gradmax);
}

void grad_field2() {
  float x,y,gradmax = 0;
  for (int i=0;i<Res;++i) {
    x = (float)i/(float)Res*2-1;
    for (int j=0;j<Res;++j) {
      y = (float)j/(float)Res;
      pttable[i][j] = x*x+1-y;
      if (pttable[i][j]>gradmax) gradmax = pttable[i][j];
    }
  }
  plot("field2.png",gradmax);
}

void init_table() {
  // Allocate data table
  pttable = new float*[Res];
  for (int zz=0;zz<Res;zz++) pttable[zz] = new float[Res];
  if (pttable == NULL) { cout << "Mem alloc prob!\n";
    exit(5);
  }
}

// XXX use a float random. use subpixel precision
#define myrand() (rand()%Res)
#define NParticles 2
Particle *plist;
void seed_particles() {
//  cerr << "seed" << flush;
  plist = new Particle[NParticles];
  for (int i=0;i<NParticles;++i) {
    plist[i].y = 0;    // height
    plist[i].x = myrand();
    while (pttable[(int)(plist[i].x)][0]==0.0)
      plist[i].x = myrand();
  }
//  cerr << " ok\n" << flush;
}

void DoPlot(const char* name, int w, int h) {

  curve = new Curve<TVec>(name);
  if (curve==NULL) exit(10);
  if (CLOSED)
    curve->link();
  curve->make_default();
  px = new unsigned char[3*w*h];
  if (px == NULL) {
    cerr << "Memory alloc problem!\n";
    exit(5);
  }

  TVec pt,pt2;
  RGB c;

  // Allocate data table
  pttable = new float*[w];
  for (int zz=0;zz<w;zz++) pttable[zz] = new float[h];
  if (pttable == NULL) { cout << "Mem alloc prob!\n";
    exit(5);
  }
  // Here we use the thickness only as a scaling factor, thus
  // perfect accuracy is not that important
#warning "Computing with thickness_fast!"
  thickness = curve->thickness_fast();
  length = curve->length();
  ptmin = 10000;
  ptmax = -10000;

  float sigma;

  // make width and height dims different
  // i.e. Pts_i and Pts_j
  TVec Pts_i[w], Tg_i[w];
  TVec Pts_j[h], Tg_j[h];
  for (int j=0;j<h;j++) {
      sigma = fromy+(float)j/(float)h*
              (toy-fromy);

      alen = sigma*length;
      if (alen>length) {
        cout << alen << " - "; alen = length;
        cout << j << " -- " << (float)j/(float)h<<endl;
        
      }
      Pts_j[j] = curve->pointAt(alen);
      Tg_j[j] = curve->tangentAt(alen);
  }

  for (int i=0;i<w;i++) {
      sigma = fromx+(float)i/(float)w*
             (tox-fromx);

      alen = sigma*length;
      if (alen>length) {
        cout << alen << " - "; alen = length;
        cout << i << " -- " << (float)i/(float)w<<endl;
        
      }
      Pts_i[i] = curve->pointAt(alen);
      Tg_i[i] = curve->tangentAt(alen);
  }

    for (int j=0;j<h;j++) {
      for (int i=0;i<w;i++) {

        TVec from = Pts_i[i], to = Pts_j[j], tg = Tg_i[i];
        TVec D = to - from;

        float Dlen = D.norm();
        float cosa = tg.dot(D)/Dlen;
        float sina = sqrt(1.0-cosa*cosa);
        
        if (sina==0) cur = 0.0;
        else cur = thickness*sina/Dlen;

        if (cur<0.) cur = 0.;
        else if (cur>1.) cur = 1.;

        ptmin = (cur<ptmin?cur:ptmin);
        ptmax = (cur>ptmax?cur:ptmax);
        pttable[i][j] = cur;
      }
    }

  for (int j=0;j<h;j++) {
    src = px + j*(3*w);
    for (int i=0;i<w;i++) {
      if (!HEIGHTMAP)
        map_color_sine_end(&c,pttable[i][j],0.0,ptmax);
      else
        height_map(&c,pttable[i][j],0.,ptmax);
      if (pttable[i][j]<.99*ptmax) {
        set_rgb(&c,255,255,255);
        pttable[i][j] = 0;
      }
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
/*
  cout << "Min/Max   : " << ptmin << " / " << ptmax << endl;
  cout << "Thickness : " << thickness << endl;
*/

  sp_png_write_rgb("out.png",px,w,h,0.1,0.1,3*w);

  delete[] px; delete curve;
}

void usage(char* prog) {
    cout << "Usage : " << prog << " [option] <pkf file>\n";
    cout << "\n  -plot type\ttype is either pp, pt or tt.\n"
         << "  -res N\tN is the resolution of the plot. Default 500\n"
         << "  -hm   \tHeighmap gradient (black&white)\n"
         << "  -xrange x0 x1\tZoom in [x0,x1]\n"
         << "  -yrange y0 y1\tZoom in [y0,y1]\n"
         << "  -open \tdefault is closed curves. Treat it as open\n"
         << "  -h    \tThis help\n";
    exit(-1);
}

#ifndef TEST
int main(int argc,char **argv) {

/*
  if (argc<2) {
    usage(argv[0]);
  }
*/

  const char *cname = "test.pkf";
  HEIGHTMAP = 0;
  CLOSED = 1;
  fromx = 0.0; tox = 1.0;
  fromy =0.0; toy = 1.0;

  int arg = 1;
/*
  while (arg<=argc) {
    if (argv[arg][0] == '-') {
      if (!strncmp(&argv[arg][1],"hm",2)) {
        HEIGHTMAP = 1; ++arg;
      }
      else if (!strncmp(&argv[arg][1],"h",1)) {
        usage(argv[0]);
      }
      else if (!strncmp(&argv[arg][1],"res",3)) {
        Res = atoi(argv[arg+1]);
        ++arg;
        ++arg;
      }
      else if (!strncmp(&argv[arg][1],"open",4)) {
        CLOSED = 0;
        ++arg;
      }
      else if (!strncmp(&argv[arg][1],"plot",4)) {
        ++arg;
        if (!strncmp("pp",argv[arg],2)) {
          pType = PP_PLOT; ++arg;
        }
        else if (!strncmp("pt",argv[arg],2)) {
          pType = PT_PLOT; ++arg;
        }
        else if (!strncmp("tt",argv[arg],2)) {
          pType = TT_PLOT; ++arg;
        }
        else {
          cout << "Unsuported or unknown plot type\n";
          usage(argv[0]);
        }
      }
      else if (!strncmp(&argv[arg][1],"xrange",6)) {
        fromx = atof(argv[arg+1]);
        tox   = atof(argv[arg+2]);
        arg+=3;
      }
      else if (!strncmp(&argv[arg][1],"yrange",6)) {
        fromy = atof(argv[arg+1]);
        toy   = atof(argv[arg+2]);
        arg+=3;
      }
      else {
        cout << "Wrong options.\n";
        usage(argv[0]);
      }
    }
    else {
//    if (arg==0) arg++;
      cout << arg << endl;
      cname = argv[arg];
      cout << cname << endl;
      if (arg+1<argc) {
        fromx = atof(argv[arg+1]);
        tox = atof(argv[arg+2]);
        fromy = atof(argv[arg+3]);
        toy = atof(argv[arg+4]);
      }
      arg=argc+10;
    }
  }
*/
/*
  cout << "Square X : " << fromx << " to " << tox << endl
       << "       Y : " << fromy << " to " << toy << endl;
*/

  fromx = .45; tox = 1;
  fromy = 0;  toy = .55;

  DoPlot(cname,Res,Res);

//  seed_particles();

  plist = new Particle[NParticles];

  plist[0].x = 48;
  plist[0].y = 48;
  plist[1].x = 52;
  plist[1].y = 52;

  // set gradient manually for now
  for (int j=0;j<Res;++j) {
    for (int i=0;i<Res;++i) {
      pttable[i][j] = j; // +rand()%200;
    }
  }

/*
  cout << "Min/Max   : " << ptmin << " / " << ptmax << endl;
  cout << "Thickness : " << thickness << endl;
*/

  // send N particles down the gradient field (actually up)
  // and identify different paths
  vector<Trajectory> Paths;
#if 1
  Trajectory t1, t2;
  Position pos; pos.x = 45; pos.y = 100;

  int ParticlesDone = 0;
  // First trajectory
  t1.ps.push_back(plist[ParticlesDone]);
  ParticlesDone++;
  while (!traj_run(t1,pos,10.0)) {
    t1.ps.clear();
    t1.ps.push_back(plist[ParticlesDone]);
    ParticlesDone++;
  }
  Paths.push_back(t1);

  for (int i=ParticlesDone;i<NParticles;++i) {
    // put initial point
    t2.ps.clear();
    t2.ps.push_back(plist[i]);

    // run it

    if(!traj_run(t2, pos, 10.0)) continue;

    // compare the trajectories
    // XXX compare with all the previously found paths!
    float tn = Paths.back().norm(t2);
    cout << "Norm : " << tn;
    if (tn<TrajectoryConstant) {
      cout << " same path.\n";
      Paths.back()+=t2;
      cout << "Sum : " << Paths.back().n << endl;
    }
    else {
      cout << " NEW path.\n";
      Paths.push_back(t2);
    }
  }
#else
  Trajectory t; Position pos; pos.x = 45; pos.y = 100;
  t.ps.push_back(plist[0]);
  if (traj_run(t,pos,10.0))
    cout << "ok\n";
  exit(0);
#endif

  ofstream out; char buf[1024];
  for (unsigned int i=0;i<Paths.size();++i) {
    sprintf(buf,"%04d.part",i);
    out.open(buf);
    out << Paths[i] << endl;
    out.close();
  }

  for (int i=0;i<Res;++i)
    delete[] pttable[i];
  delete pttable;
  delete plist;

  Paths.clear();

  return 0;
}
#else
#warning "TEST Mode!!!!!"

// Declare static member outside class
unsigned int Trajectory::num_traj_saved;

int main() {

  Particle p1, p2;
  p1.x = 1; p1.y = 2;
  p2.x = 10; p2.y = 15;
 
//  cout << "dist2(p1,p2)=" << dist2(p1,p2) << endl;
  // Initialise static counter in Trajectory class
  Trajectory::init();

  Trajectory sinx, cosx, sinphase;

  // sin, cos and phase shifted sin trajectories
  float t;
  for (int i=0;i<100;++i) {
    t = (float)i/(float)100.;
    sinx.append(t,sin(2.*M_PI*t));
    cosx.append(t,cos(2.*M_PI*t));
    sinphase.append(t,sin(0.1+2*M_PI*t));
  }
 
  sinx.save();
  cosx.save();
  sinphase.save();
 
  Trajectory sum0 = sinx+cosx, sum1 = sinx+sinphase, sum2 = cosx+sinphase;

  sum0.save();
  sum1.save();
  sum2.save();

#define ok(a) (a<TrajectoryConstant?" Same path!":" Other path!")
  cout << "Constant : " << TrajectoryConstant << endl;
  cout << "sinx.norm(cosx)=" <<  ok(sinx.norm(cosx)) << endl;
  cout << "sinx.norm(sinphase)=" <<  ok(sinx.norm(sinphase)) << endl;
  cout << "cosx.norm(sinphase)=" <<  ok(sinx.norm(sinphase)) << endl;

  cout << "norm(sinx+cosx,sinx)=" << ok(sum0.norm(sinx)) << endl;
  cout << "norm(sinx+sinphase,sinx)=" << ok(sum1.norm(sinx)) << endl;
  cout << "norm(cosx+sinphase,sinx)=" << ok(sum2.norm(sinx)) << endl;

  init_table();
  grad_field1();
  grad_field2();

  return 0;

}
#endif
