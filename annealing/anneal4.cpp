/*

Anneal a knot

Copyright (C) 1997 Ben Laurie
*/

#include <sys/types.h>
#include <stdio.h>
#include <limits.h>

#include "Vector4.h"
#define TVec Vector4

#include "CurveBundle.h"
#include "algo_helpers.h"

#ifndef WIN32
# include <unistd.h>
# include <sys/times.h>
# include <time.h>
#endif

#include <signal.h>



typedef int BOOL;
#define FALSE     0
#define TRUE      1

inline int R(int nLower,int nUpper) {
  float r=(float)rand()/((float)RAND_MAX+1);

  int n=(int)(nLower+r*(nUpper-nLower+1));
  assert(n >= nLower);
  assert(n <= nUpper);
  return n;
}

inline float r(float dLower,float dUpper) {
  return dLower+(float)rand()/RAND_MAX*(dUpper-dLower);
}

static float s_dMinSegDistance;
static float s_dInitialStep;

class CStep
    {
    float m_dPointStep;
    float m_dTangentStep;
public:
    CStep()
        { m_dPointStep=m_dTangentStep=1; }
    void Increase(BOOL bPoint)
        { Get(bPoint)*=1.1; }
    void Decrease(BOOL bPoint)
        { Get(bPoint)*=.9; }
    float &Get(BOOL bPoint)
        {
        if(bPoint)
            return m_dPointStep;
        return m_dTangentStep;
        }
    float Get(BOOL bPoint) const
        {
        if(bPoint)
            return m_dPointStep;
        return m_dTangentStep;
        }
    };

inline ostream &operator<<(ostream &os,const CStep &step)
    {
    os << step.Get(TRUE) << '/' << step.Get(FALSE);
    return os;
    }

CStep *StepArray;

void ShowStepSpread(CurveBundle<TVec> &c, ostream &os) {
  float dMinP,dMaxP,dMinT,dMaxT;

  dMinP=dMaxP=StepArray[0].Get(TRUE);
  dMinT=dMaxT=StepArray[0].Get(FALSE);
  for(int n=1; n < c[0].nodes() ; ++n) {
    double p=StepArray[n].Get(TRUE);
    double t=StepArray[n].Get(FALSE);

    if(p < dMinP)
      dMinP=p;
    else if(p > dMaxP)
      dMaxP=p;
    if(t < dMinT)
      dMinT=t;
    else if(t > dMaxT)
      dMaxT=t;
  }

  os << dMinP << '-' << dMaxP << '/' << dMinT << '-' << dMaxT;
}

# define STEP_CHANGE	.005

// XXX : Improve these checks!!!
//       after each change we have to recompute the biarc Midpoint!!!!
//       implement a routine that does that does the midpoint rule
//       for a single biarc
BOOL CheckTangent(int n, Curve<TVec> &c) {
return (c[n].isProper() && c[n].getPrevious().isProper());
/*
 if ((c[n].getPoint()-c.getPrevious(n).getPoint()).dot(c[n].getTangent()) <= 0)
   return FALSE;
 if((c.getNext(n).getPoint()-c[n].getPoint()).dot(c[n].getTangent()) <= 0)
   return FALSE;
 return TRUE;
*/
}

BOOL CheckPoint(int n, Curve<TVec> &c) {
return (c[n].isProper()&&c[n].getPrevious().isProper());
/*
  if (!CheckTangent(n,c)) return FALSE;
  if((c[n].getPoint()-c.getPrevious(n).getPoint()).dot(c.getPrevious(n).getTangent()) <= 0)
    return FALSE;
  if((c.getNext(n).getPoint()-c[n].getPoint()).dot(c.getNext(n).getTangent()) <= 0)
    return FALSE;
  return TRUE;
*/
}

