#include "plot_funcs.h"

// Sampling (x,y the same)
int Res = 500;

// Zoom in on some rectangular region
float fromx = 0, tox = 1, fromy = 0, toy = 1;

char outname[40];
PLOT_TYPE pType = PT_PLOT; 

int HEIGHTMAP = 0;

// Global variable for open/closed curve
// default is closed
int CLOSED = 1;

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

int main(int argc,char **argv) {

  if (argc<2) {
    usage(argv[0]);
  }

  char *cname = NULL;

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

  cout << "Square X : " << fromx << " to " << tox << endl
       << "       Y : " << fromy << " to " << toy << endl;

  cout << "Start plot with Resolution " << Res;
  DoPlot(cname,Res,Res,fromx,tox,fromy,toy,pType,CLOSED,HEIGHTMAP);
  cout << "\t[OK]\n";

}
