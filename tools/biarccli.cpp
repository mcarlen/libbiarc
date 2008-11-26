/*!
  \file pointat.cpp
  \ingroup ToolsGroup
  \brief Writes point/tangent at Curve(s), for s arclength and s in [0,1]
         to the standart output.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include "../include/Tube.h"
#include "../include/algo_helpers.h"

#define write_vec(vec) (cout << vec[0] << ',' << vec[1] << ',' << (vec[2]))

CurveBundle<Vector3> *cb;

void printpointat(float s) {

  // Test

  vector<Biarc<Vector3> >::iterator current;
  Vector3 p, t;
  cout << ".";
  for (int i=0;i<cb->curves();i++) {

    if (s==0.0) {
      write_vec((*cb)[i].begin()->getPoint()); cout << ';';
      write_vec((*cb)[i].begin()->getTangent()); cout << '\n';
    }
    else if (s==1.0) {
      if ((*cb)[i].isClosed()) current = (*cb)[i].begin();
      else current = (*cb)[i].end()-1;
      write_vec(current->getPoint()); cout << ';';
      write_vec(current->getTangent()); cout << '\n';
    }
    else {
      current = (*cb)[i].begin();

      // check if we have valid biarcs!
      assert(current->isBiarc());
 
      float Total = s*(*cb)[i].length();
      while (Total > current->biarclength()) {
        Total -= current->biarclength();
        ++current;
      }

      p = current->pointOnBiarc(Total);
      t = current->tangentOnBiarc(Total);

      write_vec(p); cout << ';'; write_vec(t); cout << '\n';
    }
  }
}

int main(int argc, char **argv) {
  string line;
  string token;
  if (argc!=2) {
    cerr << "Usage : "<<argv[0]<<" <pkf in>\n";
    exit(0);
  }
  cb = new CurveBundle<Vector3>(argv[1]);
  cb->link();
  cb->make_default();
  cb->thickness();

  cout << "OK" << endl << flush;

  cout.precision(10);
  while (!cin.eof()) {
    cin >> line;
    //cout << "ddd>" << line << "<ddd" <<endl;;
    token.assign( strtok( (char *) line.c_str(),":")); 
    if (token.compare("pointat")==0) {
      token.assign(strtok(NULL,":"));
      double s = atof(token.c_str());
      if (s<0.0 or s>1.0) {
        cout << "! s-value out of bound. " << __FILE__ << ":" << __LINE__ << endl;
        continue;
      }
      printpointat(s);
    }

    if (token.compare("closed")==0) {
      cb->link();
      cb->make_default();
      cout << ".Curve closed." << endl;;
      continue;
    }

    if (token.compare("normalize")==0) {
      cb->normalize();
      cb->make_default();
      cout << ".Curve normalized." << endl;
      continue;
    }

    if (token.compare("thickness")==0) {
      cout << "." << (*cb)[0].thickness() << endl;
      continue;
    }

    if (token.compare("scale")==0) {
      token.assign(strtok(NULL,":"));
      double s = atof(token.c_str());
      if (s < 0.0) {
        cout << "! s-value out of bound. " << __FILE__ << ":" << __LINE__ << endl;
        continue;
      }
      cb->scale(s);
      cb->make_default();
      cout << ".Curve rescaled." << endl;
      continue;
    }

    if (token.compare("length")==0) {
      cout << "." << (*cb)[0].length() << endl;;
      continue;
    }

    if (token.compare("setprecision")==0) {
      token.assign(strtok(NULL,":"));
      cout.precision(atoi(token.c_str())) ;
      cout << "." << "New precision " << cout.precision() << endl;;
      continue;
    }

    if (token.compare("center")==0) {
      cb->center();
      cout << ".centered\n";
    }

    if (token.compare("rotate")==0) {
      token.assign(strtok(NULL,":"));
      float angle = atof(token.c_str());
      Vector3 axis;
      token.assign(strtok(NULL,":"));
      axis[0] = atof(token.c_str());
      token.assign(strtok(NULL,":"));
      axis[1] = atof(token.c_str());
      token.assign(strtok(NULL,":"));
      axis[2] = atof(token.c_str());
      (*cb)[0].rotAroundAxis(angle,axis);
      cout << "." << "Rotate " << angle << " rad around [ " << axis << " ].\n";
    }

    // Request mesh points, given N,S,R
    if (token.compare("mesh")==0) {
      token.assign(strtok(NULL,":"));
      int N = atoi(token.c_str());
      token.assign(strtok(NULL,":"));
      int S = atoi(token.c_str());
      token.assign(strtok(NULL,":"));
      float R = atof(token.c_str());
      Tube<Vector3> curve(argv[1]);
      if ((*cb)[0].isClosed()) curve.link();
      curve.make_default();
      curve.makeMesh(N,S,R,1e-3);
      cout << ".\n" << curve ;
    }

    if (token.compare("save")==0) {
      char* name = strtok(NULL,":");
      if (name==NULL)
        token.assign("curve.pkf");
      else
        token.assign(name);
      cb->writePKF(token.c_str());
      cout << ".Saved " << token << endl;;
      continue;
    }

    if (token.compare("exit")==0) {
      return 0;
    }
  }
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
