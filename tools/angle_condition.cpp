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
float interpolate(const vector<container> &contacts, float s) {
  float sigma = contacts[0].sigma;
  if (s>=1) s -= 1;
  unsigned int i = 1;
  while (s > contacts[i].s  && i < contacts.size()) {
    sigma = contacts[i].sigma;
    ++i;
  }
  return (sigma + (s - contacts[i-1].s)/(contacts[i].s - contacts[i-1].s)*(contacts[i].sigma - contacts[i-1].sigma));;
}

#if 0
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

  sort(contacts2.begin(),contacts2.end());

  knot = new Curve<Vector3>(argv[1]);
  if (knot==NULL) {
    cout << "Knot problem\n";
    exit(1);
  }
  knot->link();
  knot->make_default();
  knot->normalize();
  knot->make_default();

  Vector3 normal_s, normal_sigma, point_s, point_sigma, point_tau, point_sigmasigma;
  float cur_s, cur_sigma;

  cout << "#Inventor V2.1 ascii\nSeparator {";
  cout << "Coordinate3 {\npoint [\n";
  int num = 0;
  for (int i=1;i<contacts.size();++i) {

    int n = knot->biarcPos(contacts[i].s);
    float my_s = (*knot)[n].biarclength();
    normal_s = knot->normalVector(n);
    normal_s.normalize();
    cur_s = knot->curvature(n);
    n = knot->biarcPos(contacts[i].sigma);
    // FIXME my_sigma is the arclength between
    // the sigma contact points of the endpoints
    // of biarc (*knot)[n]
    // XXX wrap around correctly!
    float sig = interpolate(contacts,contacts[i].s + my_s);
    if (sig<contacts[i].s+my_s) sig+=1.;
    float sig2 = contacts[i].sigma;
    if (sig2<contacts[i].s) sig2 += 1.;
    float my_sigma = sig - sig2;
    normal_sigma = knot->normalVector(n);
    normal_sigma.normalize();
    cur_sigma = knot->curvature(n);

    float dsigma_by_ds = my_sigma/my_s;

    point_s          = knot->pointAt(contacts[i].s);
    point_sigma      = knot->pointAt(contacts[i].sigma);
    float tau        = interpolate(contacts2,contacts[i].s);
    point_tau        = knot->pointAt(tau);
    float sigmasigma = interpolate(contacts,contacts[i].sigma);
    point_sigmasigma = knot->pointAt(sigmasigma);

    Vector3 tmp = point_s - point_tau; tmp.normalize();
    float psi_s       = angle(tmp, -normal_s);
    tmp               = point_sigma - point_s; tmp.normalize();
    float theta_s     =  angle(tmp, normal_s);
    float psi_sigma   = angle(tmp, -normal_sigma);
    tmp               = point_sigmasigma - point_sigma; tmp.normalize();
    float theta_sigma = angle(tmp, normal_sigma);

    float factor_s     = cur_s/sin(psi_s + theta_s);
    float factor_sigma = cur_sigma/sin(psi_sigma + theta_sigma);

    float Fi_s     =  factor_s*sin(theta_s);
    float Fo_s     = -factor_s*sin(psi_s);
    float Fi_sigma =  factor_sigma*sin(theta_sigma);
    float Fo_sigma = -factor_sigma*sin(psi_sigma);

//    float Fi_sigma = -factor_sigma*sin(psi_sigma);
//    float Fo_sigma =  factor_sigma*sin(theta_sigma);

    if (true || psi_s>1.5 || theta_s>1.5) {

      // strut 1
      cout << point_s     << ",";
      cout << point_sigma << ",";

      // strut 2
      cout << point_s   << ",";
      cout << point_tau << ",";

      // strut 3 (higher up)
      //cout << point_sigma << "," << point_sigmasigma << ",";

      // normal
      cout << point_s << ",";
      cout << point_s + normal_s*(point_s-point_sigma).norm()/3. << ",";

      // next normal (at sigma)
      //cout << point_sigma << ",";
      //cout << point_sigma + normal_sigma*(point_s-point_sigma).norm()/3 << ",";
      cout << endl;

      num++;
      // FIXME FIXME Fi_sigma/dsigma_by_ds not in formula!!!!
      cerr << contacts[i].s << " " << theta_s << " " << Fo_s
           << " " << contacts[i].sigma << " " << psi_sigma
           << " " << Fi_sigma << " "
           << cur_s << " " << sigmasigma << " " << my_s << " " << my_sigma << " " << dsigma_by_ds << endl;
    }
  }
  cout << "]}\n";
  cout << "LineSet { numVertices [\n";
  for (int i=0;i<num;++i) {
//    cout << "2,2,2,2,2,";
    cout << "2,2,2,";
  }
  cout << "]}\n";
  cout << "}";

}
#endif
#if 0
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

  knot = new Curve<Vector3>(argv[1]);
  if (knot==NULL) {
    cout << "Knot problem\n";
    exit(1);
  }
  knot->link();
  knot->make_default();
  knot->normalize();
  knot->make_default();

  sort(contacts2.begin(),contacts2.end());

  for (int i=0;i<100;++i) {
    float s = (float)i/100.;
    cout << s << " " << interpolate(contacts2, s) << endl;
  }

  float s = 0, s2 = 0;
  for (int i=0;i<knot->nodes();++i) {
    float sig = interpolate(contacts,s);
    if (sig<s) sig+=1.;
    s += (*knot)[i].biarclength();
    float sig2 = interpolate(contacts,s);
    if (sig2<s) sig2+=1.;
    s2 += sig2-sig;
    cerr << s << " " << s2 << endl;
  }
  cerr << s << endl;
  cerr << s2 << endl;
 
}
#endif

