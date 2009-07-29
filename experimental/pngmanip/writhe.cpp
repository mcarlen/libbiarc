#include "png.h"
#include "Curve.h"
#include "algo_helpers.h"
#include "colors.h"
#include "pngwrite.h"

#ifdef COMPUTE_IN_R4
#include "Vector4.h"
#include "Matrix4.h"
#define TVec Vector4
#else
#define TVec Vector3
#endif

unsigned int i,j,w,h;
unsigned char *px, *src;
char outname[40];
int size = 500;
float length,thickness,alen,ptmin,ptmax,cur;
Curve<TVec> *curve;

// Zoom in on some rectangular region
float fromx,fromy,tox,toy;
int HEIGHTMAP;

// Global variable for open/closed curve
// default is closed
int CLOSED;

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
  float **pttable;
  pttable = new float*[h];
  for (int zz=0;zz<h;zz++) pttable[zz] = new float[w];
  if (pttable == NULL) { cout << "Mem alloc prob!\n";
    exit(5);
  }
  // Here we use the thickness only as a scaling factor, thus
  // perfect accuracy is not that important
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
			TVec from = Pts_i[i], to = Pts_j[j];
			TVec tfrom = Tg_i[i], tto = Tg_j[j];

      if (fabs(i-j)<2) cur = 0;
			else {
				float len = (from-to).norm();
			  cur = (from-to).dot(tfrom.cross(tto))/pow(len,3);
			}

//			if (cur<0) cur = 0.0;
			// if (cur>1. || (from-to).norm()<1e-7) cur = 1.;

			ptmin = (cur<ptmin?cur:ptmin);
			ptmax = (cur>ptmax?cur:ptmax);

			pttable[i][j] = cur;
		}
	}

  for (int j=0;j<h;j++) {
    src = px + j*(3*w);
    for (int i=0;i<w;i++) {
      if (!HEIGHTMAP)
        map_color_sine_end(&c,pttable[i][j],ptmin,ptmax);
//        map_color_sine_acc(&c,pttable[i][j],0.0,ptmax);
      else
        height_map(&c,pttable[i][j],ptmax,ptmax);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  cout << "Min/Max   : " << ptmin << " / " << ptmax << endl;
  cout << "Thickness : " << thickness << endl;

  sp_png_write_rgb("out.png",px,w,h,0.1,0.1,3*w);

  delete px; delete curve;

	// Compute writhe double integral
	float writhe = 0;
	float ds = 1./(float)w;
	float dt = 1./(float)h;
	float dsdt = ds*dt;
	for (int i=0;i<w;++i)
		for (int j=0;j<h;++j)
      writhe += pttable[i][j]*dsdt;
	cout << "Write : " << writhe/(4.*M_PI) << endl;
}

void usage(char* prog) {
    cout << "Usage : " << prog << " [option] <pkf file>\n";
    cout << "\n  -res N\tN is the resolution of the plot. Default 500\n"
         << "  -hm   \tHeighmap gradient (black&white)\n"
         << "  -xrange x0 x1\tZoom in [x0,x1]\n"
         << "  -yrange y0 y1\tZoom in [y0,y1]\n"
         << "  -open \tdefault is closed curves. Treat it as open\n"
         << "  -h    \tThis help\n";
    exit(-1);
}

int main(int argc,char **argv) {

  if (argc<2) {
    usage(argv[0]);
  }

  int Res = 500;
  char *cname = NULL;
  HEIGHTMAP = 0;
  CLOSED = 1;
  fromx = 0.0; tox = 1.0;
  fromy =0.0; toy = 1.0;

  int arg = 1;
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

  cout << "Square X : " << fromx << " to " << tox << endl
       << "       Y : " << fromy << " to " << toy << endl;

  cout << "Start plot with Resolution " << Res;
  DoPlot(cname,Res,Res);
  cout << "\t[OK]\n";

}
