#include <ChiMesh/MeshContinuum/chi_meshcontinuum.h>
#include "cell.h"

#include "chi_log.h"
#include "chi_mpi.h"

extern ChiLog& chi_log;
extern ChiMPI& chi_mpi;

//###################################################################
/**A private method that initializes a neighbor's parallel information.
 * For now these are 2 items:
 *  - neighbor_partition_id, and
 *  - neighbor_parallel_info_initialized */
void chi_mesh::CellFace::
  InitializeNeighborParallelInfo(chi_mesh::MeshContinuum *grid)
{
  auto adj_cell = grid->cells[neighbor];
  neighbor_partition_id = adj_cell->partition_id;
  neighbor_parallel_info_initialized = true;

  if (neighbor_partition_id == chi_mpi.location_id)
    neighbor_local_id = adj_cell->local_id;
}

//###################################################################
/**Determines the neighbor's partition and whether its local or not.*/
bool chi_mesh::CellFace::
  IsNeighborLocal(chi_mesh::MeshContinuum *grid)
{
  if (neighbor < 0) return false;
  if (chi_mpi.process_count == 1) return true;

  if (not neighbor_parallel_info_initialized)
    InitializeNeighborParallelInfo(grid);

  return (neighbor_partition_id == chi_mpi.location_id);
}

//###################################################################
/**Determines the neighbor's partition.*/
int chi_mesh::CellFace::
  GetNeighborPartitionID(chi_mesh::MeshContinuum *grid)
{
  if (neighbor < 0) return -1;
  if (chi_mpi.process_count == 1) return 0;

  if (not neighbor_parallel_info_initialized)
    InitializeNeighborParallelInfo(grid);

  return neighbor_partition_id;
}

//###################################################################
/**Determines the neighbor's local id.*/
int chi_mesh::CellFace::
  GetNeighborLocalID(chi_mesh::MeshContinuum *grid)
{
  if (neighbor < 0) return -1;
  if (chi_mpi.process_count == 1) return neighbor;

  if (not neighbor_parallel_info_initialized)
    InitializeNeighborParallelInfo(grid);

  return neighbor_local_id;
}

//###################################################################
/**Determines the neighbor's associated face.*/
int chi_mesh::CellFace::
  GetNeighborAssociatedFace(chi_mesh::MeshContinuum *grid)
{
  auto& cur_face = *this;
  //======================================== Check index validity
  if ((cur_face.neighbor<0) || (not cur_face.IsNeighborLocal(grid)))
  {
    chi_log.Log(LOG_ALLERROR)
      << "Invalid cell index encountered in call to "
      << "CellFace::GetNeighborAssociatedFace. Index points "
      << "to either a boundary"
      << "or a non-local cell.";
    exit(EXIT_FAILURE);
  }

  auto adj_cell = &grid->local_cells[cur_face.GetNeighborLocalID(grid)];

  int associated_face = -1;
  if (cur_face.neighbor_ass_face < 0)
  {
    std::set<int> cfvids(cur_face.vertex_ids.begin(),
                         cur_face.vertex_ids.end());
    //======================================== Loop over adj cell faces
    int af=-1;
    for (auto& adj_face : adj_cell->faces)
    {
      ++af;
      std::set<int> afvids(adj_face.vertex_ids.begin(),
                           adj_face.vertex_ids.end());

      if (afvids == cfvids) {associated_face = af; break;}
    }
  }
  else
    associated_face = cur_face.neighbor_ass_face;

  //======================================== Check associated face validity
  if (associated_face<0)
  {
    chi_log.Log(LOG_ALLERROR)
      << "Could not find associated face in call to "
      << "CellFace::GetNeighborAssociatedFace.\n"
      << "Reference face with centroid at: "
      << cur_face.centroid.PrintS() << "\n"
      << "Adjacent cell: " << adj_cell->global_id;
    for (int af=0; af < adj_cell->faces.size(); af++)
    {
      chi_log.Log(LOG_ALLERROR)
        << "Adjacent cell face " << af << " centroid "
        << adj_cell->faces[af].centroid.PrintS();
    }
    exit(EXIT_FAILURE);
  }

  cur_face.neighbor_ass_face = associated_face;
  return associated_face;
}

//###################################################################
/**Computes the face area.*/
double chi_mesh::CellFace::ComputeFaceArea(chi_mesh::MeshContinuum *grid)
{
  if (vertex_ids.size() <= 1)
    return 1.0;
  else if (vertex_ids.size() == 2)
  {
    auto& v0 = *grid->vertices[vertex_ids[0]];
    auto& v1 = *grid->vertices[vertex_ids[1]];

    return (v1 - v0).Norm();
  }
  else
  {
    double area = 0.0;
    auto& v2 = centroid;
    const auto num_verts = vertex_ids.size();
    for (int v=0; v<num_verts; ++v)
    {
      int vid0 = vertex_ids[v];
      int vid1 = (v < (num_verts-1))? vertex_ids[v+1] : vertex_ids[0];

      auto& v0 = *grid->vertices[vid0];
      auto& v1 = *grid->vertices[vid1];

      auto v01 = v1-v0;
      auto v02 = v2-v0;

      chi_mesh::Matrix3x3 J;
      J.SetColJVec(0,v01);
      J.SetColJVec(1,v02);
      J.SetColJVec(2,chi_mesh::Vector3(1.0,1.0,1.0));

      area += 0.5*std::fabs(J.Det());
    }

    return area;
  }

}