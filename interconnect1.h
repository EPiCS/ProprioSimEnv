/*
*
* Author: Tatiana Djaba Nya (Lead author)
* Author: Stephan C. Stilkerich
*
* Reference Architecture Model (EPiCS FP7 FET program, No. 257906)
*           - Peter R. Lewis, University of Birmingham 
*           - Xin Yao, University of Birmingham
*
* Copyright (c) 2013, EADS Deutschland GmbH, EADS Innovation Works
*
*==============================================================================
*
*This file is part of ProprioSimEnv.
*
*    ProprioSimEnv is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    ProprioSimEnv is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with ProprioSimEnv.  If not, see <http://www.gnu.org/licenses/>.
*/

//==============================================================================
///  @file interconnect1.h
//
///  @brief This is the interconnect component between the sensors and the SAE - abbr.: IC1
//
///  @details
///		It forwards the methods calls between all the initiators senenv, othernode and 
///		their target sae. <br>
///		It adjusts the address parameter of the transaction object before forwarding each call.
///		to prevent the sensors for overwriting one another's data.
///		
//
//==============================================================================

#ifndef __INTERCONNECT1_H__
#define __INTERCONNECT1_H__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils\simple_initiator_socket.h>
#include <tlm_utils\simple_target_socket.h>
#include <tlm_utils\multi_passthrough_target_socket.h>

#include "constants.h"

class interconnect1 :	public sc_module
{
public:
// Constructor =================================================================
	interconnect1	( sc_module_name name 													///< sc module name
					, const unsigned int tsocket_number										///< number of target sockets
					, const unsigned int sae_memory_size_									///< memory size of the sae component
					);

// Method Declarations =========================================================
	//interface methods backward path
	void invalidate_direct_mem_ptr	( 	sc_dt::uint64 start_range,  						///< start address of the memory range
										sc_dt::uint64 end_range 							///< end address of the memory range
									);

	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_bw	( 	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
											tlm::tlm_phase& phase, 							///< ref to transaction phase
											sc_core::sc_time& delay 						///< ref to delay time
										);
	
	//interface methods forward path
	void b_transport(	int id,																///< ID of the tagged target socket receiving the call 
						tlm::tlm_generic_payload& tObj, 									///< ref to transaction object
						sc_core::sc_time& delay												///< ref to delay time
					);

	bool get_direct_mem_ptr( int id,														///< ID of the tagged target socket receiving the call 
							 tlm::tlm_generic_payload& tObj, 								///< ref to transaction object
							 tlm::tlm_dmi& dmi_data											///< ref to dmi descriptor
							);
							
	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_fw	( 	int id,											///< ID of the tagged target socket receiving the call 
											tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
											tlm::tlm_phase& phase, 							///< ref to transaction phase
											sc_core::sc_time& delay 						///< ref to delay time
										);
										
	/// Required and unused interface method
	unsigned int transport_dbg	( 	int id,													///< ID of the tagged target socket receiving the call 
									tlm::tlm_generic_payload& tObj 							///< ref to transaction object
								);

	void address_mapping(	int idx,														///< index of the tagged target socket receiving the call 
							tlm::tlm_generic_payload& tObj,									///< ref to transaction object
							bool direction													///< used communication path (forward or backward)
						);

// Variable and Object Declarations ============================================
	tlm_utils::simple_initiator_socket<interconnect1, buswidth1>		ic1_isocket;		///< simple initiator socket for communication with sae
	sc_core::sc_vector<tlm_utils::simple_target_socket_tagged<interconnect1, buswidth1>> ic1_tsocket_array;	///< vector of tagged simple target sockets for communication with the sensors
	
private:
	unsigned int				ic1_tsocket_nr;
	const unsigned int			sae_memory_size;
};
#endif /*__INTERCONNECT1_H__*/