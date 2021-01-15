#include "diffusion_solver.h"

#include "ChiMath/SpatialDiscretization/PiecewiseLinear/CellViews/pwl_slab.h"
#include "ChiMath/SpatialDiscretization/PiecewiseLinear/CellViews/pwl_polygon.h"
#include "ChiMath/SpatialDiscretization/PiecewiseLinear/CellViews/pwl_polyhedron.h"

#include "ChiTimer/chi_timer.h"

#include "chi_mpi.h"
#include "chi_log.h"
#include "ChiPhysics/chi_physics.h"

extern ChiMPI& chi_mpi;
extern ChiLog& chi_log;
extern ChiPhysics&  chi_physics_handler;

extern ChiTimer chi_program_timer;


//###################################################################
/**Builds the matrix using the PWLD_MIP discretization method and
 * uses multiple groups.*/
int chi_diffusion::Solver::ExecutePWLD_MIP_GRPS(bool suppress_assembly,
                                                bool suppress_solve)
{
  //=================================== Verbose print solver info
  if (chi_log.GetVerbosity()>=LOG_0VERBOSE_1)
  {
    MatInfo info;
    ierr = MatGetInfo(A,MAT_GLOBAL_SUM,&info);

    chi_log.Log(LOG_0VERBOSE_1) << "Number of mallocs used = " << info.mallocs
                              << "\nNumber of non-zeros allocated = "
                              << info.nz_allocated
                              << "\nNumber of non-zeros used = "
                              << info.nz_used
                              << "\nNumber of unneeded non-zeros = "
                              << info.nz_unneeded;
  }

  //################################################## Assemble Amatrix
  chi_log.Log(LOG_0) << "Diffusion Solver: Assembling A and b";
  chi_log.Log(LOG_0) << "Diffusion Solver: Local matrix instructions";
  t_assembly.Reset();


  std::vector<int> boundary_nodes,boundary_numbers;

  if (chi_physics_handler.material_stack.empty())
  {
    chi_log.Log(LOG_0ERROR)
      << "No materials added to simulation. Add materials.";
    exit(0);
  }

  //======================================================= Loop over groups
  for (int gr=0; gr<G; gr++)
  {
    //================================================== Setting references
    xref = xg[gr];
    bref = bg[gr];
    Aref = Ag[gr];

    VecSet(xref,0.0);
    VecSet(bref,0.0);

    //================================================== Loop over locally owned
    //                                                   cells
    for (auto& cell : grid->local_cells)
    {
      auto cell_ip_view = ip_cell_views[cell.local_id];

      if (!suppress_assembly)
        PWLD_Assemble_A_and_b(cell.global_id, &cell, cell_ip_view, gi + gr);
      else
        PWLD_Assemble_b(cell.global_id, &cell, cell_ip_view, gi + gr);
    }
  }//for gr



  //=================================== Call matrix assembly
  chi_log.Log(LOG_0) << "Diffusion Solver: Communicating matrix assembly";

  if (!suppress_assembly)
  {
    for (int gr=0; gr<G; gr++)
    {
      MatAssemblyBegin(Ag[gr],MAT_FINAL_ASSEMBLY);
      MatAssemblyEnd(Ag[gr],MAT_FINAL_ASSEMBLY);

      VecAssemblyBegin(xg[gr]);
      VecAssemblyEnd(xg[gr]);

      //================================= Matrix symmetry check
      PetscBool is_symmetric;
      ierr = MatIsSymmetric(Ag[gr],1.0e-4,&is_symmetric);
      if (!is_symmetric)
      {
        chi_log.Log(LOG_0WARNING)
          << "Group " << gi+gr << " "
          << "Assembled matrix is not symmetric";
      }
    }//for gr
  }

  for (int gr=0; gr<G; gr++)
  {
    VecAssemblyBegin(bg[gr]);
    VecAssemblyEnd(bg[gr]);
  }//for gr


  time_assembly = t_assembly.GetTime()/1000.0;

  //=================================== Execute solve
  if (suppress_solve)
  {
    chi_log.Log(LOG_0) << "Diffusion Solver: Setting up solver\n";
    for (int gr=0; gr<G; gr++)
    {
      PCSetUp(pcg[gr]);
      KSPSetUp(kspg[gr]);
    }
  }
  else
  {
    chi_log.Log(LOG_0) << "Diffusion Solver: Solving system\n";

    for (int gr=0; gr<G; gr++)
    {
      t_solve.Reset();
      PCSetUp(pcg[gr]);
      KSPSetUp(kspg[gr]);
      KSPSolve(kspg[gr],bg[gr],xg[gr]);
      time_solve = t_solve.GetTime()/1000.0;

      //=================================== Populate field vector
      const double* x_ref;
      VecGetArrayRead(xg[gr],&x_ref);

      for (int i=0; i < local_dof_count; i++)
      {
        int ir = i*G + gr;
        pwld_phi_local[ir] = x_ref[i];
      }
      VecRestoreArrayRead(xg[gr],&x_ref);

      //=================================== Get convergence reason
      KSPConvergedReason reason;
      KSPGetConvergedReason(kspg[gr],&reason);
      //chi_log.Log(LOG_0) << "Convergence reason: " << reason;


      //=================================== Location wise view
      if (chi_mpi.location_id == 0)
      {
        int its;
        ierr = KSPGetIterationNumber(kspg[gr],&its);
        chi_log.Log(LOG_0)
          << chi_program_timer.GetTimeString() << " "
          << solver_name
          << "[g=" << gi+gr
          << "]: Number of iterations =" << its;
//        chi_log.Log(LOG_0) << "Timing:";
//        chi_log.Log(LOG_0) << "Assembling the matrix: " << time_assembly;
//        chi_log.Log(LOG_0) << "Solving the system   : " << time_solve;
      }
    }//for gr


    //chi_log.Log(LOG_0) << "Diffusion Solver Solution completed!\n";
  }//if not suppressed solve

  //chi_log.Log(LOG_0) << "Diffusion Solver execution completed!\n";
  return 0;
}