#if 1
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

  sort(contacts2.begin(),contacts2.end());

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

  Vector3 normal_s, normal_sigma, point_s, point_sigma, point_tau, point_sigmasigma;
  float cur_s, cur_sigma;

  cout << "#Inventor V2.1 ascii\nSeparator {";
  cout << "Coordinate3 {\npoint [\n";
  int num = 0;
  float sum = 0;
  float sum2 = 0;
  for (int i=0;i<knot->nodes();++i) {
    float my_s = (*knot)[i].biarclength();
    normal_s = knot->normalVector(i);
    normal_s.normalize();
    cur_s = knot->curvature(i);
    float sigma = interpolate(contacts, sum);
    int n = knot->biarcPos(sigma);
    // FIXME my_sigma is the arclength between
    // the sigma contact points of the endpoints
    // of biarc (*knot)[n]
    // XXX wrap around correctly!
    float sig = interpolate(contacts,sum + my_s);
    if (sig<sum+my_s) sig+=1.;
    float sig2 = sigma;
    if (sig2<sum) sig2 += 1.;
    float my_sigma = sig - sig2;
    // XXX the normal at sigma is "delicate" to compute :(
 //   normal_sigma = knot->normalVector(n);
    normal_sigma = knot->pointAt(sigma+.00001) - 2*knot->pointAt(sigma) + knot->pointAt(sigma-.00001);
    normal_sigma.normalize();
    cur_sigma = knot->curvature(n);

    float dsigma_by_ds = my_sigma/my_s;

    point_s          = (*knot)[i].getPoint(); // pointAt(contacts[i].s);
    point_sigma      = knot->pointAt(sigma);
    float tau        = interpolate(contacts2,sum);
    point_tau        = knot->pointAt(tau);
    float sigmasigma = interpolate(contacts,sigma);
    point_sigmasigma = knot->pointAt(sigmasigma);

    Vector3 tmp = point_s - point_tau; tmp.normalize();
    float psi_s       = angle(tmp, -normal_s);
    tmp               = point_sigma - point_s; tmp.normalize();
    float theta_s     =  angle(tmp, normal_s);
    float psi_sigma   = angle(tmp, -normal_sigma);
    tmp               = point_sigmasigma - point_sigma; tmp.normalize();
    float theta_sigma = angle(tmp, normal_sigma);

    float factor_s     = cur_s/sin(psi_s + theta_s);
    float factor_sigma = cur_sigma/sin(psi_sigma + theta_sigma);

//    float Fi_s     =  factor_s*sin(theta_s);
    float Fo_s     = -factor_s*sin(psi_s);
    float Fi_sigma =  factor_sigma*sin(theta_sigma);
//    float Fo_sigma = -factor_sigma*sin(psi_sigma);

    if (true || psi_s>1.5 || theta_s>1.5) {

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

      num++;
      // FIXME FIXME Fi_sigma/dsigma_by_ds not in formula!!!!
      cerr << sum << " " << theta_s << " " << Fo_s
           << " " << sigma << " " << psi_sigma
           << " " << Fi_sigma << " "
           << cur_s << " " << sigmasigma << " "
           << my_s << " " << my_sigma << " "
           << dsigma_by_ds << " " << sum << " " << sum2 << endl;
      sum += my_s;
      sum2 += my_sigma;
    }
  }
  cout << "]}\n";
  cout << "LineSet { numVertices [\n";
  for (int i=0;i<num;++i) {
//    cout << "2,2,2,2,2,";
    cout << "2,"; // 2,2,";
  }
  cout << "]}\n";
  cout << "}";

}
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS
