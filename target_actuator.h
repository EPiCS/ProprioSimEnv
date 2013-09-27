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
///  @file target_actuator.h
//
///  @brief This is the internal actuator of a proprioceptive node
//
///  @details It is activated by the see components. A node may have as many actuators. <br>
///		The required number must be specified by the user during the node's instantiation <br>
///		Its role: target	
///  
//
//==============================================================================

#ifndef __TARGET_ACTUATOR_H__
#define __TARGET_ACTUATOR_H__

#include <systemc.h>
#include <tlm.h>									
//#include <tlm_utils\simple_target_socket.h>	// TLM headers

#include "constants.h"
#include "memory.h"
#include "act_core.h"

class target_actuator:	public sc_module, virtual public tlm::tlm_fw_transport_if<>
{

public:
// Constructor ================================================================= 
	target_actuator
		( sc_core::sc_module_name	module_name									///< SC module name
		, const unsigned int        ID											///< component ID
		, sc_dt::uint64             memory_size									///< memory size  (bytes)
		, unsigned int              memory_width								///< memory width (bytes)
		, const sc_core::sc_time    read_latency								///< read response delay 
		, const sc_core::sc_time    write_latency								///< write response delay
		, const sc_core::sc_time	glob_quantum_								///< time value for the global quantum
		);

	SC_HAS_PROCESS(target_actuator);
// Method Declarations =========================================================
	//
	/*void actuator_thread(void);*/

	//interface methods forward path
	void b_transport		(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								sc_core::sc_time& delay							///< ref to time delay
							);

	bool get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								tlm::tlm_dmi& dmi_data							///< ref to dmi descriptor
							);

    /// Unsused interface method
	tlm::tlm_sync_enum nb_transport_fw	(	tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay				///< ref to time delay 
										);
    
	/// Unsused interface method
	unsigned int transport_dbg	(	tlm::tlm_generic_payload& tObj 			 	///< ref to transaction object
								);


// Variable and Object Declarations ============================================
	tlm::tlm_target_socket<buswidth4> act_tsocket;								///< standard target socket for communication with iC4
	
private:
	memory				act_mem				;									///< reserved memory block
	std::string 		act_name			;									///< component name
	unsigned int		act_id				;									///< component id
	act_core			*act_core_obj		;

};

#endif /* __TARGET_ACTUATOR_H__ */