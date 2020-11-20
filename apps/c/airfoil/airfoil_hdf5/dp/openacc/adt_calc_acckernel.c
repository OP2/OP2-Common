//
// auto-generated by op2.py
//

//user function
int opDat0_adt_calc_stride_OP2CONSTANT;
int opDat0_adt_calc_stride_OP2HOST = -1;
int direct_adt_calc_stride_OP2CONSTANT;
int direct_adt_calc_stride_OP2HOST = -1;
//user function
//#pragma acc routine
inline void adt_calc_openacc( const double *x1, const double *x2, const double *x3,
                     const double *x4, const double *q, double *adt) {
  double dx, dy, ri, u, v, c;

  ri = 1.0f / q[(0) * direct_adt_calc_stride_OP2CONSTANT];
  u = ri * q[(1) * direct_adt_calc_stride_OP2CONSTANT];
  v = ri * q[(2) * direct_adt_calc_stride_OP2CONSTANT];
  c = sqrt(gam * gm1 * (ri * q[(3) * direct_adt_calc_stride_OP2CONSTANT] -
                        0.5f * (u * u + v * v)));

  dx = x2[(0) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x1[(0) * opDat0_adt_calc_stride_OP2CONSTANT];
  dy = x2[(1) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x1[(1) * opDat0_adt_calc_stride_OP2CONSTANT];
  *adt = fabs(u * dy - v * dx) + c * sqrt(dx * dx + dy * dy);

  dx = x3[(0) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x2[(0) * opDat0_adt_calc_stride_OP2CONSTANT];
  dy = x3[(1) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x2[(1) * opDat0_adt_calc_stride_OP2CONSTANT];
  *adt += fabs(u * dy - v * dx) + c * sqrt(dx * dx + dy * dy);

  dx = x4[(0) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x3[(0) * opDat0_adt_calc_stride_OP2CONSTANT];
  dy = x4[(1) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x3[(1) * opDat0_adt_calc_stride_OP2CONSTANT];
  *adt += fabs(u * dy - v * dx) + c * sqrt(dx * dx + dy * dy);

  dx = x1[(0) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x4[(0) * opDat0_adt_calc_stride_OP2CONSTANT];
  dy = x1[(1) * opDat0_adt_calc_stride_OP2CONSTANT] -
       x4[(1) * opDat0_adt_calc_stride_OP2CONSTANT];
  *adt += fabs(u * dy - v * dx) + c * sqrt(dx * dx + dy * dy);

  *adt = (*adt) * (1.0f / cfl);
}

// host stub function
void op_par_loop_adt_calc(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4,
  op_arg arg5){

  int nargs = 6;
  op_arg args[6];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;
  args[5] = arg5;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(1);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[1].name      = name;
  OP_kernels[1].count    += 1;

  int  ninds   = 1;
  int  inds[6] = {0,0,0,0,-1,-1};

  if (OP_diags>2) {
    printf(" kernel routine with indirection: adt_calc\n");
  }

  // get plan
  #ifdef OP_PART_SIZE_1
    int part_size = OP_PART_SIZE_1;
  #else
    int part_size = OP_part_size;
  #endif

  int set_size = op_mpi_halo_exchanges_cuda(set, nargs, args);


  int ncolors = 0;

  if (set->size >0) {

    if ((OP_kernels[1].count == 1) ||
        (opDat0_adt_calc_stride_OP2HOST != getSetSizeFromOpArg(&arg0))) {
      opDat0_adt_calc_stride_OP2HOST = getSetSizeFromOpArg(&arg0);
      opDat0_adt_calc_stride_OP2CONSTANT = opDat0_adt_calc_stride_OP2HOST;
    }
    if ((OP_kernels[1].count == 1) ||
        (direct_adt_calc_stride_OP2HOST != getSetSizeFromOpArg(&arg4))) {
      direct_adt_calc_stride_OP2HOST = getSetSizeFromOpArg(&arg4);
      direct_adt_calc_stride_OP2CONSTANT = direct_adt_calc_stride_OP2HOST;
    }

    //Set up typed device pointers for OpenACC
    int *map0 = arg0.map_data_d;

    double* data4 = (double*)arg4.data_d;
    double* data5 = (double*)arg5.data_d;
    double *data0 = (double *)arg0.data_d;

    op_plan *Plan = op_plan_get_stage(name,set,part_size,nargs,args,ninds,inds,OP_COLOR2);
    ncolors = Plan->ncolors;
    int *col_reord = Plan->col_reord;
    int set_size1 = set->size + set->exec_size;

    // execute plan
    for ( int col=0; col<Plan->ncolors; col++ ){
      if (col==1) {
        op_mpi_wait_all_cuda(nargs, args);
      }
      int start = Plan->col_offsets[0][col];
      int end = Plan->col_offsets[0][col+1];

      #pragma acc parallel loop independent deviceptr(col_reord,map0,data4,data5,data0)
      for ( int e=start; e<end; e++ ){
        int n = col_reord[e];
        int map0idx = map0[n + set_size1 * 0];
        int map1idx = map0[n + set_size1 * 1];
        int map2idx = map0[n + set_size1 * 2];
        int map3idx = map0[n + set_size1 * 3];

        adt_calc_openacc(&data0[map0idx], &data0[map1idx], &data0[map2idx],
                         &data0[map3idx], &data4[n], &data5[1 * n]);
      }

    }
    OP_kernels[1].transfer  += Plan->transfer;
    OP_kernels[1].transfer2 += Plan->transfer2;
  }

  if (set_size == 0 || set_size == set->core_size || ncolors == 1) {
    op_mpi_wait_all_cuda(nargs, args);
  }
  // combine reduction data
  op_mpi_set_dirtybit_cuda(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[1].time     += wall_t2 - wall_t1;
}
