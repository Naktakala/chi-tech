#include "diffusion_solver.h"

#include <PiecewiseLinear/CellViews/pwl_slab.h>
#include <PiecewiseLinear/CellViews/pwl_polygon.h>
#include <PiecewiseLinear/CellViews/pwl_polyhedron.h>

#include <ChiTimer/chi_timer.h>

#include <chi_mpi.h>
#include <chi_log.h>
#include <CHI_PHYSICS/chi_physics.h>

extern CHI_MPI chi_mpi;
extern CHI_LOG chi_log;
extern CHI_PHYSICS chi_physics_handler;

extern ChiTimer chi_program_timer;


//###################################################################
/**Builds the matrix using the PWLD_IP discretization method.*/
int chi_diffusion::Solver::ExecutePWLD_MIP_GAGG(bool suppress_assembly,
                                                bool suppress_solve)
{
  pwl_discr = ((SpatialDiscretization_PWL*)(this->discretization));

  //=================================== Verbose print solver info
  if (chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
  {
    MatInfo info;
    ierr = MatGetInfo(A,MAT_GLOBAL_SUM,&info);

    chi_log.Log(LOG_0) << "Number of mallocs used = " << info.mallocs
                              << "\nNumber of non-zeros allocated = "
                              << info.nz_allocated
                              << "\nNumber of non-zeros used = "
                              << info.nz_used
                              << "\nNumber of unneeded non-zeros = "
                              << info.nz_unneeded;
  }

  //################################################## Assemble Amatrix
  if (verbose || chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
  {
    chi_log.Log(LOG_0)
    << chi_program_timer.GetTimeString() << " "
    << solver_name << ": Assembling A and b";
    chi_log.Log(LOG_0)
    << chi_program_timer.GetTimeString() << " "
    << solver_name << ": Local matrix instructions";
  }
  t_assembly.Reset();

  std::vector<int> boundary_nodes,boundary_numbers;

  if (chi_physics_handler.material_stack.size()==0)
  {
    chi_log.Log(LOG_0ERROR)
      << "No materials added to simulation. Add materials.";
    exit(EXIT_FAILURE);
  }

  //================================================== Setting references
  xref = x;
  bref = b;
  Aref = A;

  VecSet(xref,0.0);
  VecSet(bref,0.0);

  if (!suppress_assembly)
    chi_log.Log(LOG_0) << chi_program_timer.GetTimeString() << " "
                       << solver_name << ": Assembling A locally";

  //================================================== Loop over locally owned
  //                                                   cells
  size_t num_local_cells = grid->local_cell_glob_indices.size();
  for (int lc=0; lc<num_local_cells; lc++)
  {
    int glob_cell_index = grid->local_cell_glob_indices[lc];
    chi_mesh::Cell* cell = grid->cells[glob_cell_index];

    DIFFUSION_IP_VIEW* cell_ip_view = ip_cell_views[lc];

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% If SLAB
    if (typeid(*cell) == typeid(chi_mesh::CellSlab))
    {
      if (!suppress_assembly)
        PWLD_Ab_Slab_GAGG(glob_cell_index,cell,cell_ip_view);
      else
        PWLD_b_Slab_GAGG(glob_cell_index,cell,cell_ip_view);
    }//if typeid %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% If POLYGON
    else if (typeid(*cell) == typeid(chi_mesh::CellPolygon))
    {
      if (!suppress_assembly)
        PWLD_Ab_Polygon_GAGG(glob_cell_index,cell,cell_ip_view);
      else
        PWLD_b_Polygon_GAGG(glob_cell_index,cell,cell_ip_view);
    }//if typeid %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% If POLYHEDRON
    else if (typeid(*cell) == typeid(chi_mesh::CellPolyhedron))
    {
      if (!suppress_assembly)
        PWLD_Ab_Polyhedron_GAGG(glob_cell_index, cell, cell_ip_view);
      else
        PWLD_b_Polyhedron_GAGG(glob_cell_index, cell, cell_ip_view);
    }//if typeid %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    else
    {
      chi_log.Log(LOG_ALLERROR)
        << "Invalid cell-type encountered in chi_diffusion::Solver::ExecuteS";
    }
  }

  //=================================== Call matrix assembly
  if (verbose || chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
    chi_log.Log(LOG_0)
    << chi_program_timer.GetTimeString() << " "
    << solver_name << ": Communicating matrix assembly";

  if (!suppress_assembly)
  {
    chi_log.Log(LOG_0) << chi_program_timer.GetTimeString() << " "
                       << solver_name << ": Assembling A globally";
    MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);

    //================================= Matrix symmetry check
//    chi_log.Log(LOG_0) << "Symmetry check";
//    PetscBool is_symmetric;
//    ierr = MatIsSymmetric(A,1.0e-4,&is_symmetric);
//    if (!is_symmetric)
//    {
//      chi_log.Log(LOG_0WARNING)
//        << "Assembled matrix is not symmetric";
//    }
    chi_log.Log(LOG_0) << chi_program_timer.GetTimeString() << " "
                       << solver_name << ": Diagonal check";
    PetscBool missing_diagonal;
    PetscInt  row;
    MatMissingDiagonal(A,&missing_diagonal,&row);
    if (missing_diagonal)
      chi_log.Log(LOG_ALLERROR) << chi_program_timer.GetTimeString() << " "
                                << solver_name << ": Missing diagonal detected";
  }
  if (verbose || chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
    chi_log.Log(LOG_0) << chi_program_timer.GetTimeString() << " "
                       << solver_name << ": Assembling x and b";
  VecAssemblyBegin(x);
  VecAssemblyEnd(x);
  VecAssemblyBegin(b);
  VecAssemblyEnd(b);


  time_assembly = t_assembly.GetTime()/1000.0;

  //=================================== Execute solve
  if (suppress_solve)
  {
    chi_log.Log(LOG_0) << chi_program_timer.GetTimeString() << " "
                       << solver_name
                       << ": Setting up solver and preconditioner\n";
    PCSetUp(pc);
    KSPSetUp(ksp);
  }
  else
  {
    if (verbose || chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
      chi_log.Log(LOG_0) << chi_program_timer.GetTimeString() << " "
                         << solver_name << ": Solving system\n";
    t_solve.Reset();
    PCSetUp(pc);
    KSPSetUp(ksp);
    KSPSolve(ksp,b,x);
    time_solve = t_solve.GetTime()/1000.0;

    //=================================== Populate field vector
    const double* x_ref;
    VecGetArrayRead(x,&x_ref);

    for (int i=0; i<pwld_local_dof_count*G; i++)
    {
      pwld_phi_local[i] = x_ref[i];
    }
    VecRestoreArrayRead(x,&x_ref);

    //=================================== Get convergence reason
    KSPConvergedReason reason;
    KSPGetConvergedReason(ksp,&reason);
    if (verbose || reason != KSP_CONVERGED_RTOL)
      chi_log.Log(LOG_0) << "Convergence reason: " << reason;


    //=================================== Location wise view
    if (chi_mpi.location_id == 0)
    {
      int its;
      ierr = KSPGetIterationNumber(ksp,&its);
      chi_log.Log(LOG_0)
        << chi_program_timer.GetTimeString() << " "
        << solver_name
        << "[g=" << gi << "-" << gi+G-1
        << "]: Number of iterations =" << its;

      if (verbose || chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
      {
        chi_log.Log(LOG_0) << "Timing:";
        chi_log.Log(LOG_0) << "Assembling the matrix: " << time_assembly;
        chi_log.Log(LOG_0) << "Solving the system   : " << time_solve;
      }
    }

    if (verbose || chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
      chi_log.Log(LOG_0) << "Diffusion Solver execution completed!\n";
  }//if not suppressed solve

  return 0;
}