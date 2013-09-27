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
///  @file target_extaction.h
//
///  @brief This the external actuator of a proprioceptive node
//
///  @details It is activated by the see components. A node may have as many actuators <br>
///		The required number must be specified by the user during the node's instantiation  <br>
///		Its role: target
///  
//
//==============================================================================

#ifndef __target_extaction_H__
#define __target_extaction_H__

#include <systemc.h>
#include <tlm.h>									
#include <tlm_utils\simple_target_socket.h>	// TLM headers

#include "constants.h"
#include "memory.h"

class target_extaction:	public sc_module
{

public:
// Constructor ================================================================= 
	target_extaction
		( sc_core::sc_module_name	module_name									///< SC module name
		, const unsigned int        ID											///< component ID
		, sc_dt::uint64             memory_size									///< memory size (bytes)
		, unsigned int              memory_width								///< memory width (bytes)
		, const sc_core::sc_time    read_latency								///< read response delay
		, const sc_core::sc_time    write_latency								///< write response delay 
		);


// Method Declarations =========================================================
	//interface methods forward path
	void b_transport		(	int id,											///< actuator socket id through which the call came in
								tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								sc_core::sc_time& delay							///< ref to time delay
							);

	bool get_direct_mem_ptr	(	int id,											///< actuator socket id through which the call came in
								tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								tlm::tlm_dmi& dmi_data							///< ref to dmi descriptor
							);

    /// Unsused interface method
	tlm::tlm_sync_enum nb_transport_fw	(	int id,								///< actuator socket id through which the call came in					
											tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay				///< ref to time delay 
										);
    
	/// Unsused interface method
	unsigned int transport_dbg	(	int id,										///< actuator socket id through which the call came in
									tlm::tlm_generic_payload& tObj 			 	///< ref to transaction object
								);

	void execute_action(void);

// Variable and Object Declarations ============================================
	tlm_utils::simple_target_socket_tagged<target_extaction, buswidth4>	extact_tsocket;		///< tagged simple  target socket for communication with ic4
	sc_core::sc_port< sc_fifo_out_if<sc_dt::uchar> >			extact_outport;				///< output port for with write access rights on fifo channel; for sendind data to other nodes

	
private:
	memory				extact_mem				;								///< memory object
	std::string 		extact_name				;								///< component name
	unsigned int		extact_id				;								///< component id

};

#endif /* __target_extaction_H__ */