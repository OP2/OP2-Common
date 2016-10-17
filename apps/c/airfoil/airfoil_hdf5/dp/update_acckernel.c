//
// auto-generated by op2.py
//

//user function
int direct_update_stride_OP2CONSTANT;
int direct_update_stride_OP2HOST=-1;
//user function
#pragma acc routine
inline void update( const double *qold, double *q, double *res,
                   const double *adt, double *rms) {
  double del, adti, rmsl;

  rmsl = 0.0f;
  adti = 1.0f / (*adt);

  for (int n = 0; n < 4; n++) {
    del = adti * res[n*direct_update_stride_OP2CONSTANT];
    q[n*direct_update_stride_OP2CONSTANT] = qold[n*direct_update_stride_OP2CONSTANT] - del;
    res[n*direct_update_stride_OP2CONSTANT] = 0.0f;
    rmsl += del * del;
  }
  *rms += rmsl;
}

// host stub function
void op_par_loop_update(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4){

  double*arg4h = (double *)arg4.data;
  int nargs = 5;
  op_arg args[5];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(4);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[4].name      = name;
  OP_kernels[4].count    += 1;


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  update");
  }

  op_mpi_halo_exchanges_cuda(set, nargs, args);

  double arg4_l = arg4h[0];

  if (set->size >0) {

    if ((OP_kernels[4].count==1) || (direct_update_stride_OP2HOST != getSetSizeFromOpArg(&arg0))) {
      direct_update_stride_OP2HOST = getSetSizeFromOpArg(&arg0);
      direct_update_stride_OP2CONSTANT = direct_update_stride_OP2HOST;
    }

    //Set up typed device pointers for OpenACC

    double* data0 = (double*)arg0.data_d;
    double* data1 = (double*)arg1.data_d;
    double* data2 = (double*)arg2.data_d;
    double* data3 = (double*)arg3.data_d;
    #pragma acc parallel loop independent deviceptr(data0,data1,data2,data3) reduction(+:arg4_l)
    for ( int n=0; n<set->size; n++ ){
      update(
        &data0[n],
        &data1[n],
        &data2[n],
        &data3[1*n],
        &arg4_l);
    }
  }

  // combine reduction data
  arg4h[0] = arg4_l;
  op_mpi_reduce_double(&arg4,arg4h);
  op_mpi_set_dirtybit_cuda(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[4].time     += wall_t2 - wall_t1;
  OP_kernels[4].transfer += (float)set->size * arg0.size;
  OP_kernels[4].transfer += (float)set->size * arg1.size;
  OP_kernels[4].transfer += (float)set->size * arg2.size * 2.0f;
  OP_kernels[4].transfer += (float)set->size * arg3.size;
}