float Energy(CurveBundle<TVec> &rKnot) {
  // s_dMinSegDistance=rKnot.MinSegDistanceCache();
  double off_equi;
  rKnot.make_default();
  s_dMinSegDistance = rKnot.thickness();
  if (s_dMinSegDistance > 100) {
    cout << "TOO BIG : " << s_dMinSegDistance << endl;
    exit(12);
  }
  if (s_dMinSegDistance < 1e-6) {
    cout << "NEG ENERGY! : " << s_dMinSegDistance << endl;
    exit(11);
  } //__qurick
  off_equi = (rKnot[0].maxSegDistance() / rKnot[0].minSegDistance());
  // return rKnot.length()/s_dMinSegDistance; // +0.0001*(off_equi);

  // On S^3 we maximize thickness
  return 1./s_dMinSegDistance + 0.0000001*(off_equi) ;// + rKnot.length())/(s_dMinSegDistance*s_dMinSegDistance);
}

class CAnnealInfo {
public:
  int m_nWriteFrequency;
  int m_nLogFrequency;
  float m_fTemperature;
  float m_fCooling;
  float m_dStepSize;
  float m_dTStepSize;
  float m_fStopStep;
  BOOL m_bAnneal;
};

void Increase(CAnnealInfo &info,Curve<TVec> *pC,int n,BOOL bPoint) {
  StepArray[n].Increase(bPoint);
  if(bPoint) {
    info.m_dStepSize*=1+STEP_CHANGE;
    }
  else {
    info.m_dTStepSize*=1+STEP_CHANGE;
    }
}

void Decrease(CAnnealInfo &info,Curve<TVec> *pC,int n,BOOL bPoint) {
  StepArray[n].Decrease(bPoint);
  if(bPoint)
    info.m_dStepSize*=1-STEP_CHANGE;
  else
    info.m_dTStepSize*=1-STEP_CHANGE;
}

/*
 *  Various moves for the annealing.
 *
 *  Anneal : pick a single point or tangent. move it, recompute energy.
 *  AllAnneal : move all points and tangents, recompute energy.
 *  random_dilate : pick a center and a scale factor, performs a dilation
 *                  on S^3.
 */

BOOL Anneal(CurveBundle<TVec> &rKnot,CAnnealInfo &info,float &dCurEnergy) {
  TVec vNew,vWas;
  int n,c;
  float dMin=s_dMinSegDistance;
  Curve<TVec> *pC;
  static int nTurns;
  bool bChangePoint;
  c=R(0,rKnot.curves()-1);
  pC=&(rKnot[c]);
  n=R(0,pC->nodes()-1);
  bChangePoint=R(0,1);
    //    cout << c << ',' << n << ',' << bChangePoint;

    if(bChangePoint)
	vNew=(*pC)[n].getPoint();
    else
	vNew=(*pC)[n].getTangent();

    vWas=vNew;

    if(info.m_dStepSize > dMin/10)
	{
	cout << 'M';
	info.m_dStepSize=dMin/10;
	}
    float d=(bChangePoint ? info.m_dStepSize : info.m_dTStepSize)
 	*(StepArray[n].Get(bChangePoint))
        ;
    // we calculate a random tangent vTan 
    TVec vTan = TVec(r(-d,d)*r(0,1), r(-d,d)*r(0,1), r(-d,d)*r(0,1), r(-d,d)*r(0,1));
    // Project to normal plane, so vTan is tangent to S^3
    TVec vBasePoint =(*pC)[n].getPoint();
    vBasePoint.normalize();
    vTan -= vTan.dot(vBasePoint)*(vBasePoint/vTan.norm());
    if(bChangePoint)
      {
        // move point in direction of vTan and project back to S^3
        vNew += vTan;
        vNew.normalize();
      }
    else 
      {
        vTan -= vNew.dot(vTan)/vTan.norm() * vNew;
        vNew += vTan;
        // Fix new tangent to be in TS^3
        vNew -= vNew.dot(vBasePoint)*(vBasePoint/vNew.norm());
        vNew.normalize();
      }

    BOOL ok;
    if(bChangePoint) {
      (*pC)[n].setPoint(vNew);
      // Fix tangent to be in TS^3
      vTan = (*pC)[n].getTangent();
      vTan -= vTan.dot(vNew)*vNew;
      vTan.normalize();
      (*pC)[n].setTangent(vTan);

      if(!CheckPoint(n,*pC)) {
        (*pC)[n].setPoint(vWas);
        Decrease(info,pC,n,bChangePoint);
        return FALSE;
      }
    }
    else {
      vNew.normalize();
      (*pC)[n].setTangent(vNew);
      if(!CheckTangent(n,*pC))
      {
	(*pC)[n].setTangent(vWas);
	Decrease(info,pC,n,bChangePoint);
	return FALSE;
      }
    }

// XXX : this is very slow. use a "single biarc" update (cf. CheckPoint...)
rKnot.make_default();

    float dNewEnergy=Energy(rKnot);
    if(dNewEnergy <= dCurEnergy)
	{
	dCurEnergy=dNewEnergy;
	Increase(info,pC,n,bChangePoint);
	return TRUE;
	}
    float p=exp(-(dNewEnergy-dCurEnergy)/info.m_fTemperature);
    if(r(0,1) <= p)
	{
	dCurEnergy=dNewEnergy;
	Increase(info,pC,n,bChangePoint);
	return TRUE;
	}
    if(bChangePoint)
	(*pC)[n].setPoint(vWas);
    else
	(*pC)[n].setTangent(vWas);
    Decrease(info,pC,n,bChangePoint);
    // note that since we may have updated distances above we need to do
    // something - an optimisation is that if the error of the cached distance
    // is non-zero, we haven't recalculated, so we subtract, otherwise we add
    return FALSE;
    }


