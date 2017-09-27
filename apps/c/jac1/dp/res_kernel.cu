//
// auto-generated by op2.py
//

//user function
__device__ void res_gpu( const double *A, const double *u, double *du,
                const double *beta) {
  *du += (*beta) * (*A) * (*u);
}

// CUDA kernel function
__global__ void op_cuda_res(
  const double *__restrict ind_arg0,
  double *__restrict ind_arg1,
  const int *__restrict opDat1Map,
  const double *__restrict arg0,
  const double *arg3,
  int   *ind_map,
  short *arg_map,
  int   *ind_arg_sizes,
  int   *ind_arg_offs,
  int    block_offset,
  int   *blkmap,
  int   *offset,
  int   *nelems,
  int   *ncolors,
  int   *colors,
  int   nblocks,
  int   set_size) {
  double arg2_l[1];

  __shared__  int  *ind_arg1_map, ind_arg1_size;
  __shared__  double *ind_arg1_s;

  __shared__ int    nelems2, ncolor;
  __shared__ int    nelem, offset_b;

  extern __shared__ char shared[];

  if (blockIdx.x+blockIdx.y*gridDim.x >= nblocks) {
    return;
  }
  if (threadIdx.x==0) {

    //get sizes and shift pointers and direct-mapped data

    int blockId = blkmap[blockIdx.x + blockIdx.y*gridDim.x  + block_offset];

    nelem    = nelems[blockId];
    offset_b = offset[blockId];

    nelems2  = blockDim.x*(1+(nelem-1)/blockDim.x);
    ncolor   = ncolors[blockId];

    ind_arg1_size = ind_arg_sizes[0+blockId*1];

    ind_arg1_map = &ind_map[0*set_size] + ind_arg_offs[0+blockId*1];

    //set shared memory pointers
    int nbytes = 0;
    ind_arg1_s = (double *) &shared[nbytes];
  }
  __syncthreads(); // make sure all of above completed

  for ( int n=threadIdx.x; n<ind_arg1_size*1; n+=blockDim.x ){
    ind_arg1_s[n] = ZERO_double;
  }

  __syncthreads();

  for ( int n=threadIdx.x; n<nelems2; n+=blockDim.x ){
    int col2 = -1;
    int map1idx;
    int map2idx;
    if (n<nelem) {
      //initialise local variables
      for ( int d=0; d<1; d++ ){
        arg2_l[d] = ZERO_double;
      }
      map1idx = opDat1Map[n + offset_b + set_size * 1];
      map2idx = opDat1Map[n + offset_b + set_size * 0];


      //user-supplied kernel call
      res_gpu(arg0+(n+offset_b)*1,
        ind_arg0+map1idx*1,
        arg2_l,
        arg3);
      col2 = colors[n+offset_b];
    }

    //store local variables

    int arg2_map;
    if (col2>=0) {
      arg2_map = arg_map[0*set_size+n+offset_b];
    }

    for ( int col=0; col<ncolor; col++ ){
      if (col2==col) {
        arg2_l[0] += ind_arg1_s[0+arg2_map*1];
        ind_arg1_s[0+arg2_map*1] = arg2_l[0];
      }
      __syncthreads();
    }
  }
  for ( int n=threadIdx.x; n<ind_arg1_size*1; n+=blockDim.x ){
    ind_arg1[n%1+ind_arg1_map[n/1]*1] += ind_arg1_s[n];
  }
}


//host stub function
void op_par_loop_res(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3){

  double*arg3h = (double *)arg3.data;
  int nargs = 4;
  op_arg args[4];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(0);
  op_timers_core(&cpu_t1, &wall_t1);
  OP_kernels[0].name      = name;
  OP_kernels[0].count    += 1;


  int    ninds   = 2;
  int    inds[4] = {-1,0,1,-1};

  if (OP_diags>2) {
    printf(" kernel routine with indirection: res\n");
  }

  //get plan
  #ifdef OP_PART_SIZE_0
    int part_size = OP_PART_SIZE_0;
  #else
    int part_size = OP_part_size;
  #endif

  int set_size = op_mpi_halo_exchanges_cuda(set, nargs, args);
  if (set->size > 0) {

    op_plan *Plan = op_plan_get_stage(name,set,part_size,nargs,args,ninds,inds,OP_STAGE_INC);

    //transfer constants to GPU
    int consts_bytes = 0;
    consts_bytes += ROUND_UP(1*sizeof(double));
    reallocConstArrays(consts_bytes);
    consts_bytes = 0;
    arg3.data   = OP_consts_h + consts_bytes;
    arg3.data_d = OP_consts_d + consts_bytes;
    for ( int d=0; d<1; d++ ){
      ((double *)arg3.data)[d] = arg3h[d];
    }
    consts_bytes += ROUND_UP(1*sizeof(double));
    mvConstArraysToDevice(consts_bytes);

    //execute plan

    int block_offset = 0;
    for ( int col=0; col<Plan->ncolors; col++ ){
      if (col==Plan->ncolors_core) {
        op_mpi_wait_all_cuda(nargs, args);
      }
      #ifdef OP_BLOCK_SIZE_0
      int nthread = OP_BLOCK_SIZE_0;
      #else
      int nthread = OP_block_size;
      #endif

      dim3 nblocks = dim3(Plan->ncolblk[col] >= (1<<16) ? 65535 : Plan->ncolblk[col],
      Plan->ncolblk[col] >= (1<<16) ? (Plan->ncolblk[col]-1)/65535+1: 1, 1);
      if (Plan->ncolblk[col] > 0) {
        int nshared = Plan->nsharedCol[col];
        op_cuda_res<<<nblocks,nthread,nshared>>>(
        (double *)arg1.data_d,
        (double *)arg2.data_d,
        arg1.map_data_d,
        (double*)arg0.data_d,
        (double*)arg3.data_d,
        Plan->ind_map,
        Plan->loc_map,
        Plan->ind_sizes,
        Plan->ind_offs,
        block_offset,
        Plan->blkmap,
        Plan->offset,
        Plan->nelems,
        Plan->nthrcol,
        Plan->thrcol,
        Plan->ncolblk[col],
        set->size+set->exec_size);

      }
      block_offset += Plan->ncolblk[col];
    }
    OP_kernels[0].transfer  += Plan->transfer;
    OP_kernels[0].transfer2 += Plan->transfer2;
  }
  op_mpi_set_dirtybit_cuda(nargs, args);
  cutilSafeCall(cudaDeviceSynchronize());
  //update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[0].time     += wall_t2 - wall_t1;
}
