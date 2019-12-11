#ifndef _chi_sweepbuffer_h
#define _chi_sweepbuffer_h

#include "ChiMesh/SweepUtilities/sweep_namespace.h"
#include <chi_mpi.h>

//#ifndef FLAG_FINISHED
//  #define FLAG_FINISHED     true
//  #define FLAG_NOT_FINISHED false
//#endif

typedef unsigned long long int u_ll_int;

namespace chi_mesh::sweep_management
{


//###################################################################
/**Handles the swift communication of interprocess communication
 * related to sweeping.*/
class SweepBuffer
{
private:
  chi_mesh::sweep_management::AngleSet* const angleset;
  ChiMPICommunicatorSet* const                comm_set;

  bool done_sending;
  bool data_initialized;
  bool upstream_data_initialized;

  u_ll_int EAGER_LIMIT = 32000;

  std::vector<int> prelocI_message_count;
  std::vector<int> deplocI_message_count;
  std::vector<int> delayed_prelocI_message_count;

  std::vector<std::vector<u_ll_int>> prelocI_message_size;
  std::vector<std::vector<u_ll_int>> deplocI_message_size;
  std::vector<std::vector<u_ll_int>> delayed_prelocI_message_size;

  std::vector<std::vector<u_ll_int>> prelocI_message_blockpos;
  std::vector<std::vector<u_ll_int>> deplocI_message_blockpos;
  std::vector<std::vector<u_ll_int>> delayed_prelocI_message_blockpos;

  std::vector<std::vector<bool>> prelocI_message_available;
  std::vector<std::vector<bool>> deplocI_message_sent; //Might be redundant

  std::vector<std::vector<bool>> delayed_prelocI_message_available;

  std::vector<std::vector<MPI_Request>> deplocI_message_request;

  bool messages_received = false;

public:
  int max_num_mess;

  SweepBuffer(chi_mesh::sweep_management::AngleSet* ref_angleset,
              int sweep_eager_limit,
              ChiMPICommunicatorSet* in_comm_set);
  bool DoneSending();
  void BuildMessageStructure();
  void InitializeDelayedUpstreamData();
  void InitializeLocalAndDownstreamBuffers();
  void SendDownstreamPsi(int angle_set_num);
  void ReceiveDelayedData(int angle_set_num);
  void ClearDownstreamBuffers();
  AngleSetStatus ReceiveUpstreamPsi(int angle_set_num);
  void ClearLocalAndReceiveBuffers();
  void Reset();

};
}
#endif