#include "plot_funcs.h"

// Sample curve from arclength from to arclength to using N samples
// and store it in the ALREADY initialized pts, tgs arrays
void samplePtTg(const float from, const float to,
                const int N, const Curve<TVec> &c,
						    TVec* pts, TVec* tgs) {
	float length = c.length();
	float sigma, alen;
  for (int i=0;i<N;i++) {
    sigma = from+(float)i/(float)N*
            (to-from);
    alen = sigma*length;
    if (alen>length) {
      cout << alen << " - "; alen = length;
      cout << i << " -- " << (float)i/(float)N<<endl;
    }
    pts[i] = c.pointAt(alen);
    tgs[i] = c.tangentAt(alen);
  }
}

// Compute the pt functions for a given Pt, Tg sampling
// store it in already init table (1D array)
// We explicitly compute the pt stuff
// here for perfomance. Using curve->radius_pt
// would be to slow!
void computePT(const int w, const int h,
               const TVec* Pts_i, const TVec* Tg_i,
               const TVec* Pts_j, const TVec* Tg_j,
							 const float thickness,
							 float* table,
							 float* min, float *max
               ) {
  float lmin = 1e10, lmax = -1e10;
	float Dlen, cosa, sina, cur;
	TVec from, to, tg, D;

  for (int j=0;j<h;j++) {
    for (int i=0;i<w;i++) {

      from = Pts_i[i]; to = Pts_j[j]; tg = Tg_i[i];
      D = to - from;

      Dlen = D.norm();
      cosa = tg.dot(D)/Dlen;
      sina = sqrt(1.0-cosa*cosa);
        
      if (sina==0) cur = 0.0;
      else cur = thickness*sina/Dlen;

      if (cur<0.) cur = 0.;
      else if (cur>1.) cur = 1.;

      lmin = (cur<lmin?cur:lmin);
      lmax = (cur>lmax?cur:lmax);
      table[i+j*w] = cur;
    }
  }
	*min = lmin;
	*max = lmax;
}

void computePP(const int w, const int h,
               const TVec* Pts_i, const TVec* Pts_j, 
							 const float thickness,
							 float* table,
							 float* min, float *max
               ) {
	TVec from, to;
	float cur, lmin = 1e10, lmax = -1e10;
  for (int j=0;j<h;j++) {
    // pttable[j][j] = 0.0;
    for (int i=0;i<w;i++) {
      from = Pts_i[i]; to = Pts_j[j];

      // XXX How should we chose the scaling for pp plots!!!
      //     the below gives the same scale as for pt plots.
      //     only problem : the circle is chopped of
      //cur = 2.*thickness/(from-to).norm();
      cur = 2.*(from-to).norm()/thickness;

      if (cur<0) cur = 0.0;
      // if (cur>1. || (from-to).norm()<1e-7) cur = 1.;

      lmin = (cur<lmin?cur:lmin);
      lmax = (cur>lmax?cur:lmax);

      table[i+j*w] = cur;
    }
  }
	*min = lmin;
	*max = lmax;
}

