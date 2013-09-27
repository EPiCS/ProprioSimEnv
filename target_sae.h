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
///  @file target_sae.h
//
///  @brief This is the part of the private and public self-aware engines of a 
///		proprioceptive node (abbr.: SAE) responsible for collecting all types of 
///		sensor information.
///
///  @details
///		It has a memory memory space which is equally shared by the sensors components
///		of the node. Thus, each sensor cmponent has its allocated area on this
///		memory space and can only access it (write access). The stored information
///		on this memory are read by the LModel component. <br>
///		Its Role: Target <br>
///		Its Initiators: LModel - Monitor - SenEnv - OtherNode
///				
//==============================================================================

#ifndef __TARGET_SAE_H__
#define __TARGET_SAE_H__

#include <systemc.h>
#include <tlm.h>									
#include <tlm_utils\simple_target_socket.h>	

#include "constants.h"
#include "memory.h"


class target_sae:	public sc_module
{

public:
// Constructor ================================================================= 
	target_sae	
	( sc_core::sc_module_name		module_name         						///< SC module name
		, const unsigned int        ID                  						///< component ID
		, sc_dt::uint64             memory_size									///< memory size (bytes)
		, unsigned int              memory_width								///< memory width (bytes)
		, const sc_core::sc_time    read_latency								///< read response delay 
		, const sc_core::sc_time    write_latency								///< write response delay 
	);

	
// Method Declarations =========================================================
	//interface methods forward path
	void b_transport		(	int id,											///< sae socket id through which the call came in
								tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								sc_core::sc_time& delay							///< ref to time delay
							);

	bool get_direct_mem_ptr	(	int id,											///< sae socket id through which the call came in
								tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								tlm::tlm_dmi& dmi_data							///< ref to dmi descriptor
							);

	/// Unused interface method
	tlm::tlm_sync_enum nb_transport_fw	(	int id,								///< sae socket id through which the call came in					
											tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay				///< ref to time delay 
										);

	/// Unused interface method
	unsigned int transport_dbg	(	int id,										///< sae socket id through which the call came in
									tlm::tlm_generic_payload& tObj 			 	///< ref to transaction object
								);

	void set_target_idx_for_invalidate_ptr	(	const unsigned int& start_addr	///< const ref to transaction start address
											,	const unsigned int& end_addr	///< const ref to transaction end address
											);
	
// Variable and Object Declarations ============================================
	sc_core::sc_vector< tlm_utils::simple_target_socket_tagged<target_sae, buswidth1> > sae_tsocket_array;	///< vector of tagged simple target socket for communication with iC4 and LM.
	
	unsigned int		sae_mem_size						;					///< memory's size of sae

	unsigned int get_id(void);

private:
	const unsigned int	sae_initiator_nr					;
	unsigned int		sae_id, sae_mem_width				;
	memory				sae_mem								;					// memory
	sc_core::sc_time	sae_read_latency					;
	sc_core::sc_time	sae_write_latency					;

	
};

#endif /* __TARGET_SAE_H__ */