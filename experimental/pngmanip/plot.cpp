#include "png.h"
#include "Curve.h"
#include "algo_helpers.h"
#include "colors.h"
#include "pngwrite.h"

#ifdef COMPUTE_IN_R4
#include "Vector4.h"
#define TVec Vector4
#else
#define TVec Vector3
#endif

enum PLOT_TYPE { PT_PLOT = 1, PP_PLOT, TT_PLOT };

unsigned int i,j,w,h;
unsigned char *px, *src;
char outname[40];
int size = 500;
float length,thickness,alen,ptmin,ptmax,cur;
Curve<TVec> *curve;

// Zoom in on some rectangular region
float fromx,fromy,tox,toy;

void DoPlot(const char* name, int w, int h, const PLOT_TYPE &ptype) {

  curve = new Curve<TVec>(name);
  if (curve==NULL) exit(10);
  curve->link();
  curve->make_default();
  px = new unsigned char[3*w*h];

  TVec pt,pt2;
  RGB c;

  float pttable[w][h];
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

  if (ptype==PT_PLOT) {
    cout << "pt plot\n";
    for (int j=0;j<h;j++) {
      for (int i=0;i<w;i++) {

        TVec from = Pts_i[i], to = Pts_j[j], tg = Tg_i[i];
        TVec D = to - from;

        float Dlen = D.norm();
        float cosa = tg.dot(D)/Dlen;
        float sina = sqrt(1.0-cosa*cosa);
        
        if (sina==0) cur = 0.0;
        else cur = .5*thickness*sina/Dlen;

        if (cur<0.) cur = 0.;
        else if (cur>1.) cur = 1.;

        ptmin = (cur<ptmin?cur:ptmin);
        ptmax = (cur>ptmax?cur:ptmax);

        pttable[i][j] = cur;
      }
    }
  }
#ifndef COMPUTE_IN_R4
  else if (ptype==TT_PLOT) {
    float z,n,tt2;
    for (int j=0;j<h;j++) {
      for (int i=0;i<w;i++) {

        TVec P1 = Pts_i[i], T1 = Tg_i[i];
        TVec P2 = Pts_j[j], T2 = Tg_j[j];
        TVec E = P1-P2;
        E.normalize();
        Matrix3 R;
        for (int k=0;k<3;k++)
          for (int l=0;l<3;l++)
            R[k][l] = 2.*E[k]*E[l] - (k==l?1.:0.);
        z = T1.cross(T2).dot(E); z=z*z;
        n = (((P1-P2).norm()*(P1-P2).norm()) *
             (1.0-(T2.dot(R*T1))*(T2.dot(R*T1))));
        if (T2.dot(R*T1)*T2.dot(R*T1) >= 1.0 )
          tt2 = 0;
        else
          tt2 = z/n;
        cur = sqrt(tt2)*.5*thickness;

        // FIXME clamp tt plot to [0,1]
        // I didn't think about it, but it fixes the plot ...
        if (cur<0.) cur=0.;
        if (cur>1.) cur=1.;

        ptmin = (cur<ptmin?cur:ptmin);
        ptmax = (cur>ptmax?cur:ptmax);

        pttable[i][j] = cur;
      }
    }
  }
#endif
  else if (ptype==PP_PLOT) {
    for (int j=0;j<h;j++) {
      // pttable[j][j] = 0.0;
      for (int i=0;i<w;i++) {
        TVec from = Pts_i[i], to = Pts_j[j];

        // XXX How should we chose the scaling for pp plots!!!
        //     the below gives the same scale as for pt plots.
        //     only problem : the circle is chopped of
        //cur = 2.*thickness/(from-to).norm();
        cur = (from-to).norm()*.5/thickness;

        if (cur<0) cur = 0.0;
        // if (cur>1. || (from-to).norm()<1e-7) cur = 1.;

        ptmin = (cur<ptmin?cur:ptmin);
        ptmax = (cur>ptmax?cur:ptmax);

        pttable[i][j] = cur;
      }
    }
  }
  for (int j=0;j<h;j++) {
    src = px + j*(3*w);
    for (int i=0;i<w;i++) {
      if (ptype==TT_PLOT)
        map_color_fine(&c,pttable[i][j],0.0,1.0);
      else if (ptype==PP_PLOT)
        map_color_sine_end(&c,pttable[i][j],0.,ptmax);
      else
        map_color_sine_end(&c,pttable[i][j],0.0,1.0); // (ptmax>1.0)?ptmax:1.0);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  cout << "Min/Max   : " << ptmin << " / " << ptmax << endl;
  cout << "Thickness : " << thickness << endl;

  sp_png_write_rgb("out.png",px,w,h,0.1,0.1,3*w);

  delete px; delete curve;
}

void usage(char* prog) {
    cout << "Usage : " << prog << " [option] <pkf file>\n";
    cout << "\n  -plot type\ttype is either pp, pt or tt.\n"
         << "  -res N\tN is the resolution of the plot. Default 500\n";
    exit(-1);
}

int main(int argc,char **argv) {

  if (argc<2) {
    usage(argv[0]);
  }

  int Res = 500;
  char *cname = NULL;
  PLOT_TYPE pType = PT_PLOT;

  fromx = 0.0; tox = 1.0;
  fromy =0.0; toy = 1.0;

  int arg = 1;
  while (arg<=argc) {
    if (argv[arg][0] == '-') {
      if (!strncmp(&argv[arg][1],"h",1)) {
        usage(argv[0]);
      }
      else if (!strncmp(&argv[arg][1],"res",3)) {
        Res = atoi(argv[arg+1]);
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

  cout << "Start plot ";
  DoPlot(cname,Res,Res,pType);
  cout << "\t[OK]\n";

}
