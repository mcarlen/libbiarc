#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 128
#define TAG 0

#include "../../include/algo_helpers.h"
#include "../../include/Curve.h"

#ifdef TIMING
#include "../../include/utils/timer.h"
#endif

#define BATCH 2

// FIXME : this is specialized for 3D!
// res contains at the end the serialized version of Candi<> c
// however res[0] is reserved!
//
// if res[0] is <0, then the client node knows, that it can stop listening
void pack_candidate(const Candi<Vector3> &c, const double curr_min, double res[24]) {
	int i,j;
	const ArcInfo<Vector3>* ai[2];
	ai[0] = &(c.a); ai[1] = &(c.b);

	for (j=0;j<2;++j) {
  	for (i=0;i<3;++i) res[11*j + i+1 ] = ai[j]->b0[i];
	  for (i=0;i<3;++i) res[11*j + i+4 ] = ai[j]->b1[i]; 
	  for (i=0;i<3;++i) res[11*j + i+7 ] = ai[j]->b2[i]; 
    res[11*j + 10] = ai[j]->err;
		res[11*j + 11] = ai[j]->ferr;
	}
	res[23] = curr_min;
  res[0] = 1;
}

void unpack_candidate(const double res[24], Candi<Vector3>* c, double *curr_min) {
	*c = Candi<Vector3>(Vector3(res[1],res[2],res[3]),
	                    Vector3(res[4],res[5],res[6]),
											Vector3(res[7],res[8],res[9]),
											res[10],res[11],
                      Vector3(res[12],res[13],res[14]),
	                    Vector3(res[15],res[16],res[17]),
											Vector3(res[18],res[19],res[20]),
             					res[21],res[22]);
	*curr_min = res[23];
}

/*
float para_compute_thickness(Curve<Vector> *c, Vector *from = NULL, Vector *to = NULL) {

  float min_diam = check_local_curvature(c);
  vector<Candi<Vector> > tmp, candidates;

  // Initial double critical test
  initial_dbl_crit_filter(c, tmp, min_diam);
  distance_filter(tmp,candidates,min_diam);

  float global_min = min_diam, curr_min;
  Vector lFrom, lTo;
  for (unsigned int i=0;i<candidates.size();++i) {
    curr_min = mindist_between_arcs(candidates[i],global_min,&lFrom,&lTo);
    if (curr_min<global_min) {
      global_min = curr_min;
      if (from!=NULL) *from = lFrom;
      if (to!=NULL)   *to = lTo;
    }
  }
  return global_min;

}
*/
/*
const int SIZE = 10;
double Arr[SIZE] = { 0., 1., 2., 3., 4., 5., 6., 7., 8., 9. };
static int arr_idx = 0;
*/
bool done = false;

