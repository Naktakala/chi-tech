#include"../../../ChiLua/chi_lua.h"
#include<iostream>
#include "../chi_surfacemesh.h"
#include "../../MeshHandler/chi_meshhandler.h"

#include <chi_log.h>

extern ChiLog& chi_log;

/** \defgroup LuaSurfaceMesh Surface Meshes
 * \ingroup LuaMesh
*/

//#############################################################################
/** Creates a new empty surface mesh.

\return Handle int Handle to the created surface mesh.
\ingroup LuaSurfaceMesh
\author Jan*/
int chiSurfaceMeshCreate(lua_State *L)
{
  auto cur_hndlr = chi_mesh::GetCurrentHandler();
  auto new_mesh = new chi_mesh::SurfaceMesh;

  cur_hndlr->surface_mesh_stack.push_back(new_mesh);

  int index = cur_hndlr->surface_mesh_stack.size()-1;
  lua_pushnumber(L,index);

  chi_log.Log(LOG_ALLVERBOSE_2) << "chiSurfaceMeshCreate: "
                                         "Empty SurfaceMesh object, "
                                      << index << ", created" << std::endl;

  return 1;
}

//#############################################################################
/** Creates a new surface mesh from a set of vertex arrays.

\param vertices_x array Array of vertices along x.
\param vertices_y array Array of vertices along y.


##_
Example:
\code
xmesh = {0.0,0.1,0.2,0.3,0.5}
ymesh = {0.0,0.1,0.2,0.3,0.5}
surf_mesh = chiSurfaceMeshCreateFromArrays(xmesh,ymesh)
\endcode

\return Handle int Handle to the created surface mesh.
\ingroup LuaSurfaceMesh
\author Jan*/
int chiSurfaceMeshCreateFromArrays(lua_State *L)
{
  int num_args = lua_gettop(L);
  if (num_args != 2)
    LuaPostArgAmountError("chiSurfaceMeshCreateFromArrays",2,num_args);

  if (not lua_istable(L,1))
  {
    chi_log.Log(LOG_ALLERROR)
      << "In call to chiSurfaceMeshCreateFromArrays: "
      << "The first argument was detected not to be a lua table.";
    exit(EXIT_FAILURE);
  }
  if (not lua_istable(L,2))
  {
    chi_log.Log(LOG_ALLERROR)
      << "In call to chiSurfaceMeshCreateFromArrays: "
      << "The second argument was detected not to be a lua table.";
    exit(EXIT_FAILURE);
  }

  //============================================= Extract x-array
  int xtable_len = lua_rawlen(L,1);
  std::vector<double> values_x(xtable_len,0.0);

  for (int g=0; g<xtable_len; g++)
  {
    lua_pushnumber(L,g+1);
    lua_gettable(L,1);

    values_x[g] = lua_tonumber(L,-1);
    lua_pop(L,1);
  }

  //============================================= Extract y-array
  int ytable_len = lua_rawlen(L,2);
  std::vector<double> values_y(ytable_len,0.0);

  for (int g=0; g<ytable_len; g++)
  {
    lua_pushnumber(L,g+1);
    lua_gettable(L,1);

    values_y[g] = lua_tonumber(L,-1);
    lua_pop(L,1);
  }

  //============================================= Create surface mesh
  auto surface_mesh =
    chi_mesh::SurfaceMesh::CreateFromDivisions(values_x,values_y);

  //============================================= Push to stack
  auto cur_hndlr = chi_mesh::GetCurrentHandler();
  cur_hndlr->surface_mesh_stack.push_back(surface_mesh);

  int index = cur_hndlr->surface_mesh_stack.size()-1;
  lua_pushnumber(L,index);

  chi_log.Log(LOG_ALLVERBOSE_2)
    << "chiSurfaceMeshCreateFromArrays: Created surface mesh "
    << index << std::endl;


  return 1;
}