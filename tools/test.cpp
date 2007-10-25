/*!
  \file test.cpp
  \ingroup ToolsGroup
  \brief Program for testing purpose only.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  Curve<Vector3> c;
  Vector3 v;

  char line[200];
  cin.getline(line,sizeof line);
  int N = atoi(line);
  for (int i=0;i<N;i++) {
    cin.getline(line,sizeof line);
    v[0] = atof(strtok(line," "));
    v[1] = atof(strtok(NULL," "));
    v[2] = atof(strtok(NULL," "));
    c.append(v,Vector3(0,0,0));
  }
  c.computeTangents();
  c.writePKF("ccurve.pkf");

/*
  if (argc==4) {  
    Curve<Vector3> c;
    cout << "Read PKF " << argv[2];
    c.readPKF(argv[2]);
    cout << "\t\t[OK]\n";
    float scale = atof(argv[1]);
    cout << "Scale curve by the factor " << scale;
    c.scale(scale);
    cout << "\t\t[OK]\n";
    cout << "Write to " << argv[3];
    c.writePKF(argv[3]);
    cout << "\t\t\t[OK]\n";
  }
  else
    cout << "Usage "<< argv[0] << " <scalefactor> <pkf_in> <pkf_out>\n";
*/
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
