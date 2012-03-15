//
// auto-generated by op2.m on 14-Mar-2012 14:11:09
//

// user function

#include "update.h"


// x86 kernel function

void op_x86_update(
  float *arg0,
  float *arg1,
  float *arg2,
  float *arg3,
  float *arg4,
  int   start,
  int   finish ) {


  // process set elements

  for (int n=start; n<finish; n++) {

    // user-supplied kernel call

    update( arg0+n*1,
            arg1+n*1,
            arg2+n*1,
            arg3,
            arg4 );
  }
}


// host stub function

void op_par_loop_update(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4 ){

  float *arg3h = (float *)arg3.data;
  float *arg4h = (float *)arg4.data;

  int    nargs   = 5;
  op_arg args[5] = {arg0,arg1,arg2,arg3,arg4};

  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  update \n");
  }

  op_mpi_halo_exchanges(set, nargs, args);

  // initialise timers

  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timers_core(&cpu_t1, &wall_t1);

  // set number of threads

#ifdef _OPENMP
  int nthreads = omp_get_max_threads( );
#else
  int nthreads = 1;
#endif

  // allocate and initialise arrays for global reduction

  float arg3_l[1+64*64];
  for (int thr=0; thr<nthreads; thr++)
    for (int d=0; d<1; d++) arg3_l[d+thr*64]=ZERO_float;

  float arg4_l[1+64*64];
  for (int thr=0; thr<nthreads; thr++)
    for (int d=0; d<1; d++) arg4_l[d+thr*64]=arg4h[d];

  if (set->size >0) {


  // execute plan

#pragma omp parallel for
  for (int thr=0; thr<nthreads; thr++) {
    int start  = (set->size* thr   )/nthreads;
    int finish = (set->size*(thr+1))/nthreads;
    op_x86_update( (float *) arg0.data,
                   (float *) arg1.data,
                   (float *) arg2.data,
                   arg3_l + thr*64,
                   arg4_l + thr*64,
                   start, finish );
  }

  }


  // combine reduction data

  for (int thr=0; thr<nthreads; thr++)
    for(int d=0; d<1; d++) arg3h[d] += arg3_l[d+thr*64];

  op_mpi_reduce(&arg3,arg3h);

  for (int thr=0; thr<nthreads; thr++)
    for(int d=0; d<1; d++) arg4h[d]  = MAX(arg4h[d],arg4_l[d+thr*64]);

  op_mpi_reduce(&arg4,arg4h);

  // update kernel record

  op_mpi_barrier();
  op_timers(&cpu_t2, &wall_t2);
  op_timing_realloc(1);
  OP_kernels[1].name      = name;
  OP_kernels[1].count    += 1;
  OP_kernels[1].time     += wall_t2 - wall_t1;
  OP_kernels[1].transfer += (float)set->size * arg0.size;
  OP_kernels[1].transfer += (float)set->size * arg1.size * 2.0f;
  OP_kernels[1].transfer += (float)set->size * arg2.size * 2.0f;
}

