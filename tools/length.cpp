/*!
  \file length.cpp
  \ingroup ToolsGroup
  \brief Arc-length of a PKF curve.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc<3 || argc>5 || argc==4) {
    cerr << "Usage : "<<argv[0]<<" [-rescale=VAL] <closed=1;open=0> <pkf in> [pkf out]\n";
    exit(0);
  }
	
  int CLOSED = 0;
  float Scale = -1.0;
  int IDX = 1;

  if (argc==5) IDX = 2;

  for (int i=1;i<argc-IDX;i++) {
    if (argv[i][0]=='-') {
      if (!strncmp(argv[i],"-rescale=",9)) {
        if (argc==3) exit(2);
        Scale = atof(&(argv[i][9]));
        if (Scale<=0) cerr << "Rescale value is negative! Ignored.\n";
      }
      else {
        cerr << &argv[i][1] << " is not an option.\n";
        exit(3);
      }
    } 
    else if (atoi(argv[i])==1) CLOSED = 1;
    else if (atoi(argv[i])==0) CLOSED = 0;
    else
      exit(3);
  }

  CurveBundle<Vector3> cb(argv[argc-IDX]);
  if (CLOSED)
    cb.link();

  if (Scale > 0) {
    for (int i=0;i<cb.curves();i++)
      cb[i].scale(Scale);
  }

  cb.make_default();

  cout << "Length info : \n";
  cout.precision(10);
  for (int i=0;i<cb.curves();i++)
    cout << "Curve " << i+1 << " : " << cb[i].length() << endl;
  cout << "----------\nTotal length : " << cb.length() << endl;
  
  if (argc==5) cb.writePKF(argv[argc-IDX+1]);

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
