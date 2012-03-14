// 
// auto-generated by op2.m on 14-Mar-2012 14:10:16 
//

// header                                                
                                                         
#include "op_lib_cpp.h"                                  
                                                         
#include "op_cuda_rt_support.h"                          
#include "op_cuda_reduction.h"                           
// global constants                                      
                                                         
#ifndef MAX_CONST_SIZE                                   
#define MAX_CONST_SIZE 128                               
#endif                                                   
                                                         
__constant__ float gam;                                  
__constant__ float gm1;                                  
__constant__ float cfl;                                  
__constant__ float eps;                                  
__constant__ float mach;                                 
__constant__ float alpha;                                
__constant__ float qinf[4];                              
                                                         
void op_decl_const_char(int dim, char const *type,       
            int size, char *dat, char const *name){      
  cutilSafeCall(cudaMemcpyToSymbol(name, dat, dim*size));
}                                                        
                                                         
// user kernel files                                     
                                                         
#include "save_soln_kernel.cu"                           
#include "adt_calc_kernel.cu"                            
#include "res_calc_kernel.cu"                            
#include "bres_calc_kernel.cu"                           
#include "update_kernel.cu"                              
