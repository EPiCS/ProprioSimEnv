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
///  @file interconnect4.h
//
///  @brief This is the interconnect component between the SEE component and both 
///  	the  actuator and the extaction component. 
///		abbr.: IC4
//
///  @details
///		It forwards the methods calls from the SEE to the components actuators and 
///		extaction.
//
//==============================================================================

#ifndef __INTERCONNECT4_H__
#define __INTERCONNECT4_H__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils\simple_target_socket.h>
#include <tlm_utils\simple_initiator_socket.h>	// TLM headers

#include "constants.h"

class interconnect4: public sc_module
{
public:
// Constructor ================================================================= 
	interconnect4	(	sc_module_name name										///< sc module name
					,	const unsigned int isocket_number						///< number of initiator sockets
					);

// Method Declarations =========================================================

	//interface methods backward path
	void invalidate_direct_mem_ptr		(	int id,								///< simple tagged initiator socket receiving the call
											sc_dt::uint64 start_range, 			///< start address of the memory range
											sc_dt::uint64 end_range				///< end address of the memory range
										);

	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_bw	(	int id,								///< ID of the simple tagged initiator socket receiving the call
											tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay				///< ref to time delay	
										);
	
	//interface methods forward path
	void b_transport		(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								sc_core::sc_time& delay							///< ref to time delay	
							);

	bool get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object 
								tlm::tlm_dmi& dmi_data							///< ref to dmi descriptor				
							);

	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_fw	(	tlm::tlm_generic_payload& tObj,  	///< ref to transaction object 
											tlm::tlm_phase& phase, 				///< ref to transaction phase 
											sc_core::sc_time& delay				///< ref to time delay
										);
										
    /// Required and unused interface method
	unsigned int transport_dbg	( 	tlm::tlm_generic_payload& tObj 				///< ref to transaction object
								);

// Variable and Object Declarations ============================================
	tlm_utils::simple_target_socket<interconnect4, buswidth4>				ic4_tsocket;						///< simple target socket for communication with see
	sc_core::sc_vector<tlm_utils::simple_initiator_socket_tagged<interconnect4, buswidth4>> ic4_isocket_array; ///< vector of standard initiator sockets for communication with actuator and extaction
	
private:
	const unsigned int			ic4_isocket_nr;									///< number of IC4 target components
	
};

#endif /* __INTERCONNECT4_H__ */