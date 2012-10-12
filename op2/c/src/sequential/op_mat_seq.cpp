/*
  Open source copyright declaration based on BSD open source template:
  http://www.opensource.org/licenses/bsd-license.php

* Copyright (c) 2011, Florian Rathgeber
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Mike Giles may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Mike Giles ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Mike Giles BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <assert.h>
#include <algorithm>
#include <iostream>
#include <iterator>

#include <petscksp.h>

#include "op_lib_mat.h"

op_sparsity op_decl_sparsity ( op_map *rowmaps, op_map *colmaps, int nmaps, int *dim, int ndim, char const * name )
{
  assert(rowmaps && colmaps);
  op_sparsity sparsity = op_decl_sparsity_core(rowmaps, colmaps, nmaps, dim, ndim, name);

  return sparsity;
}

op_mat op_decl_mat( op_sparsity sparsity, int *dims, int ndim, char const * type, int type_size, char const * name )
{
  assert( sparsity );
  op_mat mat = op_decl_mat_core ( sparsity, dims, ndim, type, type_size, name );

  Mat p_mat;
  int size;
  MPI_Comm comm= PETSC_COMM_WORLD;
  MPI_Comm_size(comm, &size);
  if (size > 1) {
    // Create a PETSc CSR sparse matrix and pre-allocate storage
    MatCreateMPIAIJ(comm,
                    sparsity->nrows, // Local number of rows
                    sparsity->ncols, // Local number of columns
                    PETSC_DETERMINE, // Global number of rows,
                    PETSC_DETERMINE, // Global number of columns,
                    PETSC_DEFAULT,   // unused
                    (const PetscInt*)sparsity->d_nnz, // diagonal subblock
                    PETSC_DEFAULT,
                    (const PetscInt*)sparsity->o_nnz, // offdiagonal subblock)
                    &p_mat);
    // These are wrong for petsc MPI matrices I think, so make them NULL.
    free(sparsity->rowptr);
    sparsity->rowptr = NULL;
    free(sparsity->colidx);
    sparsity->colidx = NULL;
  } else {
    // Create a PETSc CSR sparse matrix and pre-allocate storage
    MatCreateSeqAIJ(comm,
                    sparsity->nrows, // Number of rows
                    sparsity->ncols, // Number of columns
                    PETSC_DEFAULT,   // unused
                    (const PetscInt*)sparsity->d_nnz, // diagonal block
                    &p_mat);
    // Set the column indices (FIXME: benchmark if this is worth it)
    MatSeqAIJSetColumnIndices(p_mat, (PetscInt*)sparsity->colidx);
  }
  // FIXME: work out what is necessary to make OP2 do redundant
  // computation in L1 element halos.
  //MatSetOption(p_mat, MAT_IGNORE_OFF_PROC_ENTRIES, PETSC_TRUE);
  MatZeroEntries(p_mat);
  mat->mat = p_mat;
  return mat;
}

void op_mat_destroy ( op_mat m )
{
  if ( m->mat == NULL )
  {
    return;
  }
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2)
  MatDestroy ( ((Mat*)&(m->mat)) );
#else
  MatDestroy ( ((Mat)(m->mat)) );
#endif
  m->mat = NULL;
}

op_arg op_arg_mat ( op_mat mat, int rowidx, op_map rowmap, int colidx, op_map colmap, int *dims, const char * typ, op_access acc )
{
  return op_arg_mat_core(mat, rowidx, rowmap, colidx, colmap, dims, typ, acc);
}

template < typename T >
static inline int is_float(T dat)
{
  return strncmp(dat->type, "float", 5) == 0;
}

template < typename T >
static inline PetscScalar * to_petsc(T dat, const void * values, int size)
{
  PetscScalar * dvalues;
  // If we're passed float data, we have to convert it to double
  if (is_float(dat)) {
    dvalues = (PetscScalar*)malloc(sizeof(PetscScalar) * size);
    for ( int i = 0; i < size; i++ )
      dvalues[i] = (PetscScalar)(((float*)values)[i]);
  } else {
    dvalues = (PetscScalar *) values;
  }
  return dvalues;
}

void op_mat_addto_scalar( op_mat mat, const void* value, int row, int col )
{
  assert( mat && value);
  PetscScalar v[1];
  if (is_float(mat))
    v[0] = (PetscScalar)((float *)value)[0];
  else
    v[0] = ((const PetscScalar *)value)[0];

  if ( v[0] == 0.0 ) {
    return;
  }
  op_set s = mat->sparsity->rowmaps[0]->to;
  row = op_global_index(s, row);
  col = op_global_index(s, col);

  MatSetValues( (Mat) mat->mat,
                1, (const PetscInt *)&row,
                1, (const PetscInt *)&col,
                v, ADD_VALUES );
}

void op_mat_addto( op_mat mat, const void* values, int nrows, const int *irows, int ncols, const int *icols )
{
  assert( mat && values && irows && icols );

  PetscScalar * dvalues = to_petsc(mat, values, nrows * ncols);

  op_set s = mat->sparsity->rowmaps[0]->to;
  PetscInt *rows = (PetscInt *)malloc(nrows * sizeof(PetscInt));
  PetscInt *cols = (PetscInt *)malloc(ncols * sizeof(PetscInt));
  for ( int i = 0; i < nrows; i++ ) {
    rows[i] = op_global_index(s, irows[i]);
  }
  for ( int i = 0; i < ncols; i++ ) {
    cols[i] = op_global_index(s, icols[i]);
  }
  MatSetValues( (Mat) mat->mat,
                nrows, (const PetscInt *)rows,
                ncols, (const PetscInt *)cols,
                dvalues, ADD_VALUES);

  free(rows);
  free(cols);
  if (is_float(mat)) free(dvalues);
}

void op_mat_assemble( op_mat mat )
{
  assert( mat );

  MatAssemblyBegin((Mat) mat->mat,MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd((Mat) mat->mat,MAT_FINAL_ASSEMBLY);
}

static Vec op_create_vec ( const op_dat vec ) {
  assert( vec );

  PetscScalar * dvalues = to_petsc(vec, vec->data, vec->dim * vec->set->size);

  Vec p_vec;
  int size;
  MPI_Comm comm = PETSC_COMM_WORLD;
  MPI_Comm_size(comm, &size);
  if ( size > 1 ) {
    VecCreateMPIWithArray(comm, vec->dim * vec->set->size,
                          PETSC_DETERMINE, dvalues, &p_vec);
  } else {
    // Create a PETSc vector and pass it the user-allocated storage
    VecCreateSeqWithArray(comm, vec->dim * vec->set->size, dvalues, &p_vec);
  }
  VecAssemblyBegin(p_vec);
  VecAssemblyEnd(p_vec);

  return p_vec;
}

static void op_destroy_vec ( Vec v, op_dat d ) {
  // If the op_dat holds float data we need to copy out
  if (is_float(d)) {
    PetscScalar * a;
    VecGetArray(v, &a);
    for (int i = 0; i < d->dim * d->set->size; i++) {
      ((float *)d->data)[i] = (float)a[i];
    }
  }
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2)
  VecDestroy(&v);
#else
  VecDestroy(v);
#endif
}

void op_mat_mult ( const op_mat mat, const op_dat v_in, op_dat v_out )
{
  assert( mat && v_in && v_out );

  Vec p_v_in = op_create_vec(v_in);
  Vec p_v_out = op_create_vec(v_out);

  MatMult((Mat) mat->mat, p_v_in, p_v_out);

  op_destroy_vec(p_v_in, v_in);
  op_destroy_vec(p_v_out, v_out);
}

void op_mat_zero ( op_mat mat )
{
  MatZeroEntries((Mat) mat->mat);
}

int op_ksp_types(char ***array)
{
  // Create and destroy a KSP context to ensure that KSPList is populated.
  KSP ksp;
  KSPCreate(PETSC_COMM_WORLD,&ksp);
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2)
  KSPDestroy(&ksp);
#else
  KSPDestroy(ksp);
#endif

  int n;
  PetscFListGet(KSPList, array, &n);
  return n;
}

int op_pc_types(char ***array)
{
  // Create and destroy a KSP context to ensure that KSPList is populated.
  PC pc;
  PCCreate(PETSC_COMM_WORLD, &pc);
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2)
  PCDestroy(&pc);
#else
  PCDestroy(pc);
#endif

  int n;
  PetscFListGet(PCList, array, &n);
  return n;
}

void op_solve ( const op_mat mat, const op_dat b, op_dat x, char *ksptype,
                char *pctype, double tol, int maxits )
{
  assert( mat && b && x );

  Vec p_b = op_create_vec(b);
  Vec p_x = op_create_vec(x);
  Mat A = (Mat) mat->mat;
  KSP ksp;
  PC pc;

  KSPCreate(PETSC_COMM_WORLD,&ksp);
  KSPSetFromOptions(ksp);
  KSPSetType(ksp,ksptype);
  KSPSetOperators(ksp,A,A,DIFFERENT_NONZERO_PATTERN);
  KSPGetPC(ksp,&pc);
  PCSetType(pc,pctype);
  KSPSetTolerances(ksp,tol,PETSC_DEFAULT,PETSC_DEFAULT,maxits);

  KSPSolve(ksp,p_b,p_x);

  op_destroy_vec(p_b, b);
  op_destroy_vec(p_x, x);
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2)
  KSPDestroy(&ksp);
#else
  KSPDestroy(ksp);
#endif
}

void* op_create_ksp()
{
  KSP ksp;
  KSPCreate(PETSC_COMM_WORLD,&ksp);
  KSPSetFromOptions(ksp);
  return (void*)ksp;
}

void op_destroy_ksp(void *ksp)
{
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2)
  KSPDestroy((KSP*)(&ksp));
#else
  KSPDestroy((KSP)ksp);
#endif
}

int op_get_converged_reason(void *ksp)
{
  KSPConvergedReason reason;
  KSPGetConvergedReason((KSP)ksp, &reason);
  return (int)reason;
}

int op_ksp_get_iteration_number(void *ksp)
{
  int its;
  KSPGetIterationNumber((KSP)ksp, &its);
  return its;
}

void op_ksp_set_type(void *ksp, const char *type)
{
  KSPSetType((KSP)ksp, type);
}

void* op_ksp_get_pc(void *ksp)
{
  PC pc;
  KSPGetPC((KSP)ksp, &pc);
  return (void*)pc;
}

void op_pc_set_type(void *pc, const char *type)
{
  PCSetType((PC)pc, type);
}

void op_ksp_set_tolerances(void *ksp, double rtol, double atol, double dtol, int maxits)
{
  KSPSetTolerances((KSP)ksp, rtol, atol, dtol, maxits);
}

void op_ksp_solve(void *ksp, op_mat mat, op_dat x, op_dat b)
{
  assert( mat && b && x );

  Vec p_b = op_create_vec(b);
  Vec p_x = op_create_vec(x);
  Mat A = (Mat) mat->mat;

  KSPSetOperators((KSP)ksp,A,A,DIFFERENT_NONZERO_PATTERN);
  KSPSolve((KSP)ksp,p_b,p_x);

  op_destroy_vec(p_b, b);
  op_destroy_vec(p_x, x);
}

// FIXME: Only correct for PETSc configured with --with-scalar-type=real,
// --with-precision=double
void op_mat_get_values ( const op_mat mat, double **v, int *m, int *n)
{
  MatGetSize((Mat) mat->mat, m, n);
  *v = (double *) malloc(sizeof(PetscScalar)*m[0]*n[0]);
  int *idxm = (int *) malloc(sizeof(int)*m[0]);
  int *idxn = (int *) malloc(sizeof(int)*n[0]);

  for (int i=0; i<m[0]; ++i) idxm[i] = i;
  for (int i=0; i<n[0]; ++i) idxn[i] = i;

  MatGetValues((Mat) mat->mat, m[0], idxm, n[0], idxn, *v);
}

void op_mat_zero_rows ( op_mat mat, int n, const int *rows, double val)
{
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2)
  MatZeroRows( (Mat)mat->mat, n, rows, val, PETSC_NULL, PETSC_NULL );
#else
  MatZeroRows( (Mat)mat->mat, n, rows, val);
#endif
}

void op_mat_get_array( const op_mat mat)
{
  MatGetArray((Mat) mat->mat, (MatScalar**) &mat->mat_array);
}

void op_mat_put_array( const op_mat mat)
{
  MatRestoreArray((Mat) mat->mat, (MatScalar**) &mat->mat_array);
}