int main(int argc, char *argv[]) {

/*
  Candi<Vector3> cc(Vector3(0,0,0),Vector3(1,.5,0),Vector3(2,0,0),
	                  Vector3(0,-2,0),Vector3(1,-2.5,0),Vector3(2,-2,0));

  double serial[23];
	pack_candidate(cc, serial);

	for (int i=0;i<23;++i) printf("%f ",serial[i]);
	printf("\n");
*/

  if (argc!=2) {
    printf("Usage : %s pkf\n", argv[0]);
		exit(0);
	}

//	char idstr[32];
//	char buff[BUFSIZE];
	int numprocs;
  int rank;
	MPI_Status stat; 

  int next_candidate_id = 0;
  int num_received     = 0;
	double serial[BATCH][24];

	MPI_Init(&argc,&argv); /* all MPI programs start with MPI_Init; all 'N' processes exist thereafter */

  MPI_Datatype newtype;
  MPI_Type_contiguous(24, MPI_DOUBLE, &newtype);
	MPI_Type_commit(&newtype);

	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /* find out how big the SPMD world is */
	MPI_Comm_rank(MPI_COMM_WORLD,&rank); /* and this processes' rank is */

	if(rank == 0)	{

//    printf("No of procs : %d\n", numprocs);

		// Master computes the first candidates for the computation
		Curve<Vector3> curve(argv[1]);

		curve.link();
		curve.make_default();
//	  curve.normalize();
//		curve.make_default();

#ifdef TIMING
  float mytime = start_time();
#endif

	  double dval;
		double min_diam = check_local_curvature(&curve);
		vector<Candi<Vector3> > tmp, candidates;

		// Initial double critical test
		initial_dbl_crit_filter(&curve, tmp, min_diam);
		distance_filter(tmp,candidates,min_diam);
    double gDmin = min_diam;

//		printf("ROOT : We have %d candidates\n", candidates.size());

		// printf("%d: We have %d processors\n", rank, numprocs);

	  for(int i=1;i<numprocs;i++)	{
		  if (next_candidate_id>=candidates.size()) break;

			for (int j=0;j<BATCH;++j) {
			// XXX lol, if everything is sent, send the same pair again ;)
			  if (next_candidate_id>=candidates.size()) next_candidate_id--;
        pack_candidate(candidates[next_candidate_id],gDmin,serial[j]);
	  		next_candidate_id++;
			}

	  	MPI_Send(serial, BATCH, newtype, i, TAG, MPI_COMM_WORLD);
		}


		while (!done) {
			for(int i=1;i<numprocs;i++)	{
				if (num_received<candidates.size()) {
  				MPI_Recv(&dval, 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, &stat);
	  			num_received+=BATCH;

  				if (dval<gDmin)
	  				gDmin = dval;

          if (next_candidate_id>=candidates.size())
  		  		if (num_received>=candidates.size()) { done = true; break; }
          if (next_candidate_id<candidates.size()) {

			for (int j=0;j<BATCH;++j) {
			// XXX lol, if everything is sent, send the same pair again ;)
			  if (next_candidate_id>=candidates.size()) next_candidate_id--;
        pack_candidate(candidates[next_candidate_id],gDmin,serial[j]);
	  		next_candidate_id++;
			}

	      	  MPI_Send(serial, BATCH, newtype, i, TAG, MPI_COMM_WORLD);
				  }

//			printf("ROOT : Node %d sends %f\n", i, dval);
			}
			else done = true;
		}
  }
    printf("ROOT : all done send STOP to children\n");

		// Tell the children, that we're done
		serial[0][0] = -1;
		for(int i=1;i<numprocs;i++)
			MPI_Send(serial, BATCH, newtype, i, TAG, MPI_COMM_WORLD);

    printf("Rope : %f\n", curve.length()/gDmin);
#ifdef TIMING
		cerr << stop_time(mytime) << endl;
#endif

	}
	else {
		while (true) {
			double curr_best, curr_min;
			/* receive from rank 0: */
			MPI_Recv(serial, BATCH, newtype, 0, TAG, MPI_COMM_WORLD, &stat);
			if (serial[0][0]<0) {
//				printf("Node %d : DONE!\n", rank);
				break; // we're done
			}

      Candi<Vector3> candi[BATCH];
			for (int j=0;j<BATCH;++j)
        unpack_candidate(serial[j], &(candi[j]), &curr_best);

//      printf("Node %d : received currently best : %f\n", rank, curr_best);

      for (int j=0;j<BATCH;++j) {
        curr_min = mindist_between_arcs(candi[j], curr_best, (Vector3*)NULL, (Vector3*)NULL);
			  if (curr_min<curr_best) curr_best = curr_min;
			}
//			printf("Node %d : computed val %f\n", rank, curr_min);

			/* send to rank 0: */
//			cout << "Node " << rank << " sending back answer " <<curr_best <<endl;
			MPI_Send(&curr_best, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize(); /* MPI Programs end with MPI Finalize; this is a weak synchronization point */

	return 0;
}

