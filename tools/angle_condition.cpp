/*!
  \file angle_condition.cpp
  \ingroup ToolsGroup
  \brief Check the angle condition for an ideal knot.
         input file :
         s sigma tau
         ...

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

struct container {
  float s,sigma,tau;
};

vector<container> contacts;
Curve<Vector3> *knot;

void compute_forces(Curve<Vector3>* knot, float s, float sigma, float tau,
                    float *F1, float *F2) {

  Vector3 normal, point, strut1, strut2;
  float factor, theta, psi, curvature;
  int n = knot->biarcPos(s);

  curvature = 1./knot->curvature(n);
  normal = knot->normalVector(n);
  normal.normalize();

  point = knot->pointAt(s);
  strut1 = point - knot->pointAt(tau); strut1.normalize();
  strut2 = knot->pointAt(sigma) - point; strut2.normalize();

  theta = fabsf(strut1.dot(normal));
  theta = acos(theta);
  psi   = fabsf(strut2.dot(normal));
  psi = acos(psi);

  factor = curvature/sin(theta+psi);
  *F1 =  factor*sin(psi);
  *F2 = -factor*sin(theta);

  cerr << s << " " << theta << " " << psi << endl;
}

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf> <contacts>\n";
    exit(0);
  }
  
  struct container vals;
  ifstream in(argv[2],ios::in);
  while (in >> vals.s >> vals.sigma >> vals.tau)
    contacts.push_back(vals);
  in.close();

  knot = new Curve<Vector3>(argv[1]);
  if (knot==NULL) {
    cout << "Knot problem\n";
    exit(1);
  }
  knot->link();
  knot->make_default();
  knot->normalize();
  knot->make_default();

  // First Force computation
  // check for contact 5
  int num;
  float F1, F2, Fdummy;
  float s, sigma, tau, xx;
  for (int i=0;i<contacts.size();++i) {
    s = contacts[i].s;
    sigma = contacts[i].sigma;
    tau = contacts[i].tau;

//    cout << s << " " << fabsf(F1) << " " << fabsf(F2) << endl;
    
    /*
    compute_forces(knot, s, sigma, tau, &Fdummy, &F1);
    num = 0;
    while (contacts[num].s < sigma) num++;
    xx = contacts[num-1].sigma + (contacts[num].sigma - contacts[num-1].sigma)*(sigma - contacts[num-1].s)/(contacts[num].s - contacts[num-1].s);
    compute_forces(knot, sigma, xx, s, &F2, &Fdummy);
    */

    compute_forces(knot, s, sigma, tau, &F1, &Fdummy);
    num = 0;
    while (contacts[num].s < tau) num++;
    xx = contacts[num-1].tau + (contacts[num].tau - contacts[num-1].tau)*(tau - contacts[num-1].s)/(contacts[num].s - contacts[num-1].s);
    compute_forces(knot, tau, s, xx, &Fdummy, &F2);

    cout << i << " " << s << " " << F1 << " " << -F2 << endl;
  }

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
