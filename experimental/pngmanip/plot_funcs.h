#ifndef __PLOT_FUNCS_H__
#define __PLOT_FUNCS_H__

#include "png.h"
#include "Curve.h"
#include "colors.h"
#include "pngwrite.h"

#ifdef COMPUTE_IN_R4
#include "Vector4.h"
#include "Matrix4.h"
#define TVec Vector4
#else
#define TVec Vector3
#endif

enum PLOT_TYPE { PT_PLOT = 1, PP_PLOT, TT_PLOT };

void samplePtTg(const float from, const float to,
                const int N, const Curve<TVec> &c,
						    TVec* pts, TVec* tgs);

void computePT(const int w, const int h,
               const TVec* Pts_i, const TVec* Tg_i,
               const TVec* Pts_j, const TVec* Tg_j,
							 const float thickness,
							 float* table,
							 float* min, float *max
               );

void computePP(const int w, const int h,
               const TVec* Pts_i, const TVec* Pts_j, 
							 const float thickness,
							 float* table,
							 float* min, float *max
               );

void computeTT(const int w, const int h,
               const TVec* Pts_i, const TVec* Tg_i,
               const TVec* Pts_j, const TVec* Tg_j,
							 const float thickness,
							 float* table,
							 float* min, float *max
               );

void convertToColoring(const int w, const int h,
                       const float min, const float max,
											 const float* table,
											 void(*gradient)(RGB*,float,float,float),
                       unsigned char* px);

void DoPlot(const char* name, int w, int h,
            const float fromx, const float tox,
						const float fromy, const float toy,
						const PLOT_TYPE &ptype,
            const int CLOSED, const int HEIGHTMAP);

#endif // __PLOT_FUNCS_H__
