/*

Anneal a knot

Copyright (C) 1997 Ben Laurie
*/

#include <sys/types.h>
#include <stdio.h>
#include <limits.h>

#define TVec Vector3

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
//  float off_equi;
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
//  off_equi = (rKnot[0].maxSegDistance() / rKnot[0].minSegDistance());
  return rKnot.length()/s_dMinSegDistance; // +0.0001*(off_equi);
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

BOOL Anneal(CurveBundle<TVec> &rKnot,CAnnealInfo &info,float &dCurEnergy) {
  TVec vNew,vWas;
  int n,c;
  float dMin=s_dMinSegDistance;
  Curve<TVec> *pC;
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
    //vNew+=TVec(r(-d,d)*r(0,1),r(-d,d)*r(0,1),r(-d,d)*r(0,1));
    vNew+=TVec(d*r(-1,1),d*r(-1,1),d*r(-1,1));

    if(bChangePoint) {
      (*pC)[n].setPoint(vNew);
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

shuffle_again:
  for (int i=0;i<rKnot.curves();++i) {
    n=0;
    // Displace points
    for (b=rKnot[i].begin();b!=rKnot[i].end();b++) {
      pnow = b->getPoint();
      d=info.m_dStepSize*(StepArray[n].Get(1));
      // cout << "d = " << d << endl;
      TVec vTan = TVec(r(-d,d)*r(0,1), r(-d,d)*r(0,1), r(-d,d)*r(0,1));
      pnow += vTan;
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
      TVec vTan = TVec(r(-d,d)*r(0,1), r(-d,d)*r(0,1), r(-d,d)*r(0,1));
      tnow += vTan;
      tnow.normalize();
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
  rKnot.normalize();
  rKnot.make_default();

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


char *g_szPlotRoot;

void WriteBest(CurveBundle<TVec> &rKnot) {
  char buf[1000];

  sprintf(buf,"%s/best",g_szPlotRoot);
  rKnot.writePKF(buf);
  cout << "!" << Energy(rKnot) << "!" << flush;
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
	if(info.m_nWriteFrequency > 1000 && nGeneration%1000 == 0)
	    {
//       rKnot.make_default(); // MC
	    float dL=rKnot.length();
	    info.m_dStepSize/=dL;
	    info.m_dTStepSize/=dL;
	    s_dMinSegDistance/=dL;

//	    rKnot.normalize();
//       rKnot.make_default(); // MC
	    }

       	if(nGeneration%info.m_nWriteFrequency == 0)
	    {
//       rKnot.make_default(); // MC
	    float dL=rKnot.length();
	    info.m_dStepSize/=dL;
	    info.m_dTStepSize/=dL;
	    s_dMinSegDistance/=dL;
//	    rKnot.normalize();
//       rKnot.make_default(); // MC
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
	      ;
	    nSuccess=0;
	    if(info.m_dStepSize/rKnot.length() < info.m_fStopStep)
		{
		cout << info.m_dStepSize << "/" << rKnot.length() << "=" << info.m_dStepSize/rKnot.length() << " < " << info.m_fStopStep << endl;
		cout << "Finished!\n";
		exit(0);
		}
	    }
	if(info.m_bAnneal)
	    {
//rKnot.make_default();
	    BOOL bStepped=AnnealAll(rKnot,info,dEnergy);
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
    if (!knot.readPKF(szSourceFile)) {
      cerr << "[Err] Problem reading file : " << szSourceFile << endl;
      exit(1);
    }
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