void computeTT(const int w, const int h,
               const TVec* Pts_i, const TVec* Tg_i,
               const TVec* Pts_j, const TVec* Tg_j,
							 const float thickness,
							 float* table,
							 float* min, float *max
               ) {
#ifndef COMPUTE_IN_R4
  float lmin = 1e10, lmax = -1e10;
	float cur;
	TVec P1,T1,P2,T2,E;

  float z,n,tt2;
  for (int j=0;j<h;j++) {
    for (int i=0;i<w;i++) {

      P1 = Pts_i[i]; T1 = Tg_i[i];
      P2 = Pts_j[j]; T2 = Tg_j[j];
      E = P1-P2;
      E.normalize();
      Matrix3 R;
      for (int k=0;k<3;k++)
        for (int l=0;l<3;l++)
          R[k][l] = 2.*E[k]*E[l] - (k==l?1.:0.);
      z = T1.cross(T2).dot(E); z=z*z;
      n = (((P1-P2).norm()*(P1-P2).norm()) *
           (1.0-(T2.dot(R*T1))*(T2.dot(R*T1))));
      if (T2.dot(R*T1)*T2.dot(R*T1) >= 1.0 )
        tt2 = 0;
      else
        tt2 = z/n;
      cur = sqrt(tt2)*thickness;

      //FIXME clamp tt plot to [0,1]
      // I didn't think about it, but it fixes the plot ...
      if (cur<0.) cur=0.;
      if (cur>1.) cur=1.;

      lmin = (cur<lmin?cur:lmin);
      lmax = (cur>lmax?cur:lmax);

      table[i+j*w] = cur;
    }
  }
#else
  
    float z,n,tt2;
    for (int j=0;j<h;j++) {
      for (int i=0;i<w;i++) {

        TVec P1 = Pts_i[i], T1 = Tg_i[i];
        TVec P2 = Pts_j[j], T2 = Tg_j[j];
        TVec E = P1-P2;
        E.normalize();
        Matrix4 R;
        for (int k=0;k<4;k++)
          for (int l=0;l<4;l++)
            R[k][l] = 2.*E[k]*E[l] - (k==l?1.:0.);
        
        // z = T1.cross(T2).dot(E); z=z*z;

        // The grammien is given by G = M^T*M, where M = [ T1 T2 E ]
        // is the matrix with T1, T2 and E as column vecs.
        // The triple product in R^3 is the volume of a ||-epipede,
        // so is sqrt ( det G ), which gives us what we need in R^4
        float M[4][3];
        for (int nn=0;nn<4;++nn) {
          M[nn][0] = T1[nn];
          M[nn][1] = T2[nn];
          M[nn][2] = E[nn];
        }
        Matrix3 G; G.zero();
        for (int row=0;row<3;++row)
          for (int col=0;col<3;++col)
            for (int nn=0;nn<4;++nn)
              G[col][row] += M[nn][row]*M[nn][col];

/*
        cout << "M = \n";
        for (int kk=0;kk<4;++kk) {
          for (int nn=0;nn<3;++nn)
            cout << M[kk][nn] << " ";
          cout << endl;
        }
        cout << G << endl;
*/
        z = G.det(); z = z*z;

        n = (((P1-P2).norm()*(P1-P2).norm()) *
             (1.0-(T2.dot(R*T1))*(T2.dot(R*T1))));
        if (T2.dot(R*T1)*T2.dot(R*T1) >= 1.0 )
          tt2 = 0;
        else
          tt2 = z/n;
        cur = sqrt(tt2)*thickness;

        // FIXME clamp tt plot to [0,1]
        // I didn't think about it, but it fixes the plot ...
        if (cur<0.) cur=0.;
        if (cur>1.) cur=1.;

        lmin = (cur<lmin?cur:lmin);
        lmax = (cur>lmax?cur:lmax);

        table[i+j*w] = cur;
      }
    }
  }
#endif // R4 or not?
	*min = lmin;
	*max = lmax;
}

// Use the float values computed for the given plot
// to get a coloring (using some gradient)
void convertToColoring(const int w, const int h,
                       const float min, const float max,
											 const float* table,
											 void(*gradient)(RGB*,float,float,float),
                       unsigned char* px) {
	unsigned char* src;
	RGB c;
  for (int j=0;j<h;j++) {
    src = px + j*(3*w);
    for (int i=0;i<w;i++) {
      gradient(&c,table[i+j*w],min,max);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
}


void DoPlot(const char* name, int w, int h,
            const float fromx, const float tox,
						const float fromy, const float toy,
            const PLOT_TYPE &ptype,
						const int CLOSED, const int HEIGHTMAP) {

  Curve<Vector3> *curve = new Curve<TVec>(name);
  unsigned char* px;
	float ptmin, ptmax;

  if (curve==NULL) exit(10);
  if (CLOSED)
    curve->link();
  curve->make_default();
  px = new unsigned char[3*w*h];
  if (px == NULL) {
    cerr << "Memory alloc problem!\n";
    exit(5);
  }

  TVec pt,pt2;

  // Allocate data table
  float *pttable;
  pttable = new float[h*w];
  if (pttable == NULL) {
		cout << "Mem alloc prob!\n";
    exit(5);
  }
  // Here we use the thickness only as a scaling factor, thus
  // perfect accuracy is not that important
  float thickness = curve->thickness_fast();

  // make width and height dims different
  // i.e. Pts_i and Pts_j
  TVec Pts_i[w], Tg_i[w];
	samplePtTg(fromx, tox, w, *curve, Pts_i, Tg_i);
  TVec Pts_j[h], Tg_j[h];
	samplePtTg(fromy, toy, h, *curve, Pts_j, Tg_j);

  if (ptype==PT_PLOT)
    computePT(w,h,Pts_i,Tg_i,Pts_j,Tg_j,thickness,
		          pttable,&ptmin,&ptmax);
  else if (ptype==TT_PLOT)
    computeTT(w,h,Pts_i,Tg_i,Pts_j,Tg_j,thickness,
		          pttable,&ptmin,&ptmax);
  else if (ptype==PP_PLOT)
    computePP(w,h,Pts_i,Pts_j,thickness,
		          pttable,&ptmin,&ptmax);

  if (!HEIGHTMAP)
    convertToColoring(w,h,0,ptmax,pttable,map_color_sine_end,px);
	else
    convertToColoring(w,h,0,ptmax,pttable,height_map,px);

  cout << "Min/Max   : " << ptmin << " / " << ptmax << endl;
  cout << "Thickness : " << thickness << endl;

  sp_png_write_rgb("out.png",px,w,h,0.1,0.1,3*w);

  delete px; delete curve;
}

