/*

 a simple gradient flow

*/

#include <iostream>  // I/O 
#include <fstream>   // file I/O
#include <iomanip>   // format manipulation

#include <string>
#include <math.h>
#include <sys/types.h>
#include <stdio.h>
#include <limits.h>

#ifdef COMPUTE_IN_R4
#include "Vector4.h"
#define TVec Vector4
#else
#define TVec Vector3
#endif

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


static double s_dMinSegDistance;
static double s_dInitialStep;


#define POINT_MODE 0x1
#define TANGENT_MODE 0x2

#define ABS(x) (((x)>0)?(x):-(x))
#define entry(i,j) ((i)*6+(j))
#define CHKPT  {cout << "CHK " __FILE__ << ":" << __LINE__ << endl;}
#define SETDEFAULTS(kc,name) {kc.SetName(name); kc.SetEtic("default"); kc.SetCite("default"); kc.SetHistory("default");}


double oscfactor = 0;

#define KNOT knotContainer[0]

/*
double oscillationEnergy(CurveBundle<TVec> &knotContainer) {
   int noOfNodes = KNOT.nodes();
   double c1,c2, d1,d2, osc=0;
   KNOT.Curvature(c1,c2,0);
   d1=c1;
   d2=c2;
   for(int row=0;row<noOfNodes;row++) {
            KNOT.Curvature(c1,c2,row);
            osc += fabs(c1-d2);
            osc += fabs(c2-c1);
            d2 = c2;
            }
   osc += fabs(c2-d1);
   return osc;
   }


*/

double energy(CurveBundle<TVec> &knot) {
    double thickness = knot.thickness();
    double length = knot.length();
    return length / thickness ;
}

/*
void saveGradient(const char *name, double *gradient, int noOfRows) {
   ofstream file;
   file.open(name, ios::out);
   for (int row=0; row < noOfRows; row++) {
      for (int col=0; col < 6; col ++) {
         file << gradient[entry(row,col)] << " ";
         }
      file << endl;
      }
   file.close();
}


void computeCentralGradientP(double *gradient,CurveBundle<TVec> &knotContainer,const double deltax=10e-10) {
   int noOfNodes = KNOT.nodes();
   TVec v, vold;
   double leftEnergy,rightEnergy;
   cout << "Computing central gradient P..." << endl;
   for(int row=0;row<noOfNodes;row++) {
            for(int i=0;i<3;i++) {
               vold = v = KNOT[row].getPoint();
               v[i] -= deltax;
               KNOT[row].setPoint(v);
               leftEnergy = energy(knotContainer);
               v[i] += 2.0 * deltax;
               KNOT[row].setPoint(v);
               rightEnergy = energy(knotContainer);
               gradient[entry(row,i)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
               KNOT[row].setPoint(vold);
            }

            // Tangents
            for(int i=0;i<3;i++) {
               gradient[entry(row,i+3)] = 0;
               }

            if (row % 10 ==0) {
               cout << "." << flush;
               }
            }
   cout << endl;
   }

  
void computeCentralGradientT(double *gradient,CurveBundle<TVec> &knotContainer, const double deltax=10e-10) {
   int noOfNodes = KNOT.nodes();
   TVec v, vold;
   double leftEnergy,rightEnergy;
   cout << "Computing central gradient T..." << endl;
   for(int row=0;row<noOfNodes;row++) {
            for(int i=0;i<3;i++) {
               gradient[entry(row,i)] = 0;
            }

            // Tangents
            for(int i=0;i<3;i++) {
               vold = v = KNOT[row].getTangent();
               v[i] -= deltax;
               v.normalize();
               KNOT[row].setTangent(v);
               leftEnergy = energy(knotContainer);
               v[i] += 2.0 * deltax;
               v.normalize();
               KNOT[row].setTangent(v);
               rightEnergy = energy(knotContainer);
               gradient[entry(row,i+3)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
               KNOT[row].setTangent(vold);
               }

            if (row % 10 ==0) {
               cout << "." << flush;
               }
            }
   cout << endl;
   }

*/