// It seems that for "not so well" converged knots this leads
// faster to better shapes!
BOOL AnnealAll(CurveBundle<TVec> &rKnot,CAnnealInfo &info,float &dCurEnergy) {

  CurveBundle<TVec> rCopy(rKnot);
  vector<Biarc<TVec> >::iterator b;
  TVec tnow,pnow;
  int n;
  float d;

  int again_counter = 0;
shuffle_again:
  for (int i=0;i<rKnot.curves();++i) {
    n=0;
    // Displace points
    for (b=rKnot[i].begin();b!=rKnot[i].end();b++) {
      pnow = b->getPoint();
      d=info.m_dStepSize*(StepArray[n].Get(1));
      // cout << "d = " << d << endl;
      // we calculate a random tangent vTan 
      TVec vTan = TVec(r(-d,d)*r(0,1), r(-d,d)*r(0,1),
                       r(-d,d)*r(0,1), r(-d,d)*r(0,1));
      // Project to normal plane, so vTan is tangent to S^3
      pnow.normalize();
      vTan -= vTan.dot(pnow)*((pnow)/vTan.norm());
      // move point in direction of vTan and project back to S^3
      pnow += d*vTan;
      pnow.normalize();
      b->setPoint(pnow);
      ++n;
    }
    n=0;
    // Displace tangents
#if 1
    for (b=rKnot[i].begin();b!=rKnot[i].end();++b) {
      tnow = b->getTangent();
      d=info.m_dStepSize*(StepArray[n].Get(0));
      // cout << "d = " << d << endl;
      // we calculate a random tangent vTan 
      TVec vTan = TVec(r(-d,d)*r(0,1), r(-d,d)*r(0,1),
                       r(-d,d)*r(0,1), r(-d,d)*r(0,1));
      // Project to normal plane, so vTan is tangent to S^3
      pnow.normalize();
      vTan -= vTan.dot(pnow)*((pnow)/vTan.norm());
      // move point in direction of vTan and project back to S^3
      pnow += d*vTan;
      pnow.normalize();
      b->setTangent(tnow);
      ++n;
    }
  }
#else
  }
  rKnot.computeTangents();
