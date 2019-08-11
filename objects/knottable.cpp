/*!
  \file knottable.cpp
  \ingroup ObjectGroup
  \brief Construct a table of curves.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include <stdlib.h>
#ifdef __FreeBSD__
# include <dirent.h>
#else
# include <sys/dir.h>
#endif
#include <vector>
#include <algorithm>

int main(int argc,char** argv) {

  if(argc<2 || argc>3) {
    cerr << "Usage : " << argv[0] << " [Spacing] <pkf directory>\n";
    cerr << "\nIn the specified directory, only *.pkf files\n"
	 << "are taken into account!\n";
    exit(1);
  }

  // Distance (coefficient) between the curve objects.
  float SPACING = 1.0;

  if (argc == 3)
    SPACING = atof(argv[1]);

  // number of knots in PKFdir?
  const char* szDir = argv[(argc==2?1:2)];
  DIR *pDir;

  cout << "Scan for *.pkf in directory "<<szDir << flush;
  pDir = opendir(szDir);
  if (!pDir) {
    cout << "\t[FAILED]\n"<<flush;
    exit(3);
  }

  Curve<Vector3> *pkf;

  int PKFcounter = 0;

#ifdef __FreeBSD__
  struct dirent *pEnt;
#else
  struct direct *pEnt;
#endif
  vector<string> Words;

  for (; (pEnt=readdir(pDir)) ;) {
    // Skip directories
    if (pEnt->d_type == DT_DIR)
      continue;

    if (!strncmp(&pEnt->d_name[strlen(pEnt->d_name)-4],".pkf",4)) {
      PKFcounter++;
      Words.push_back(string(szDir)+'/'+string(pEnt->d_name));
    }
  }
  closedir(pDir);
  if (PKFcounter<1) { cout << "\t\t[FAILED]\n"; exit(3); }
  cout << "\t\t[OK]\n";

  // Lexically sorting
  sort(Words.begin(), Words.end());

  cout << "Load " << PKFcounter << " pkf files";;
  // Now we load the PKF files

  pkf = new Curve<Vector3>[PKFcounter];
  PKFcounter = 0;
  if (pkf==NULL) { cout << "\t\t\t[FAILED]\n"; exit(3); }
  for (vector<string>::iterator it = Words.begin(); it != Words.end(); it++) {
      pkf[PKFcounter++].readPKF(it->c_str());
      pkf[PKFcounter-1].center();
      pkf[PKFcounter-1].link();
      pkf[PKFcounter-1].make_default();
      pkf[PKFcounter-1].normalize();
  }
  cout << "\t\t\t\t[OK]\n";

  // Construct table
  CurveBundle<Vector3> table;
  int Width, Height;

  Width = (int)ceil(sqrt((float)PKFcounter));
  Height = Width;//(Width*Width<PKFcounter)?Width:Width+1;

  cout << "Table dimensions : " << Width <<'x'<<Height<<endl;

  float minx=1000,maxx=-1000,miny=1000,maxy=-1000;
  vector<Biarc<Vector3> >::iterator b = pkf[0].begin();
  for (int i=0;i<pkf[0].nodes();i++) {
    if (b->getPoint()[0]<minx) minx=b->getPoint()[0];
    if (b->getPoint()[0]>maxx) maxx=b->getPoint()[0];
    if (b->getPoint()[1]<miny) miny=b->getPoint()[1];
    if (b->getPoint()[1]>maxy) maxy=b->getPoint()[1];

    b++;
  }
  float SizeH = maxy-miny, SizeW = maxx-minx;
  float OffsetH = (float)SizeH/2.0*(float)Height/2.0;
  float OffsetW = (float)SizeW/2.0*(float)Width/2.0;

  int count = 0;
  // Arrange curves!
  for (int i=0;i<Width;i++) {
    for (int j=0;j<Height;j++) {
      if (count<PKFcounter) {
	pkf[count] += (-Vector3(OffsetW,OffsetH,0)+
		       Vector3((float)i*SizeW*SPACING,(float)j*SizeH*SPACING,0));
	count++;
      }
    }
  }
  for (int i=0;i<PKFcounter;i++)
    table.newCurve(pkf[i]);

  table.header("Knot table","M.Carlen","","");
  cout << "Write file : knottable.pkf" << flush;
  if (!table.writePKF("knottable.pkf"))
    cerr << "[FAILED] Problem writing file."<<endl;
  else
    cout << "\t\t\t[OK]\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
