#include "../include/Curve.h"
#include "../include/algo_helpers.h"

/*!
  Returns true if the index difference between i and
  j is greater than the given skip parameter.
*/
int check_idx_skip(const int i, const int j, const int N, const int skip) {
  int didx = j-i;
  if (didx > N-didx) didx = N-didx;
  return (int)(didx > skip);
}

void Smooth(Curve<Vector3> &c, int stencil) {
  
}

void Shrink(Curve<Vector3> &c,float factor) {
  for (int i=0;i<c.nodes();++i)
    c[i].setPoint(c[i].getPoint()*factor);
}

float GetLength(Curve<Vector3> &c) {
  float L = 0;
  for (int i=0;i<c.nodes();++i)
    L += (c[i].getPoint()-c[(i+1)%(c.nodes())].getPoint()).norm();
  return L;
}

void SavePkf(Curve<Vector3> &c, int i) {
  char buf[16];
  sprintf(buf,"%010i.pkf",i);
  c.computeTangents();
  c.writePKF(buf);
}

void ShiftNodes(Curve<Vector3> &c, float scalefactor) {
  int dir = rand()%2;
  Vector3 e1, e2, dummy;
  float n1, n2;
  for (vector<Biarc<Vector3> >::iterator it=c.begin();it!=c.end();it++) {
    e1 = (it->getNext().getPoint()-it->getPrevious().getPoint());
    n1 = e1.norm(); e1/=n1;
    dummy = (it->getNext().getPoint()-it->getPoint());
    e2 = dummy-dummy.dot(e1)*e1;
    n2 = e2.norm(); e2/=n2;
    if (dir)
      it->setPoint(it->getPoint()+e2*n2*scalefactor+e1*n1*scalefactor*.01);
    else
      it->setPoint(it->getPoint()+e2*n2*scalefactor-e1*n1*scalefactor*.01);
  }
}

int RemoveOverlaps(Curve<Vector3> &c,float D,int skip,float delta) {
  int overlap = 0, N = c.nodes(), start = rand()%N;
  float diff;
  Vector3 v;
  for (int i=0;i<N;++i) {
    for (int j=0;j<N;++j) {
      v = (c[(start+i)%N].getPoint()-c[(start+j)%N].getPoint());
      if (v.norm() < D && check_idx_skip(i,j,N,skip)) {
        overlap++;
        diff = fabsf(v.norm()-D-delta)*.5; v.normalize();
        c[(start+i)%N].setPoint(c[(start+i)%N].getPoint()+v*diff);
        c[(start+j)%N].setPoint(c[(start+j)%N].getPoint()-v*diff);
      }
    }
  }
  return overlap;
}

void ControlLeashes(Curve<Vector3> &c,float l) {
/*
  c.computeTangents();
  c.make_default();
  c.resample(c.nodes());
*/
  int N = c.nodes(), start = rand()%N, i, ii, jj;
  int dir = rand()%2;
  Vector3 d;
  float dd,sep,lmin=10000,lmax=0;
  for (i=0;i<N;++i) {
    if (dir)
      ii = (start+i)%N;
    else {
      ii = (start-i); if (ii<0) ii+=N;
    }
    if (dir)
      jj = (start+i+1)%N;
    else {
      jj = (start-i-1); if (jj<0) jj+=N;
    }
//    cout << "Correct " << ii
//         << " and "    << jj << endl;
    d = c[jj].getPoint()-c[ii].getPoint();
    dd = d.norm(); d.normalize();
    if (dd>lmax) lmax = dd;
    else if (dd<lmin) lmin = dd;
    sep = (l-dd)*.5;
    if (fabsf(l-dd)>l*.2) {
      c[jj].setPoint(c[jj].getPoint()+sep*d);
      c[ii].setPoint(c[ii].getPoint()-sep*d);
    }
  }
//  cout << "Leashes min/max : " << lmin << "/" << lmax << endl;
}
#if 1

int   OverlapTol       = 10;
float OverlapDelta     = 0.0;
float RelErrTol        = 1e-4;
float ShiftScaleFactor = 0.0002;
float ShrinkFactor     = .999;
int   MaxIter;

int main(int argc,char** argv) {
  if (argc!=7) {
    cout << "Usage   : " << argv[0]
         << " <overlap>"
         << " <overlap delta>"
         << " <shift>"
         << " <shrink>"
         << " <no steps>"
         << " <pkf>\n"
         << "Example : 1 0.001 0.001 .99 250 bone.pkf\n";
    exit(1);
  }

  OverlapTol       = atoi(argv[1]);
  OverlapDelta     = atof(argv[2]);
  ShiftScaleFactor = atof(argv[3]);
  ShrinkFactor     = atof(argv[4]);
  MaxIter          = atoi(argv[5]);

  srand(time(NULL));

  Curve<Vector3> c(argv[6]);
  c.link();
  c.make_default();
 
  int   N = c.nodes();
  float D = c.thickness_fast()*.99;
  float L = GetLength(c);

  float l = L/(float)N;

  Vector3 v;
  int overlap, skip = (int)(M_PI*D*.5*(float)N/(float)L);
  int MaxOverlap = 0, AvgOverlap = 0, ShrinkCount = 0;
  int ShrinkFailed = 0;
  if (skip>N/2) skip = N/2-1;

  float InitRope = L/D;
  float Rope = L/D;
  float BestRope = L/D;
  float RopeOld = 0.;

  cout << "N="     << N
       << ",D="    << D
       << ",L="    << L
       << ",Rope=" << Rope
       << ",l="    << l
       << ",skip=" <<skip<< endl;

  int zz = 0;
  SavePkf(c,zz++);
  //while(fabsf(RopeOld-Rope)/Rope > RelErrTol && zz<1000) {
  while (zz<MaxIter) {
    // if (zz%50==0 && zz!=0)
    overlap = RemoveOverlaps(c,D, (int)(M_PI*D*.5*(float)N/(float)L),OverlapDelta);
    ControlLeashes(c,l);
    if (ShrinkFailed > 50) {
      ShiftNodes(c,ShiftScaleFactor);
      ControlLeashes(c,l);
    }
    if (overlap <= OverlapTol) { 
      SavePkf(c,zz++);
      Shrink(c,ShrinkFactor);
      ControlLeashes(c,l);
      l*=ShrinkFactor;
      if (overlap>MaxOverlap) MaxOverlap = overlap;
      AvgOverlap += overlap;
      ShrinkCount++;
    }
    else
      ShrinkFailed++;

    L = GetLength(c);
    RopeOld = Rope;
    Rope = L/D;
    if (Rope < BestRope) BestRope = Rope;
  }
  cout << "\nInit Rope     = " << InitRope << endl;
  cout << "Best Rope     = " << BestRope << endl;
  cout << "Max overlaps  = " << MaxOverlap << endl;
  cout << "Average overl = " << (float)AvgOverlap/(float)ShrinkCount << endl;
}
#else
int main(int argc,char** argv) {

  srand(time(NULL));

  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();
 
  int   N = c.nodes();
  float D = c.thickness_fast()*.99;
  float L = GetLength(c);

  float l = L/(float)N;

  ShiftNodes(c,0.1);  

  c.writePKF("out.pkf");
  return 0;
}
#endif