#endif // Tangents

  for (int i=0;i<rKnot.curves();++i) {
    for (int n=0;n<rKnot[i].nodes();++n) {
      if (!(rKnot[i][n].isProper())) {
        rKnot = rCopy;
cout << "AGAIN\n";
        if (again_counter++ > 100) {
          cout << "GIVING UP\n";
          exit(1);
        }
        goto shuffle_again;
      }
    }
  }

/*
  // Anneal the matching point as well
  for (int i=0;i<rKnot.curves();++i)
    for (b=rKnot[i].begin();b!=rKnot[i].end();++b)
      b->make(r(.1,.9));
*/
  rKnot.make_default();
//  rKnot.normalize();
//  rKnot.make_default();

  float dNewEnergy=Energy(rKnot);
  if(dNewEnergy <= dCurEnergy) {
    dCurEnergy=dNewEnergy;
    for (int i=0;i<rKnot.curves();++i)
      for (int n=0;n<rKnot[i].nodes();++n)
        Increase(info,&rKnot[i],n,1);
    return TRUE;
  }
  float p=exp(-(dNewEnergy-dCurEnergy)/info.m_fTemperature);
  if(r(0,1) <= p) {
    dCurEnergy=dNewEnergy;
    for (int i=0;i<rKnot.curves();++i)
      for (int n=0;n<rKnot[i].nodes();++n)
        Increase(info,&rKnot[i],n,1);
    return TRUE;
  }
  rKnot = rCopy;
  for (int i=0;i<rKnot.curves();++i)
    for (int n=0;n<rKnot[i].nodes();++n)
      Decrease(info,&rKnot[i],n,1);

  // note that since we may have updated distances above we need to do
  // something - an optimisation is that if the error of the cached distance
  // is non-zero, we haven't recalculated, so we subtract, otherwise we add
  return FALSE;
}


// Dilation move functions

/*! Dilation with center center_v and a factor expalpha applied
    to a point p_v.

    expalpha is between 0 and inf. Identity map for expalpha = 1
 */
inline Vector4 dilate1Point(const Vector4 &p_v, Vector4 &center_v, double expalpha) {
  Vector4 p;
  if ((p_v-center_v).norm2() < 1e-9) return center_v;
  p = ( ( (expalpha*expalpha-1.0)*(p_v-center_v).norm2() +
          4.0*expalpha*(expalpha-1.0)*center_v.dot(p_v))*
         center_v + 4.0*expalpha*p_v
      )
      / ( (expalpha-1.0)*p_v + (expalpha+1.0)*center_v).norm2();
  p.normalize();
  return p;
}

/*! Dilation with center center_v and a factor alpha applied
    to curve knot.

    alpha is between -inf and inf. Identity map for alpha = 0
 */
void dilateOnS3(Curve<Vector4> *knot, Vector4 center_v, double alpha) {
  double expalpha = exp(alpha);
  vector<Biarc<Vector4> >::iterator it;
  Biarc<Vector4> b(Vector4(0,0,0,0),Vector4(1,0,0,0));
  for (it=knot->begin();it!=knot->end();++it)
    it->setPoint(dilate1Point(it->getPoint(),center_v,expalpha));
  knot->make_default();
}

#define rand_vec4() (Vector4(drand48(),drand48(),drand48(),drand48()))

BOOL random_dilate(CurveBundle<TVec> &rKnot,
                   CAnnealInfo &info,float &dCurEnergy) {

  CurveBundle<TVec> rCopy(rKnot);

  Curve<TVec> *knot;
  double dilation_factor = info.m_fTemperature*(1.-2.*drand48());

  Vector4 center = rand_vec4();
  while (center.norm2()<1e-9) center = rand_vec4();
  center.normalize();

  for (int i=0;i<rKnot.curves();++i) {
    knot = &(rKnot[i]);

    // pos scalefactor is a dilation
    // neg a contraction
    dilateOnS3(knot, center, dilation_factor);
    knot->computeTangents();

    vector<Biarc<Vector4> >::iterator it;
    Vector4 vTan, pnow;
    for (it=knot->begin();it!=knot->end();++it) {
      vTan = it->getTangent(); pnow = it->getPoint();
      vTan -= vTan.dot(pnow)*((pnow)/vTan.norm());
      it->setTangent(vTan);
    }
  }

  rKnot.make_default();

  float dNewEnergy=Energy(rKnot);
  if(dNewEnergy <= dCurEnergy) {
    dCurEnergy=dNewEnergy;
    return TRUE;
  }
  float p=exp(-(dNewEnergy-dCurEnergy)/info.m_fTemperature);
  if(r(0,1) <= p) {
    dCurEnergy=dNewEnergy;
    return TRUE;
  }

  rKnot = rCopy;

  return FALSE;
}




