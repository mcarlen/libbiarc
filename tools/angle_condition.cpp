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
#include <algorithm>

// #define TEST

struct container {
  float s,sigma,tau;
};

bool operator<(const container& a, const container& b) {
      return a.s < b.s;
}

vector<container> contacts;
vector<container> contacts2;
Curve<Vector3> *knot;

// !! normalize a and b !!
float angle(const Vector3 &a, const Vector3 &b) {
  return acos(a.dot(b));
}

// transpose and sort contacts first!
// we need the first and last point t=0, t=1 !!
float interpolate(const vector<container> &contacts, float s) {
  if (s==0 || s==1) return contacts[0].sigma;
  if (s>1) s -= 1;
  float sigma = contacts[0].sigma;
  unsigned int i = 1;
  while (s > contacts[i].s  && i < contacts.size()-1) {
    sigma = contacts[i].sigma;
    ++i;
  }
  if (contacts[i-1].sigma <= contacts[i].sigma) {
    return (sigma + (s - contacts[i-1].s)/(contacts[i].s - contacts[i-1].s)*(contacts[i].sigma - contacts[i-1].sigma));;
  }
  else {
    float ret = (sigma + (s - contacts[i-1].s)/(contacts[i].s - contacts[i-1].s)*(contacts[i].sigma + 1. - contacts[i-1].sigma));;
    if (ret>=1) ret -= 1;
    return ret;
  }
}

void force(float s, int i, float *Fi, float *Fo,
           float *curv, float *psi, float* theta) {
  float sigma, tau, factor;
  Vector3 ps, psigma, ptau, tmp;
  Vector3 normal;

  sigma   = interpolate(contacts,s);
  tau     = interpolate(contacts2,s);

  if (i>=0)
    ps    = (*knot)[i].getPoint();
  else
    ps    = knot->pointAt(s);
  psigma  = knot->pointAt(sigma);
  ptau    = knot->pointAt(tau);

  if (i<0) {
    i = knot->biarcPos(s);
    normal = knot->pointAt(s+.00001) - 2*ps + knot->pointAt(s-.00001);
  }
  else
    normal = knot->normalVector(i);
  normal.normalize();
  *curv   = knot->curvature(i);

  tmp     = ps - ptau; tmp.normalize();
  *psi     = angle(tmp, -normal);
  tmp     = psigma - ps; tmp.normalize();
  *theta   =  angle(tmp, normal);

  factor  = *curv/sin(*psi + *theta);

  *Fi     =  factor*sin(*theta);
  *Fo     = -factor*sin(*psi);
}

int main(int argc, char** argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf> <contacts>\n";
    exit(0);
  }

  struct container vals, vals2;
  ifstream in(argv[2],ios::in);
  while (in >> vals.s >> vals.sigma >> vals.tau) {
    if (vals.s >= 1) vals.s = vals.s - 1;
    if (vals.sigma >= 1) vals.sigma = vals.sigma - 1;
    contacts.push_back(vals);
    vals2.s = vals.sigma; vals2.sigma = vals.s;
    contacts2.push_back(vals2);
  }
  in.close();

  float mid = std::abs((contacts[0].sigma+contacts.back().sigma)*.5);
  vals.s = 0; vals.sigma = mid;
  contacts.insert(contacts.begin(),vals);
  vals.s = 1; vals.sigma = mid;
  contacts.push_back(vals);

  sort(contacts2.begin(),contacts2.end());
  mid = std::abs((contacts2[0].sigma+contacts2.back().sigma)*.5);
  vals.s = 0; vals.sigma = mid;
  contacts2.insert(contacts2.begin(),vals);
  vals.s = 1; vals.sigma = mid;
  contacts2.push_back(vals);

  knot = new Curve<Vector3>(argv[1]);
  if (knot==NULL) {
    cout << "Knot problem\n";
    exit(1);
  }
  knot->link();
  knot->make_default();
  knot->normalize();
  knot->make_default();

//  cerr.precision(14);

  cout << "#Inventor V2.1 ascii\nSeparator {";
  cout << "Coordinate3 {\npoint [\n";
  int num = 0;
  float sum = 0;
  float sum2 = 0;
  float my_s, sigma, sig, sig2, my_sigma, dsigma_by_ds;
  float Fi_s, Fo_s, Fi_sigma, Fo_sigma,cur_s,cur_sigma;
  float psi_s,theta_s,psi_sigma,theta_sigma;
  for (int i=0;i<knot->nodes()-1;++i) {
    my_s = (*knot)[i].biarclength();
    sigma = interpolate(contacts,sum);
    sig = interpolate(contacts,sum + my_s);
    if (sig<sum+my_s) sig+=1.;
    sig2 = sigma;
    if (sig2<sum) sig2 += 1.;
    my_sigma = sig - sig2;

    dsigma_by_ds = my_sigma/my_s;

    force(sum,i,&Fi_s,&Fo_s,&cur_s,&psi_s,&theta_s);
    force(sigma,-1,&Fi_sigma,&Fo_sigma,&cur_sigma,&psi_sigma,&theta_sigma);

#define SECOND_ORDER
#ifdef SECOND_ORDER
    float Fi_s2,Fo_s2,Fi_sigma2,Fo_sigma2;
    float cur_s2,psi_s2,theta_s2,cur_sigma2,psi_sigma2,theta_sigma2;
    force(sum+my_s,i+1,&Fi_s2,&Fo_s2,&cur_s2,&psi_s2,&theta_s2);
    if (sig>=1) sig-=1.;
    force(sig,-1,&Fi_sigma2,&Fo_sigma2,&cur_sigma2,&psi_sigma2,&theta_sigma2);

    Fi_s = (Fi_s+Fi_s2)*.5;
    Fo_s = (Fo_s+Fo_s2)*.5;
    Fi_sigma = (Fi_sigma+Fi_sigma2)*.5;
    Fo_sigma = (Fo_sigma+Fo_sigma2)*.5;
#endif

    if (true || psi_s>1.5 || theta_s>1.5) {
#if 0

      // strut 1
      //cout << point_s     << ",";
      //cout << point_sigma << ",";

/*
      // strut 2
      cout << point_s   << ",";
      cout << point_tau << ",";

      // strut 3 (higher up)
      //cout << point_sigma << "," << point_sigmasigma << ",";
*/
      // normal
      //cout << point_s << ",";
      //cout << point_s + normal_s*(point_s-point_sigma).norm()/3. << ",";

      // next normal (at sigma)
      cout << point_sigma << ",";
      cout << point_sigma + normal_sigma*(point_s-point_sigma).norm()/3 << ",";
      cout << endl;
#endif
      num++;
      // FIXME FIXME Fi_sigma/dsigma_by_ds not in formula!!!!
      cerr << sum << " " << theta_s << " " << Fo_s
           << " " << sigma << " " << psi_sigma
           << " " << Fi_sigma << " "
           << cur_s << " " << 0 << " " // sigmasigma << " "
           << my_s << " " << my_sigma << " "
           << dsigma_by_ds << " " << sum << " " << sum2 << endl;
      sum += my_s;
      sum2 += my_sigma;
    }
  }
#if 0
  cout << "]}\n";
  cout << "LineSet { numVertices [\n";
  for (int i=0;i<num;++i) {
//    cout << "2,2,2,2,2,";
    cout << "2,"; // 2,2,";
  }
  cout << "]}\n";
  cout << "}";
#endif
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
