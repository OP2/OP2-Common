//
// auto-generated by op2.py
//

//user function
#include "../init_cg.h"

// host stub function
void op_par_loop_init_cg(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4){

  int nargs = 5;
  op_arg args[5];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(2);
  op_timers_core(&cpu_t1, &wall_t1);


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  init_cg");
  }

  int set_size = op_mpi_halo_exchanges(set, nargs, args);
  int reduct_bytes = 0;

  reduct_bytes += ROUND_UP(arg1.dim*sizeof(double)*set_size);
  reallocReductArrays(reduct_bytes);

  reduct_bytes=0;
  double* red1 = (double*)(OP_reduct_h+reduct_bytes);
  reduct_bytes+=arg1.dim*sizeof(double)*set_size;
  for ( int i=0; i<arg1.dim*set_size; i++ ){
    red1[i]=0;
  }

  if (set->size >0) {

    for ( int n=0; n<set_size; n++ ){
      init_cg(
        &((double*)arg0.data)[1*n],
        &red1[1*n],
        &((double*)arg2.data)[1*n],
        &((double*)arg3.data)[1*n],
        &((double*)arg4.data)[1*n]);
    }
  }

  reprLocalSum(&arg1,set_size,red1);
  // combine reduction data
  op_mpi_repr_inc_reduce_double(&arg1,(double*)arg1.data);
  op_mpi_set_dirtybit(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[2].name      = name;
  OP_kernels[2].count    += 1;
  OP_kernels[2].time     += wall_t2 - wall_t1;
  OP_kernels[2].transfer += (float)set->size * arg0.size;
  OP_kernels[2].transfer += (float)set->size * arg2.size * 2.0f;
  OP_kernels[2].transfer += (float)set->size * arg3.size * 2.0f;
  OP_kernels[2].transfer += (float)set->size * arg4.size * 2.0f;
}
