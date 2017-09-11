//
// auto-generated by op2.py
//

void save_soln_omp4_kernel(
  double *data0,
  int dat0size,
  double *data1,
  int dat1size,
  int count,
  int num_teams,
  int nthread,
  int direct_save_soln_stride_OP2CONSTANT){

  #pragma omp target teams distribute parallel for schedule(static,1)\
     num_teams(num_teams) thread_limit(nthread) map(to:data0[0:dat0size],data1[0:dat1size])
  for ( int n_op=0; n_op<count; n_op++ ){
    //variable mapping
    const double *q = &data0[n_op];
    double *qold = &data1[n_op];

    //inline function
      
    for (int n = 0; n < 4; n++)
      qold[(n)*direct_save_soln_stride_OP2CONSTANT] = q[(n)*direct_save_soln_stride_OP2CONSTANT];
    //end inline func
  }

}