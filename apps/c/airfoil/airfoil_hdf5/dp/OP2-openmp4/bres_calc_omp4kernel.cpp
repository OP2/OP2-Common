//
// auto-generated by op2.py
//

#pragma omp declare target
inline void bres_calc( const double *x1, const double *x2, const double *q1,
                      const double *adt1, double *res1, const int *bound) {
  double dx, dy, mu, ri, p1, vol1, p2, vol2, f;

  dx = x1[0] - x2[0];
  dy = x1[1] - x2[1];

  ri = 1.0f / q1[0];
  p1 = gm1_ompkernel * (q1[3] - 0.5f * ri * (q1[1] * q1[1] + q1[2] * q1[2]));

  if (*bound == 1) {
    res1[1] += +p1 * dy;
    res1[2] += -p1 * dx;
  } else {
    vol1 = ri * (q1[1] * dy - q1[2] * dx);

    ri = 1.0f / qinf_ompkernel[0];
    p2 = gm1_ompkernel * (qinf_ompkernel[3] - 0.5f * ri * (qinf_ompkernel[1] * qinf_ompkernel[1] + qinf_ompkernel[2] * qinf_ompkernel[2]));
    vol2 = ri * (qinf_ompkernel[1] * dy - qinf_ompkernel[2] * dx);

    mu = (*adt1) * eps_ompkernel;

    f = 0.5f * (vol1 * q1[0] + vol2 * qinf_ompkernel[0]) + mu * (q1[0] - qinf_ompkernel[0]);
    res1[0] += f;
    f = 0.5f * (vol1 * q1[1] + p1 * dy + vol2 * qinf_ompkernel[1] + p2 * dy) +
        mu * (q1[1] - qinf_ompkernel[1]);
    res1[1] += f;
    f = 0.5f * (vol1 * q1[2] - p1 * dx + vol2 * qinf_ompkernel[2] - p2 * dx) +
        mu * (q1[2] - qinf_ompkernel[2]);
    res1[2] += f;
    f = 0.5f * (vol1 * (q1[3] + p1) + vol2 * (qinf_ompkernel[3] + p2)) +
        mu * (q1[3] - qinf_ompkernel[3]);
    res1[3] += f;
  }
}
#pragma omp end declare target

void bres_calc_omp4_kernel(
  int *map0,
  int *map2,
  int *data5,
  double *data0,
  double *data2,
  double *data3,
  double *data4,
  int *col_reord,
  int set_size1,
  int start,
  int end){
  #pragma omp target teams map(to:col_reord,map0,map2,data5,data0,data2,data3,data4)
  #pragma omp distribute parallel for schedule(static,1)
  for ( int e=start; e<end; e++ ){
    int n = col_reord[e];
    int map0idx = map0[n + set_size1 * 0];
    int map1idx = map0[n + set_size1 * 1];
    int map2idx = map2[n + set_size1 * 0];

    /*
    bres_calc(
      &data0[2 * map0idx],
      &data0[2 * map1idx],
      &data2[4 * map2idx],
      &data3[1 * map2idx],
      &data4[4 * map2idx],
      &data5[1 * n]);
    */
    //variable mapping
    /*
    const double *x1 = &data0[2 * map0idx];
    const double *x2 = &data0[2 * map1idx];
    const double *q1 = &data2[4 * map2idx];
    const double *adt1 = &data3[1 * map2idx];
    double *res1 = &data4[4 * map2idx];
    const int *bound = &data5[1*n];
    */
    //inline function
      
    double dx, dy, mu, ri, p1, vol1, p2, vol2, f;
  
    dx = data0[2 * map0idx + 0] - data0[2 * map1idx + 0];
    dy = data0[2 * map0idx + 1] - data0[2 * map1idx + 1];
  
    ri = 1.0f / data2[4 * map2idx + 0];
    p1 = gm1_ompkernel * (data2[4 * map2idx + 3] - 0.5f * ri * (data2[4 * map2idx + 1] * data2[4 * map2idx + 1] + data2[4 * map2idx + 2] * data2[4 * map2idx + 2]));
  
    if (data5[1*n + 0] == 1) {
      data4[4 * map2idx + 1] += +p1 * dy;
      data4[4 * map2idx + 2] += -p1 * dx;
    } else {
      vol1 = ri * (data2[4 * map2idx + 1] * dy - data2[4 * map2idx + 2] * dx);
  
      ri = 1.0f / qinf_ompkernel[0];
      p2 = gm1_ompkernel * (qinf_ompkernel[3] - 0.5f * ri * (qinf_ompkernel[1] * qinf_ompkernel[1] + qinf_ompkernel[2] * qinf_ompkernel[2]));
      vol2 = ri * (qinf_ompkernel[1] * dy - qinf_ompkernel[2] * dx);
  
      mu = (data3[1 * map2idx + 0]) * eps_ompkernel;
  
      f = 0.5f * (vol1 * data2[4 * map2idx + 0] + vol2 * qinf_ompkernel[0]) + mu * (data2[4 * map2idx + 0] - qinf_ompkernel[0]);
      data4[4 * map2idx + 0] += f;
      f = 0.5f * (vol1 * data2[4 * map2idx + 1] + p1 * dy + vol2 * qinf_ompkernel[1] + p2 * dy) +
          mu * (data2[4 * map2idx + 1] - qinf_ompkernel[1]);
      data4[4 * map2idx + 1] += f;
      f = 0.5f * (vol1 * data2[4 * map2idx + 2] - p1 * dx + vol2 * qinf_ompkernel[2] - p2 * dx) +
          mu * (data2[4 * map2idx + 2] - qinf_ompkernel[2]);
      data4[4 * map2idx + 2] += f;
      f = 0.5f * (vol1 * (data2[4 * map2idx + 3] + p1) + vol2 * (qinf_ompkernel[3] + p2)) +
          mu * (data2[4 * map2idx + 3] - qinf_ompkernel[3]);
      data4[4 * map2idx + 3] += f;
    }
    //end inline func
  }

}