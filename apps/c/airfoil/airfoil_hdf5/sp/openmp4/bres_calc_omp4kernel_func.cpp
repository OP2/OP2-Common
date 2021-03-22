//
// auto-generated by op2.py
//

void bres_calc_omp4_kernel(int *map0, int map0size, int *map2, int map2size,
                           int *data5, int dat5size, float *data0, int dat0size,
                           float *data2, int dat2size, float *data3,
                           int dat3size, float *data4, int dat4size,
                           int *col_reord, int set_size1, int start, int end,
                           int num_teams, int nthread,
                           int opDat0_bres_calc_stride_OP2CONSTANT,
                           int opDat2_bres_calc_stride_OP2CONSTANT) {

#pragma omp target teams num_teams(num_teams) thread_limit(nthread)            \
    map(to : data5[0 : dat5size])                                              \
        map(to : gm1_ompkernel, eps_ompkernel, qinf_ompkernel[ : 4]) map(      \
            to : col_reord[0 : set_size1],                                     \
                           map0[0 : map0size],                                 \
                                map2[0 : map2size],                            \
                                     data0[0 : dat0size],                      \
                                           data2[0 : dat2size],                \
                                                 data3[0 : dat3size],          \
                                                       data4[0 : dat4size])
#pragma omp distribute parallel for schedule(static, 1)
  for ( int e=start; e<end; e++ ){
    int n_op = col_reord[e];
    int map0idx;
    int map1idx;
    int map2idx;
    map0idx = map0[n_op + set_size1 * 0];
    map1idx = map0[n_op + set_size1 * 1];
    map2idx = map2[n_op + set_size1 * 0];

    //variable mapping
    const float *x1 = &data0[map0idx];
    const float *x2 = &data0[map1idx];
    const float *q1 = &data2[map2idx];
    const float *adt1 = &data3[1 * map2idx];
    float *res1 = &data4[map2idx];
    const int *bound = &data5[1*n_op];

    //inline function
    
    float dx, dy, mu, ri, p1, vol1, p2, vol2, f;

    dx = x1[(0) * opDat0_bres_calc_stride_OP2CONSTANT] -
         x2[(0) * opDat0_bres_calc_stride_OP2CONSTANT];
    dy = x1[(1) * opDat0_bres_calc_stride_OP2CONSTANT] -
         x2[(1) * opDat0_bres_calc_stride_OP2CONSTANT];

    ri = 1.0f / q1[(0) * opDat2_bres_calc_stride_OP2CONSTANT];
    p1 = gm1_ompkernel *
         (q1[(3) * opDat2_bres_calc_stride_OP2CONSTANT] -
          0.5f * ri * (q1[(1) * opDat2_bres_calc_stride_OP2CONSTANT] *
                           q1[(1) * opDat2_bres_calc_stride_OP2CONSTANT] +
                       q1[(2) * opDat2_bres_calc_stride_OP2CONSTANT] *
                           q1[(2) * opDat2_bres_calc_stride_OP2CONSTANT]));

    vol1 = ri * (q1[(1) * opDat2_bres_calc_stride_OP2CONSTANT] * dy -
                 q1[(2) * opDat2_bres_calc_stride_OP2CONSTANT] * dx);

    ri = 1.0f / qinf_ompkernel[0];
    p2 = gm1_ompkernel * (qinf_ompkernel[3] - 0.5f * ri * (qinf_ompkernel[1] * qinf_ompkernel[1] + qinf_ompkernel[2] * qinf_ompkernel[2]));
    vol2 = ri * (qinf_ompkernel[1] * dy - qinf_ompkernel[2] * dx);

    mu = (*adt1) * eps_ompkernel;

    f = 0.5f * (vol1 * q1[(0) * opDat2_bres_calc_stride_OP2CONSTANT] +
                vol2 * qinf_ompkernel[0]) +
        mu *
            (q1[(0) * opDat2_bres_calc_stride_OP2CONSTANT] - qinf_ompkernel[0]);
    res1[(0) * opDat2_bres_calc_stride_OP2CONSTANT] += *bound == 1 ? 0.0f : f;
    f = 0.5f * (vol1 * q1[(1) * opDat2_bres_calc_stride_OP2CONSTANT] + p1 * dy +
                vol2 * qinf_ompkernel[1] + p2 * dy) +
        mu *
            (q1[(1) * opDat2_bres_calc_stride_OP2CONSTANT] - qinf_ompkernel[1]);
    res1[(1) * opDat2_bres_calc_stride_OP2CONSTANT] +=
        *bound == 1 ? p1 * dy : f;
    f = 0.5f * (vol1 * q1[(2) * opDat2_bres_calc_stride_OP2CONSTANT] - p1 * dx +
                vol2 * qinf_ompkernel[2] - p2 * dx) +
        mu *
            (q1[(2) * opDat2_bres_calc_stride_OP2CONSTANT] - qinf_ompkernel[2]);
    res1[(2) * opDat2_bres_calc_stride_OP2CONSTANT] +=
        *bound == 1 ? -p1 * dx : f;
    f = 0.5f * (vol1 * (q1[(3) * opDat2_bres_calc_stride_OP2CONSTANT] + p1) +
                vol2 * (qinf_ompkernel[3] + p2)) +
        mu *
            (q1[(3) * opDat2_bres_calc_stride_OP2CONSTANT] - qinf_ompkernel[3]);
    res1[(3) * opDat2_bres_calc_stride_OP2CONSTANT] += *bound == 1 ? 0.0f : f;
    //end inline func
  }
}