void computeCentralGradientWeak(double *gradient,CurveBundle<TVec> &knotContainer, const int mode = (POINT_MODE | TANGENT_MODE),
      const double deltax=10e-10) {
   //Like Central gradient but do not try to escape from local directional minima. Set gradient to 0 instead.
   int noOfNodes = KNOT.nodes();
   TVec v, vold;
   double leftEnergy,rightEnergy,centralEnergy;
   cout << "Computing central gradient weak... " << ((POINT_MODE & mode)?'P':' ') << ((TANGENT_MODE & mode)?'T':' ') <<  endl;
   centralEnergy = energy(knotContainer);
   for(int row=0; row < noOfNodes; row++) {
      for(int i=0;i<6;i++) {
         gradient[entry(row,i)] = 0;
         }
      }
   for(int row=0;row<noOfNodes;row++) {
            if (mode & POINT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getPoint();
                  v[i] -= deltax;
                  knotContainer[0][row].setPoint(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  KNOT[row].setPoint(v);
                  rightEnergy = energy(knotContainer);
                  if ((leftEnergy > centralEnergy) && (rightEnergy > centralEnergy)) {
                     //we are stuck in a local directional minimum.
                     gradient[entry(row,i)] = (rightEnergy - leftEnergy) / (200.0 * deltax);
                     }
                  else  {
                     gradient[entry(row,i)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                     }
                  KNOT[row].setPoint(vold);
                  }
               }

            // Tangents
            if (mode & TANGENT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getTangent();
                  v[i] -= deltax;
                  v.normalize();
                  KNOT[row].setTangent(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  v.normalize();
                  KNOT[row].setTangent(v);
                  rightEnergy = energy(knotContainer);
                  if ((leftEnergy > centralEnergy) && (rightEnergy > centralEnergy)) {
                     //we are stuck in a local directional minimum.
                     gradient[entry(row,i+3)] = (rightEnergy - leftEnergy) / (200.0 * deltax);
                     }
                  else  {
                     gradient[entry(row,i+3)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                     }
                  KNOT[row].setTangent(vold);
                  }
               }

            if (row % 10 ==0) {
               cout << "." << flush;
               }
            }
   cout << endl;
   }

/*
void computeCentralGradientWeakOnSubset(double *gradient, CurveBundle<TVec> &knotContainer, int startNode, int endNode,
      const int mode = (POINT_MODE | TANGENT_MODE), const double deltax=10e-10) {
   int noOfNodes = KNOT.nodes();
   TVec v, vold;
   double leftEnergy,rightEnergy,centralEnergy;
   cout << "Computing central gradient weak... " << ((POINT_MODE & mode)?'P':' ') << ((TANGENT_MODE & mode)?'T':' ') <<  endl;
   centralEnergy = energy(knotContainer);
   startNode= (startNode % noOfNodes) ;
   endNode = (endNode % noOfNodes) ;
   cout << "  between " << startNode << " and " << endNode << endl;
   for(int row=0; row < noOfNodes; row++) {
      for(int i=0;i<6;i++) {
         gradient[entry(row,i)] = 0;
      }
   }
   for(int row=startNode ; row  != endNode; row = (row+1) % noOfNodes) {
            if (mode & POINT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getPoint();
                  v[i] -= deltax;
                  KNOT[row].setPoint(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  KNOT[row].setPoint(v);
                  rightEnergy = energy(knotContainer);
                  if ((leftEnergy > centralEnergy) && (rightEnergy > centralEnergy)) {
                     //we are stuck in a local directional minimum.
                     gradient[entry(row,i)] = (rightEnergy - leftEnergy) / (200.0 * deltax);
                     }
                  else  {
                     gradient[entry(row,i)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                     }
                  KNOT[row].setPoint(vold);
                  }
               }

            // Tangents
            if (mode & TANGENT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getTangent();
                  v[i] -= deltax;
                  v.normalize();
                  KNOT[row].setTangent(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  v.normalize();
                  KNOT[row].setTangent(v);
                  rightEnergy = energy(knotContainer);
                  if ((leftEnergy > centralEnergy) && (rightEnergy > centralEnergy)) {
                     //we are stuck in a local directional minimum.
                     gradient[entry(row,i+3)] = (rightEnergy - leftEnergy) / (200.0 * deltax);
                     }
                  else  {
                     gradient[entry(row,i+3)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                     }
                  KNOT[row].setTangent(vold);
                  }
               }

            if (row % 10 ==0) {
               cout << "." << flush;
               }
            }
   cout << endl;
   }



void computeCentralGradientOnSubset(double *gradient, CurveBundle<TVec> &knotContainer, int startNode, int endNode,
      const int mode = (POINT_MODE | TANGENT_MODE), const double deltax=10e-10) {
   int noOfNodes = KNOT.nodes();
   TVec v, vold;
   double leftEnergy,rightEnergy;
   cout << "Computing central gradient... " << ((POINT_MODE & mode)?'P':' ') << ((TANGENT_MODE & mode)?'T':' ') << endl;
   startNode= (startNode % noOfNodes) ;
   endNode = (endNode % noOfNodes) ;
   cout << "  between " << startNode << " and " << endNode << endl;
   for(int row=0; row < noOfNodes; row++) {
      for(int i=0;i<6;i++) {
         gradient[entry(row,i)] = 0;
      }
   }
   for(int row=startNode ; row  != endNode; row = (row+1) % noOfNodes) {
            if (mode & POINT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getPoint();
                  v[i] -= deltax;
                  KNOT[row].setPoint(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  KNOT[row].setPoint(v);
                  rightEnergy = energy(knotContainer);
                  gradient[entry(row,i)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                  KNOT[row].setPoint(vold);
                  }
               }

            // Tangents
            if (mode & TANGENT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getTangent();
                  v[i] -= deltax;
                  v.normalize();
                  KNOT[row].setTangent(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  v.normalize();
                  KNOT[row].setTangent(v);
                  rightEnergy = energy(knotContainer);
                  gradient[entry(row,i+3)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                  KNOT[row].setTangent(vold);
                  }
               }

            if (row % 10 ==0) {
               cout << "." << flush;
               }
            }
   cout << endl;
   }



void computeCentralGradient(double *gradient,CurveBundle<TVec> &knotContainer, const int mode = (POINT_MODE | TANGENT_MODE),
      const double deltax=10e-10) {
   int noOfNodes = knot->nodes();
   TVec v, vold;
   double leftEnergy,rightEnergy;
   cout << "Computing central gradient... " << ((POINT_MODE & mode)?'P':' ') << ((TANGENT_MODE & mode)?'T':' ') <<  endl;
   for(int row=0; row < noOfNodes; row++) {
      for(int i=0;i<6;i++) {
         gradient[entry(row,i)] = 0;
         }
      }
   for(int row=0;row<noOfNodes;row++) {
            if (mode & POINT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getPoint();
                  v[i] -= deltax;
                  knot[row].setPoint(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  knot[row].setPoint(v);
                  rightEnergy = energy(knotContainer);
                  gradient[entry(row,i)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                  knot[row].setPoint(vold);
                  }
               }

            // Tangents
            if (mode & TANGENT_MODE) {
               for(int i=0;i<3;i++) {
                  vold = v = KNOT[row].getTangent();
                  v[i] -= deltax;
                  v.normalize();
                  knot-[row].setTangent(v);
                  leftEnergy = energy(knotContainer);
                  v[i] += 2.0 * deltax;
                  v.normalize();
                  knot-[row].setTangent(v);
                  rightEnergy = energy(knotContainer);
                  gradient[entry(row,i+3)] = (rightEnergy - leftEnergy) / (2.0 * deltax);
                  knot-[row].setTangent(vold);
                  }
               }

            if (row % 10 ==0) {
               cout << "." << flush;
               }
            }
   cout << endl;
   }

void computeForwardGradient(double *gradient,CurveBundle<TVec> &knotContainer, const double deltax = 10e-10) {
   Curve<TVec> *knot = NULL;
   knot =  knotContainer[0];
   int noOfNodes = knot->nodes();
   TVec v, vold;
   double currentEnergy = energy(knotContainer);
   cout << "Computing forward gradient..." << endl;
   for(int i=0;i<noOfNodes;i++) {
            vold = v = KNOT[i].getPoint();
            v += TVec(deltax,0,0);
            knot[i].setPoint(v);
            gradient[entry(i,0)] = (energy(knotContainer) -currentEnergy) / deltax;
            knot[i].setPoint(vold);
            // cout << i << " : " << gradient[entry(i,0)] << " ";

            vold = v = KNOT[i].getPoint();
            v += TVec(0,deltax,0);
            knot[i].setPoint(v);
            gradient[entry(i,1)] = (energy(knotContainer) -currentEnergy) / deltax;
            knot[i].setPoint(vold);
            // cout << gradient[entry(i,1)] << " ";

            vold = v = KNOT[i].getPoint();
            v += TVec(0,0,deltax);
            knot[i].setPoint(v);
            gradient[entry(i,2)] = (energy(knotContainer) -currentEnergy) / deltax;
            knot[i].setPoint(vold);
            // cout << gradient[entry(i,2)] << " ";

            // Tangents
            vold = v = KNOT[i].getTangent();
            v += TVec(deltax,0,0);
            v.normalize();
            knot-[i].setTangent(v);
            gradient[entry(i,3)] = (energy(knotContainer) -currentEnergy) / deltax;
            knot-[i].setTangent(vold);
            // cout << gradient[entry(i,3)] << " ";

            vold = v = KNOT[i].getTangent();
            v += TVec(0,deltax,0);
            v.normalize();
            knot-[i].setTangent(v);
            gradient[entry(i,4)] = (energy(knotContainer) -currentEnergy) / deltax;
            knot-[i].setTangent(vold);
            // cout << gradient[entry(i,4)] << " ";

            vold = v = KNOT[i].getTangent();
            v += TVec(0,0,deltax);
            v.normalize();
            knot-[i].setTangent(v);
            gradient[entry(i,5)] = (energy(knotContainer) -currentEnergy) / deltax;
            knot-[i].setTangent(vold);
            // cout << gradient[entry(i,5)] << endl;
            if (i % 10 ==0) {
               cout << "." << flush;
               }
            }
   cout << endl;
   }
*/

double findAbsMax(const double *x, const int len) {
   double max = ABS(x[0]);
   for (int i=1; i< len; i++) {
      if (ABS(x[i])>max) {
         max = ABS(x[i]);
         }
      }
   return max;
}

/*
CurveBundle<TVec> lineSearchP(CurveBundle<TVec> &knotContainer, double *gradient, const double min, double &factor) {
   CurveBundle<TVec> knotContainerCopy;
   Curve<TVec> *knot = NULL;
   double currentEnergy , newEnergy;
   knot = knotContainer[0];
   int noOfNodes = knot->nodes();
   int i,j;
   TVec v;
   currentEnergy = energy (knotContainer);
   if (factor < 1.0) {
      factor *=3.0;
      }
   while (factor > min) {
      cout << "LinesearchP factor: "<< factor << endl;
      cout << "copy " << flush;
      knotContainerCopy = knotContainer;
      knot = knotContainerCopy[0];
      cout << "done" <<endl;
      try {
              for(j=0;j<noOfNodes;j++) {
                 v = KNOT[j].getPoint();
                 for(i=0;i<3;i++) {
                    v[i] -= factor * gradient[entry(j,i)];
                    }
                 knot[j].setPoint(v);

                 if (j % 10 == 0) {
                     cout << "." << flush;
                     }
                 }
               newEnergy = energy(knotContainerCopy);
              cout << endl << " " << newEnergy << " ?<? " << currentEnergy << endl;
              if ( newEnergy < currentEnergy) {
                 return knotContainerCopy;
                 }
         }
      catch (std::string s) {
         cerr << s << endl;
         }
      factor /= 2.0;
      }
   cout << "Giving up..." << endl;
   SETDEFAULTS(knotContainerCopy,"Curr");
   knotContainerCopy.writePKF("stuck.pkf");
   return knotContainer;
}

CurveBundle<TVec> lineSearchT(CurveBundle<TVec> &knotContainer, double *gradient, const double min, double &factor) {
   CurveBundle<TVec> knotContainerCopy;
   Curve<TVec> *knot = NULL;
   double currentEnergy , newEnergy;
   knot = knotContainer[0];
   int noOfNodes = knot->nodes();
   int i,j;
   TVec v;
   currentEnergy = energy (knotContainer);
   if (factor < 1.0) {
      factor *=3.0;
      }
   while (factor > min) {
      cout << "LinesearchT factor: "<< factor << endl;
      cout << "copy " << flush;
      knotContainerCopy = knotContainer;
      knot = knotContainerCopy[0];
      cout << "done" <<endl;
      try {
              for(j=0;j<noOfNodes;j++) {
                 // Tangents
                 v = KNOT[j].getTangent();
                 for(i=0;i<3;i++) {
                    v[i] -= factor * gradient[entry(j,i+3)];
                    v.normalize();
                    }
                 knot-[j].setTangent(v);
                 if (j % 10 == 0) {
                     cout << "." << flush;
                     }
                 }
               newEnergy = energy(knotContainerCopy);
              cout << endl << " " << newEnergy << " ?<? " << currentEnergy << endl;
              if ( newEnergy < currentEnergy) {
                 return knotContainerCopy;
                 }
         }
      catch (std::string s) {
         cerr << s << endl;
         }
      factor /= 2.0;
      }
   cout << "Giving up..." << endl;
   SETDEFAULTS(knotContainerCopy,"Curr");
   knotContainerCopy.writePKF("stuck.pkf");
   return knotContainer;
}


CurveBundle<TVec> lineSearchOnSubset(CurveBundle<TVec> &knotContainer, double *gradient, double min, int startNode, int endNode, double &factor,
       const int mode = (POINT_MODE | TANGENT_MODE)) {
   CurveBundle<TVec> knotContainerCopy;
   Curve<TVec> *knot = NULL;
   double currentEnergy , newEnergy;
   knot = knotContainer[0];
   int noOfNodes = knot->nodes();
   int i,j;
   TVec v;
   currentEnergy = energy (knotContainer);
   if (factor < 1.0) {
      factor *=3.0;
      }
   startNode= (startNode % noOfNodes) ;
   endNode = (endNode % noOfNodes) ;

   while (factor > min) {
      cout << "Linesearch factor "<< ((POINT_MODE & mode)?'P':' ') << ((TANGENT_MODE & mode)?'T':' ') << ": " << factor << endl;
      cout << "  between " << startNode << " and " << endNode << endl;
      cout << "copy " << flush;
      knotContainerCopy = knotContainer;
      knot = knotContainerCopy[0];
      cout << "done" <<endl;
      try {
              for(int row=startNode ; row  != endNode; row = (row+1) % noOfNodes) {
                 if (mode & POINT_MODE) {
                    v = KNOT[row].getPoint();
                    for(i=0;i<3;i++) {
                      v[i] -= factor * gradient[entry(row,i)];
                      }
                    knot[row].setPoint(v);
                    }

                 // Tangents
                 if (mode & TANGENT_MODE) {
                    v = KNOT[row].getTangent();
                    for(i=0;i<3;i++) {
                       v[i] -= factor * gradient[entry(row,i+3)];
                       v.normalize();
                       }
                     knot-[row].setTangent(v);
                     }
                 if (row % 10 == 0) {
                     cout << "." << flush;
                     }
                 }
               newEnergy = energy(knotContainerCopy);
              cout << endl << " " << newEnergy << "   ?<?" << endl << " " << currentEnergy << endl;
              if ( newEnergy < currentEnergy) {
                 return knotContainerCopy;
                 }
         }
      catch (std::string s) {
         cerr << s << endl;
         }
      factor /= 2.0;
      }
   cout << "Giving up..." << endl;
   SETDEFAULTS(knotContainerCopy,"Curr");
   knotContainerCopy.writePKF("stuck.pkf");
   return knotContainer;
}

*/

CurveBundle<TVec> lineSearch(CurveBundle<TVec> &knotContainer, double *gradient, double min, double &factor,
   const int mode = (POINT_MODE | TANGENT_MODE)) {
   CurveBundle<TVec> knotContainerCopy; 
   Curve<TVec> knot;
   double currentEnergy , newEnergy;
   int noOfNodes = KNOT.nodes();
   int i,j;
   TVec v;
   currentEnergy = energy (knotContainer);
   if (factor < 1.0) {
      factor *=3.0;
      }
   while (factor > min) {
      cout << "Linesearch factor " << ((POINT_MODE & mode)?'P':' ') << ((TANGENT_MODE & mode)?'T':' ') << " : "<< factor << endl;
      cout << "copy " << flush;
      knotContainerCopy = knotContainer;
      knot = knotContainerCopy[0];
      cout << "done" <<endl;
      try {
              for(j=0;j<noOfNodes;j++) {
                 if (mode & POINT_MODE) {
                    v = knot[j].getPoint();
                    for(i=0;i<3;i++) {
                       v[i] -= factor * gradient[entry(j,i)];
                       }
                    knot[j].setPoint(v);
                    }

                 // Tangents
                 if (mode & TANGENT_MODE) {
                    v = knot[j].getTangent();
                    for(i=0;i<3;i++) {
                       v[i] -= factor * gradient[entry(j,i+3)];
                       v.normalize();
                       }
                    knot[j].setTangent(v);
                    }
                 if (j % 10 == 0) {
                     cout << "." << flush;
                     }
                 }
               newEnergy = energy(knotContainerCopy);
              cout << endl << " " << newEnergy << " ?<? " << currentEnergy << endl;
              if ( newEnergy < currentEnergy) {
                 return knotContainerCopy;
                 }
         }
      catch (std::string s) {
         cerr << s << endl;
         }
      factor /= 2.0;
      }
   cout << "Giving up..." << endl;
   //SETDEFAULTS(knotContainerCopy,"Curr");
   knotContainerCopy.writePKF("stuck.pkf");
   //hgassert(! "I'm stuck\n");
}

int main(int argc,char **argv) {
   CurveBundle<TVec> knotContainer;
   double factorOnSubsetP=10e-6, 
         factorOnSubsetT=10e-6, 
         Tfactor=10e-7, Pfactor = 10e-11;
   int spread = 3, step = 2, globaliters=10;
   if(argc < 2) {
      std::string  filename;
      cout << "Manual configuration" << endl;
      cout << "PKF-Filename? ";
      cin >> filename;
      knotContainer.readPKF(filename.c_str());
      knotContainer.link();
      knotContainer.make_default();
      cout << "How many global iterations per round? ";
      cin >> globaliters;
      cout << "Stepsize for local iterations? ";
      cin >> step;
      cout << "Spread for local iterations? ";
      cin >> spread;
      }
   else {
      const char *szSourceFile=argv[1];
      knotContainer.readPKF(szSourceFile);
      knotContainer.link();
      knotContainer.make_default();
      }
   int noOfNodes = knotContainer.nodes();
   double * gradient= NULL;
   gradient = (double *) malloc (sizeof(double)*noOfNodes*6);
   cout.precision(20);
   for (long i=0;1;i++) {
      for (int j = 0; j < globaliters; j++) {
         cout << "Acting globally" << endl;
         computeCentralGradientWeak(gradient, knotContainer, POINT_MODE, 10e-12);
         cout << "Max absolute value in gradient:" << findAbsMax(gradient,noOfNodes*6) << endl;
         // saveGradient("curr-gradientP.dat",gradient,noOfNodes);
         knotContainer = lineSearch(knotContainer,gradient, 10e-70, Pfactor, POINT_MODE);

         //computeCentralGradientWeak(gradient, knotContainer, TANGENT_MODE, 10e-12);
//         cout << "Max absolute value in gradient:" << findAbsMax(gradient,noOfNodes*6) << endl;
         // saveGradient("curr-gradientT.dat",gradient,noOfNodes);
         //knotContainer = lineSearch(knotContainer, gradient, 10e-70, Tfactor, TANGENT_MODE);

         CHKPT;
         //SETDEFAULTS(knotContainer,"Curr");
         knotContainer.writePKF("curr.pkf");
         }
/*
      for (int j = 0; j < noOfNodes; j+=step) {
         double maxgrad;
         cout << "Acting locally" << endl;  
         oscfactor = 0.1;
         cout << "oscfactor: " << oscfactor << endl;  
         computeCentralGradientWeakOnSubset(gradient, knotContainer,j+1,j+spread,POINT_MODE,10e-12);
         maxgrad = findAbsMax(gradient,noOfNodes*6);
         factorOnSubsetP = 10e-8/maxgrad + factorOnSubsetP;
         cout << "Max absolute value in gradient:" << maxgrad << endl;
         // saveGradient("curr-gradientSub.dat",gradient,noOfNodes);
         knotContainer = lineSearchOnSubset(knotContainer, gradient, 10e-16, j+1, j+spread, factorOnSubsetP,POINT_MODE);

         computeCentralGradientWeakOnSubset(gradient, knotContainer,j,j+spread+1, TANGENT_MODE,10e-12);
         maxgrad = findAbsMax(gradient,noOfNodes*6);
         factorOnSubsetT = 10e-8/maxgrad +factorOnSubsetT;
         cout << "Max absolute value in gradient:" << maxgrad << endl;
         // saveGradient("curr-gradientSub.dat",gradient,noOfNodes);
         knotContainer = lineSearchOnSubset(knotContainer, gradient, 10e-16, j, j+spread+1, factorOnSubsetT, TANGENT_MODE);

         SETDEFAULTS(knotContainer,"Curr");
         knotContainer.writePKF("curr.pkf");
         oscfactor = 0.1;
         }
*/
      }
    knotContainer.writePKF("opt.pkf");
    free(gradient);
    return 0;
    }
