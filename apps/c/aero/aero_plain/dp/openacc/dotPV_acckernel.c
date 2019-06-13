//
// auto-generated by op2.py
//

//user function
//user function
//#pragma acc routine
inline void dotPV_openacc(const double *p, const double *v, double *c) {
  *c += (*p) * (*v);
}

// host stub function
void op_par_loop_dotPV(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2){

  double*arg2h = (double *)arg2.data;
  int nargs = 3;
  op_arg args[3];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(4);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[4].name      = name;
  OP_kernels[4].count    += 1;


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  dotPV");
  }

  op_mpi_halo_exchanges_cuda(set, nargs, args);

  double arg2_l = arg2h[0];

  if (set->size >0) {


    //Set up typed device pointers for OpenACC

    double* data0 = (double*)arg0.data_d;
    double* data1 = (double*)arg1.data_d;
    #pragma acc parallel loop independent deviceptr(data0,data1) reduction(+:arg2_l)
    for ( int n=0; n<set->size; n++ ){
      dotPV_openacc(&data0[1 * n], &data1[1 * n], &arg2_l);
    }
  }

  // combine reduction data
  arg2h[0] = arg2_l;
  op_mpi_reduce_double(&arg2,arg2h);
  op_mpi_set_dirtybit_cuda(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[4].time     += wall_t2 - wall_t1;
  OP_kernels[4].transfer += (float)set->size * arg0.size;
  OP_kernels[4].transfer += (float)set->size * arg1.size;
}
