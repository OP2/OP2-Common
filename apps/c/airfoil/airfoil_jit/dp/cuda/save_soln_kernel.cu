//
// auto-generated by op2.py
//

//user function
__device__ void save_soln_gpu( const double *q, double *qold)
{
  for (int n = 0; n < 4; n++)
    qold[n] = q[n];

}
