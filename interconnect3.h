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
///  @file interconnect3.h
//
///  @brief This is the interconnect component between the gvoc component and the 
/// 	two following components: the Monitor and the SEE. abbr.: IC3
//
///  @details
///		It forwards the method calls between GVOC and SEE, GVOC and Monitor. <br>
//
//==============================================================================

#ifndef __INTERCONNECT3_H__
#define __INTERCONNECT3_H__

#include "constants.h"
#include <systemc.h>
#include <tlm.h>

class interconnect3: public sc_module,
					 virtual public tlm::tlm_fw_transport_if<>,
					 virtual public tlm::tlm_bw_transport_if<>
{
public:

// Constructor ================================================================= 
	interconnect3	( sc_module_name name											///< sc module name
					);

// Method Declarations =========================================================
	//interface methods backward path
	void invalidate_direct_mem_ptr		(	sc_dt::uint64 start_range, 				///< start address of the memory range
											sc_dt::uint64 end_range					///< end address of the memory range
										);

	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_bw	(	tlm::tlm_generic_payload& tObj, 		///< ref to transaction object
											tlm::tlm_phase& phase, 					///< ref to transaction phase
											sc_core::sc_time& delay					///< ref to time delay
										);	
	
	//interface methods forward path
	void b_transport		(	tlm::tlm_generic_payload& tObj, 					///< ref to transaction object
								sc_core::sc_time& delay								///< ref to time delay	
							);

	bool get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 					///< ref to transaction object 
								tlm::tlm_dmi& dmi_data								///< ref to dmi descriptor
							);
							
	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_fw	(	tlm::tlm_generic_payload& tObj,  		///< ref to transaction object 
											tlm::tlm_phase& phase, 					///< ref to transaction phase 
											sc_core::sc_time& delay					///< ref to time delay
										);
									
	/// Required and unused interface method
	unsigned int transport_dbg	( 	tlm::tlm_generic_payload& tObj 					///< ref to transaction object
								);


// Variable and Object Declarations ============================================
	tlm::tlm_target_socket<buswidth3>	ic3_tsocket;								///< standard target socket for communication with gvoc
	sc_core::sc_vector< tlm::tlm_initiator_socket<buswidth3> > ic3_isocket_array;	///< vector of standard initiator sockets for communication with see and monitor

private:
	const unsigned int  ic3_target_nr;
};

#endif /* __INTERCONNECT_H__ */