char *g_szPlotRoot;

void WriteBest(CurveBundle<TVec> &rKnot) {
  char buf[1000];

  sprintf(buf,"%s/best",g_szPlotRoot);
  rKnot.writePKF(buf);
  cout << "!" << Energy(rKnot) << "!" << flush;
}

inline void adjust_prob(float &prob, BOOL increase, float min = 1.0, float max = 1000.0) {
    if (increase && prob < max) prob +=1.0;
    else if (prob > min) prob -= 1.0;
    }

void DoAnneal(CurveBundle<TVec> &rKnot,CAnnealInfo &info)
    {
    int nGeneration=0;
    char buf[1000];
rKnot.make_default(); // MC
    float dEnergy=Energy(rKnot);
    float dMinEnergy;
    float dLogMax,dLogMin;
    int nSuccess=0;

    dMinEnergy=dLogMax=dLogMin=dEnergy;
    sprintf(buf,"%s/log",g_szPlotRoot);
    ofstream log(buf,ios::trunc);

    WriteBest(rKnot);

    // Creates the Step Size arrays in each component
    // MC rKnot.InitSteps();

    float random_dilate_prob= 4.0, anneal_all_prob=500.0, anneal_prob=550.0; // 1000.0 based
    float total_prob = random_dilate_prob + anneal_all_prob + anneal_prob;
    for( ; ; ++nGeneration)
	{

// if (nGeneration == 100) exit(0);

	if(nGeneration%info.m_nLogFrequency == 0)
	    {
	    log << nGeneration << ',' << dEnergy << ',' << dLogMax << ','
		<< dLogMin << ','
		<< info.m_fTemperature << ',' << info.m_dStepSize << ','
		<< info.m_dTStepSize << " -- ";
	    ShowStepSpread(rKnot,log);
	    log << endl;
	    dLogMax=dLogMin=dEnergy;
	    }
       	if(nGeneration%info.m_nWriteFrequency == 0)
	    {
	    sprintf(buf,"%s/%08d",g_szPlotRoot,nGeneration);
	    rKnot.writePKF(buf);
	    cout << Energy(rKnot)
	         << " s=" << info.m_dStepSize << '/' << info.m_dTStepSize
		 << " x=";
	    ShowStepSpread(rKnot,cout);
	    cout << " T=" << info.m_fTemperature
		 << " L=" << rKnot.length()
// MC		 << " d=" << rKnot.MinSegDistanceCache()
		 << " d=" << rKnot.thickness()
// MC		 << '/' << rKnot.MinSegDistance()
		 << " S=" << (float)nSuccess/info.m_nWriteFrequency
		 << endl;
                 cout << "probs:" << anneal_all_prob << ", " << anneal_prob << ", " << random_dilate_prob << endl;
	      ;
	    nSuccess=0;
	    if(info.m_dStepSize < info.m_fStopStep)
		{
		cout << info.m_dStepSize <<  " < " << info.m_fStopStep << endl;
		cout << "Finished!\n";
		exit(0);
		}
	    }
	if(info.m_bAnneal)
	    {
//rKnot.make_default();
	    double move_proba = drand48();
            BOOL bStepped;
            total_prob = random_dilate_prob + anneal_all_prob + anneal_prob;
            //cout << "probs:" << anneal_all_prob << ", " << anneal_prob << ", " << random_dilate_prob << endl;
            if (move_proba < anneal_all_prob / total_prob) {
 	      bStepped=AnnealAll(rKnot,info,dEnergy);
              adjust_prob(anneal_all_prob, bStepped, 200.0);
              //cout << "bStepped: " << bStepped << " " <<  anneal_all_prob << endl;
            } else if (move_proba < (anneal_all_prob + anneal_prob)/total_prob) {
 	      bStepped=Anneal(rKnot,info,dEnergy);
              adjust_prob(anneal_prob, bStepped, 200.0);
            } else {
              bStepped=random_dilate(rKnot,info,dEnergy);
              adjust_prob(random_dilate_prob, bStepped);
            }
	    if(dEnergy < dMinEnergy)
		{
		WriteBest(rKnot);
		dMinEnergy=dEnergy;
		}
	    if(dEnergy < dLogMin)
		dLogMin=dEnergy;
	    if(dEnergy > dLogMax)
		dLogMax=dEnergy;
	    if(bStepped)
		++nSuccess;
	    info.m_fTemperature*=(1-info.m_fCooling);
	    }
	}
cout << "again\n";
    }

