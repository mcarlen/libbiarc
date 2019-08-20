#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __FreeBSD__
# include <dirent.h>
#else
# include <sys/dir.h>
#endif
#include <math.h>

using namespace std;

int main(int argc, char** argv) {

  if(argc != 3) {
    cerr << "Usage : " << argv[0] << " <Steadystate file directory> <Knot directory>\n";
    cerr << "\nWalks through the directories and read\n" //"kX.YZ" and reads\n"
	    	<< " from file results/sim_N_GammaMid.dat";
    exit(1);
  }

  char tmp[1024];
  //char line[1024];

  ifstream in;
  char filename[1024];
  char call[1024];
  //char KnotName[40];
  float real_part[2];
  int real_ind;
  float temp_number;

  // Vars to sort the eigvals
  int NSS;
  //int STAB[3];
  float Axis[3];
  float EVal;

  // number of knots in PKFdir?
  const char* szDir = argv[1];
  const char* szKnot = argv[2];
  DIR *pDir;

  clog << "Enter directory "<<szDir << endl;
  pDir = opendir(szDir);
  if (!pDir) {
    clog << "\t\t[FAILED]\n"<<flush;
    exit(3);
  }

#ifdef __FreeBSD__
  struct dirent *pEnt;
#else
  struct direct *pEnt;
#endif
  for (; (pEnt=readdir(pDir)) ;) {
    // Skip directories
    if (pEnt->d_type != DT_DIR)
      continue;

    if (!strncmp(&pEnt->d_name[2],".",1)) {
      clog << "Enter " << pEnt->d_name << flush;
      sprintf(filename,"%s/%s/results/sim_N_GammaMid.dat",szDir,pEnt->d_name);
      in.open(filename,ios::in);
      if (!in.good()) {
	cerr << "Problem reading from " << filename << endl;
	return 3;
      }

      // eat up lines till N=550
      in.getline(tmp,sizeof tmp);
      while (strncmp(tmp,"550 ",4)) in.getline(tmp,sizeof tmp);

      // Parse that line
      // No nodes
      strtok(tmp," ");
      strtok(NULL," ");
      strtok(NULL," ");

      NSS = atoi(strtok(NULL," "));
      for (int i=0;i<NSS;i++) {
	// Read in for three !!
	EVal = atof(strtok(NULL," "));
	Axis[0] = atof(strtok(NULL," "));
	Axis[1] = atof(strtok(NULL," "));
	Axis[2] = atof(strtok(NULL," "));
	real_ind = 0;
	for (int k=0;k<3;k++) {
	  temp_number = atof(strtok(NULL," "));
	  if (fabsf(temp_number)>1e-8)
	    real_part[real_ind++] = temp_number;
	}

	// Eat up rest (img part, helix stuff)
	for (int k=0;k<7;k++)
	  strtok(NULL," ");

	if (real_part[0]*real_part[1]>0) {
	  if (real_part[0]>0 && real_part[1]>0) {
	    Axis[0]*=-1.0; Axis[1]*=-1.0; Axis[2]*=-1.0;
	  }
	  sprintf(call,"./projected_dij %f %f %f %f %s/%s.pkf >>results.txt 2>/dev/null",
		  EVal,Axis[0],Axis[1],Axis[2],szKnot,pEnt->d_name);
	  system(call);
	}
      }
      in.close();
      clog << "\t\t[OK]\n";
    }
  }
  closedir(pDir);

  return 1;
}

#endif
