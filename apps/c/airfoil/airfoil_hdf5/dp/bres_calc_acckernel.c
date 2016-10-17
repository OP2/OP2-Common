//
// auto-generated by op2.py
//

//user function
int opDat0_bres_calc_stride_OP2CONSTANT;
int opDat0_bres_calc_stride_OP2HOST=-1;
int opDat2_bres_calc_stride_OP2CONSTANT;
int opDat2_bres_calc_stride_OP2HOST=-1;
//user function
#pragma acc routine
inline void bres_calc( const double *x1, const double *x2, const double *q1,
                      const double *adt1, double *res1, const int *bound) {
  double dx, dy, mu, ri, p1, vol1, p2, vol2, f;

  dx = x1[0*opDat0_bres_calc_stride_OP2CONSTANT] - x2[0*opDat0_bres_calc_stride_OP2CONSTANT];
  dy = x1[1*opDat0_bres_calc_stride_OP2CONSTANT] - x2[1*opDat0_bres_calc_stride_OP2CONSTANT];

  ri = 1.0f / q1[0*opDat2_bres_calc_stride_OP2CONSTANT];
  p1 = gm1 * (q1[3*opDat2_bres_calc_stride_OP2CONSTANT] - 0.5f * ri * (q1[1*opDat2_bres_calc_stride_OP2CONSTANT] * q1[1*opDat2_bres_calc_stride_OP2CONSTANT] + q1[2*opDat2_bres_calc_stride_OP2CONSTANT] * q1[2*opDat2_bres_calc_stride_OP2CONSTANT]));

  if (*bound == 1) {
    res1[1*opDat2_bres_calc_stride_OP2CONSTANT] += +p1 * dy;
    res1[2*opDat2_bres_calc_stride_OP2CONSTANT] += -p1 * dx;
  } else {
    vol1 = ri * (q1[1*opDat2_bres_calc_stride_OP2CONSTANT] * dy - q1[2*opDat2_bres_calc_stride_OP2CONSTANT] * dx);

    ri = 1.0f / qinf[0];
    p2 = gm1 * (qinf[3] - 0.5f * ri * (qinf[1] * qinf[1] + qinf[2] * qinf[2]));
    vol2 = ri * (qinf[1] * dy - qinf[2] * dx);

    mu = (*adt1) * eps;

    f = 0.5f * (vol1 * q1[0*opDat2_bres_calc_stride_OP2CONSTANT] + vol2 * qinf[0]) + mu * (q1[0*opDat2_bres_calc_stride_OP2CONSTANT] - qinf[0]);
    res1[0*opDat2_bres_calc_stride_OP2CONSTANT] += f;
    f = 0.5f * (vol1 * q1[1*opDat2_bres_calc_stride_OP2CONSTANT] + p1 * dy + vol2 * qinf[1] + p2 * dy) +
        mu * (q1[1*opDat2_bres_calc_stride_OP2CONSTANT] - qinf[1]);
    res1[1*opDat2_bres_calc_stride_OP2CONSTANT] += f;
    f = 0.5f * (vol1 * q1[2*opDat2_bres_calc_stride_OP2CONSTANT] - p1 * dx + vol2 * qinf[2] - p2 * dx) +
        mu * (q1[2*opDat2_bres_calc_stride_OP2CONSTANT] - qinf[2]);
    res1[2*opDat2_bres_calc_stride_OP2CONSTANT] += f;
    f = 0.5f * (vol1 * (q1[3*opDat2_bres_calc_stride_OP2CONSTANT] + p1) + vol2 * (qinf[3] + p2)) +
        mu * (q1[3*opDat2_bres_calc_stride_OP2CONSTANT] - qinf[3]);
    res1[3*opDat2_bres_calc_stride_OP2CONSTANT] += f;
  }
}

// host stub function
void op_par_loop_bres_calc(char const *name, op_set set,
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
  op_timing_realloc(3);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[3].name      = name;
  OP_kernels[3].count    += 1;

  int  ninds   = 4;
  int  inds[6] = {0,0,1,2,3,-1};

  if (OP_diags>2) {
    printf(" kernel routine with indirection: bres_calc\n");
  }

  // get plan
  #ifdef OP_PART_SIZE_3
    int part_size = OP_PART_SIZE_3;
  #else
    int part_size = OP_part_size;
  #endif

  int set_size = op_mpi_halo_exchanges_cuda(set, nargs, args);


  int ncolors = 0;

  if (set->size >0) {

    if ((OP_kernels[3].count==1) || (opDat0_bres_calc_stride_OP2HOST != getSetSizeFromOpArg(&arg0))) {
      opDat0_bres_calc_stride_OP2HOST = getSetSizeFromOpArg(&arg0);
      opDat0_bres_calc_stride_OP2CONSTANT = opDat0_bres_calc_stride_OP2HOST;
    }
    if ((OP_kernels[3].count==1) || (opDat2_bres_calc_stride_OP2HOST != getSetSizeFromOpArg(&arg2))) {
      opDat2_bres_calc_stride_OP2HOST = getSetSizeFromOpArg(&arg2);
      opDat2_bres_calc_stride_OP2CONSTANT = opDat2_bres_calc_stride_OP2HOST;
    }

    //Set up typed device pointers for OpenACC
    int *map0 = arg0.map_data_d;
    int *map2 = arg2.map_data_d;

    int* data5 = (int*)arg5.data_d;
    double *data0 = (double *)arg0.data_d;
    double *data2 = (double *)arg2.data_d;
    double *data3 = (double *)arg3.data_d;
    double *data4 = (double *)arg4.data_d;

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

      #pragma acc parallel loop independent deviceptr(col_reord,map0,map2,data5,data0,data2,data3,data4)
      for ( int e=start; e<end; e++ ){
        int n = col_reord[e];
        int map0idx = map0[n + set_size1 * 0];
        int map1idx = map0[n + set_size1 * 1];
        int map2idx = map2[n + set_size1 * 0];

        bres_calc(
          &data0[map0idx],
          &data0[map1idx],
          &data2[map2idx],
          &data3[1 * map2idx],
          &data4[map2idx],
          &data5[1 * n]);
      }

    }
    OP_kernels[3].transfer  += Plan->transfer;
    OP_kernels[3].transfer2 += Plan->transfer2;
  }

  if (set_size == 0 || set_size == set->core_size || ncolors == 1) {
    op_mpi_wait_all_cuda(nargs, args);
  }
  // combine reduction data
  op_mpi_set_dirtybit_cuda(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[3].time     += wall_t2 - wall_t1;
}