void onkill(int sig)
    {
    cout << "Killed\n";
    exit(1);
    }

#ifndef TEST
int main(int argc,char **argv)
    {
    if(argc < 8)
	{
	cerr << argv[0] << " <source file> <plot file root> <temperature> <cooling> <anneal flag> <write frequency> <stop step> [<seed>]\n";
	exit(1);
	}
    CAnnealInfo info;

    const char *szSourceFile=argv[1];
    g_szPlotRoot=argv[2];
    info.m_fTemperature=atof(argv[3]);
    info.m_fCooling=atof(argv[4]);
    info.m_bAnneal=atoi(argv[5]);
    info.m_nWriteFrequency=atoi(argv[6]);
    info.m_fStopStep=atof(argv[7]);
    info.m_nLogFrequency=100;

    int nSeed;
    if(argc > 8)
	nSeed=atoi(argv[8]);
    else
	nSeed=time(NULL);

   cout << "Setting seed to " << nSeed << endl;
    srand(nSeed);

    CurveBundle<TVec> knot;
    knot.readPKF(szSourceFile);
    knot.link();
    knot.make_default();

    StepArray = new CStep[knot.nodes()];

    // MC knot.InitCache();
    // info.m_dStepSize=info.m_dTStepSize=knot.MinSegDistanceCache()/1000;
    info.m_dStepSize=info.m_dTStepSize=knot.thickness()/1000.;
    signal(SIGINT,onkill);
    if(info.m_bAnneal)
	DoAnneal(knot,info);
    return 0;
    }
#else // TEST

#define SNAP_FREQ 100

int main(int argc, char** argv) {

  cout << "Load " << argv[1];
  Curve<Vector4> knot(argv[1]);
  knot.link();
  knot.make_default();
  cout << " [ok]\n";
 // dilateOnS3(&knot, Vector4(0,0,0,1), -1);
  // knot.computeTangents();

  int no = 1;
  char buf[1024];
  double thick, best_thick;
  best_thick = thick = knot.thickness();

  cout << "Thickness : " << thick << endl;
  cout << "Thickness fast : " << knot.thickness_fast() << endl;


  Curve<Vector4> best_knot = knot;

  while(1) {
    no++;
    thick = knot.thickness_fast();
    random_dilate(&knot, 0.08);
    // cout << no << " Thickness : " << thick << endl;
    if (thick > best_thick) {
      best_thick = thick;
      best_knot = knot;
      cout << "!!!" << no << " Thickness : " << thick << endl;
    } 
    else {knot = best_knot;}
    if (no%SNAP_FREQ) {
      sprintf(buf,"snap%05d.pkf",no);
//      cout << "Thickness : " << thick << endl;
      knot.writePKF(buf);
    }
  }

}
#endif // TEST
