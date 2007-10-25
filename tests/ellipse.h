/*!
  \defgroup ObjectGroup Object Directory
*/

/*!
  \file ellipse.cpp
  \ingroup ObjectGroup
  \brief Constructs a ellipse.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../objects/aux.h"

float p_diff_ellipse(float a,float b, float ds, Curve<Vector3>* c) {
  Vector3 p, p2, dummy;
  float max=0.0,s_max=0.0;
  for (float s=0.0;s<=c->length();s+=ds) {
    p = c->pointAt(s);
    pointat_ellipse(a,b,s/c->length(),p2,dummy);
    if ((p-p2).norm2()>max) {
      max = (p-p2).norm2();
      s_max = s;
    }
  }
  // cout << "s_max = " << s_max << " ; max = " 
  //cout << "\nEllipse :\nP  " << max << endl;
  return max;
}

float t_diff_ellipse(float a,float b,float ds,Curve<Vector3>* c) {
  Vector3 t, t2, dummy;
  float max=0.0,s_max=0.0;
  for (float s=0.0;s<=c->length();s+=ds) {
    t = c->tangentAt(s);
    pointat_ellipse(a,b,s/c->length(),dummy,t2);
    if ((t-t2).norm2()>max) {
      max = (t-t2).norm2();
      s_max = s;
    }
  }
  //cout << "T  " << max << endl;
  return max;
}

void mp_diff_ellipse(float a,float b, Curve<Vector3>* c,
                     float &pmax, float &tmax) {

  Vector3 MP, rMP;
  Vector3 MT, rMT;

  Vector3 t, t2, dummy;
  pmax=0.0,tmax=0.0;

  Biarc<Vector3>* curr = c->listHead();
  for (int i=0;i<c->nodes();i++) {
    MP = curr->getMidPoint();
    MT = curr->getMidTangent();
    pointat_ellipse(a,b,((float)i+.5)/(float)c->nodes(),rMP,rMT);

    curr = curr->getNext();
    if ((MP-rMP).norm2()>pmax) {
      pmax = (MP-rMP).norm2();
    }
    if ((MT-rMT).norm2()>tmax) {
      tmax = (MT-rMT).norm2();
    }
  }

  //cout << "mP " << pmax << endl  << "mT " << tmax<<endl;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
