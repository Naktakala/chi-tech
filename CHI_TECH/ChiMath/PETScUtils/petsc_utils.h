#ifndef _chi_math_petscutils_h
#define _chi_math_petscutils_h

#include <petscksp.h>

#include <vector>

namespace chi_math
{
  namespace PETScUtils
  {
    /**Generalized solver structure.*/
    struct PETScSolverSetup
    {
      KSP ksp;
      PC  pc;

      std::string in_solver_name = "KSPSolver";

      std::string solver_type = KSPGMRES;
      std::string preconditioner_type = PCNONE;

      double relative_residual_tol = 1.0e-6;
      int    maximum_iterations = 100;
    };

    //01
    Vec CreateVector(int local_size, int global_size);
    void CreateVector(Vec& x, int local_size, int global_size);

    Vec CreateVectorWithGhosts(int local_size, int global_size,
                               int nghosts,
                               std::vector<int>& ghost_indices);

    //02
    Mat CreateSquareMatrix(int local_size, int global_size);
    void CreateSquareMatrix(Mat& A, int local_size, int global_size);
    void InitMatrixSparsity(Mat A,
                            std::vector<int>& nodal_nnz_in_diag,
                            std::vector<int>& nodal_nnz_off_diag);

    //03
    PETScSolverSetup CreateCommonKrylovSolverSetup(
      Mat ref_matrix,
      std::string in_solver_name = "KSPSolver",
      std::string in_solver_type = KSPGMRES,
      std::string in_preconditioner_type = PCNONE,
      double in_relative_residual_tolerance = 1.0e-6,
      int in_maximum_iterations = 100);

    PetscErrorCode RelativeResidualConvergenceTest(
      KSP ksp, PetscInt n,
      PetscReal rnorm,
      KSPConvergedReason* convergedReason,
      void *monitordestroy);

    PetscErrorCode
    GeneralKSPMonitor(KSP ksp, PetscInt n,
                      PetscReal rnorm, void *monitordestroy);

    //04
    void CopyVecToSTLvector(Vec x, std::vector<double>& data, size_t N);

    void CopyGlobalVecToSTLvector(
      Vec x,
      const std::vector<int>& global_indices,
      std::vector<double>& data);

    void CommunicateGhostEntries(Vec x);

    /**Simple data structure to keep track of a ghost vector's
     * localized views.*/
    struct GhostVecLocalRaw
    {
      Vec     x_localized;
      double* x_localized_raw;

      /**Returns a copy of the value at the specified index.*/
      double operator[](int index)
      {return x_localized_raw[index];}

      /**Returns a reference of the value at the specified index.*/
      double& operator()(int index)
      {return x_localized_raw[index];}
    };

    GhostVecLocalRaw GetGhostVectorLocalViewRead(Vec x);
    void RestoreGhostVectorLocalViewRead(Vec x,GhostVecLocalRaw& local_data);
  }
}

#endif