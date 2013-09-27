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

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "target_extaction.h"
#include "reporting.h"

#include <cstdio>
#include <stdlib.h>
using namespace std;

static const char *filename = "target_extaction.cpp"; ///< filename for reporting
static std::ostringstream	msg;

//================================================================================
/// @fn target_extaction
//
/// @brief constructor
//
/// @details
///		It registers the callback for the use of convenience socket.
//
//======================================================================
target_extaction::target_extaction  
								( sc_core::sc_module_name   module_name				///< SC module name
								, const unsigned int        ID						///< target ID
								, sc_dt::uint64             memory_size				///< memory size (bytes)
								, unsigned int              memory_width			///< memory width (bytes)
								, const sc_core::sc_time    read_latency			///< read response delay (SC_TIME, SC_NS)
								, const sc_core::sc_time    write_latency			///< write response delay (SC_TIME, SC_NS)
								)	
								:
								  extact_name			( name()			)
								, extact_id				( ID				)
								, extact_tsocket		( "actuator_tsocket")
								, extact_outport		( "output_port"		)
								, extact_mem
								  (	  ID 
									, read_latency									// delay for reads
									, write_latency									// delay for writes
									, memory_size									// memory size (bytes)
									, memory_width									// memory width (bytes)
								  )
{ 
	extact_tsocket.register_b_transport			( this, &target_extaction::b_transport,			extact_id	);
	extact_tsocket.register_get_direct_mem_ptr	( this, &target_extaction::get_direct_mem_ptr,	extact_id	);
	extact_tsocket.register_nb_transport_fw		( this, &target_extaction::nb_transport_fw,		extact_id	);
	extact_tsocket.register_transport_dbg		( this, &target_extaction::transport_dbg,		extact_id	);
}

//======================================================================
/// @fn b_transport
//
/// @brief implementation of the blocking transport for transactions
///		on the ExtAction's memory. 
//
/// @details It calls the funtion memory::operation() function to execute
///		the requested transaction on the extaction's memory and write data
///		on the bounded fifo channel through the ouptut port.
//
//======================================================================
void target_extaction::b_transport  (	int id,												///< actuator socket id through which the call arrived
										tlm::tlm_generic_payload& tObj, 					///< ref to transaction object
										sc_core::sc_time& delay								///< ref to time delay
									)
{
	extact_mem.operation(id, tObj, delay);
	execute_action();
	return;
}

bool target_extaction::get_direct_mem_ptr(	int id,											///< actuator socket id through which the call arrived
											tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
											tlm::tlm_dmi& dmi_data							///< ref to dmi descriptor
										)
{
	return false;
}

tlm::tlm_sync_enum target_extaction::nb_transport_fw(	int id,								///< actuator socket id through which the call arrived					
														tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
														tlm::tlm_phase& phase, 				///< ref to transaction phase
														sc_core::sc_time& delay				///< ref to time delay 
													)
{
	return tlm::TLM_ACCEPTED;
}

unsigned int target_extaction::transport_dbg(	int id,										///< actuator socket id through which the call arrived
												tlm::tlm_generic_payload& tObj				///< ref to transaction object
											)
{
	return 0;
}


//======================================================================
/// @fn execute_action
//
/// @brief evaluates the see data and executes the corresponding action.
//
/// @details to be implemented according to the system under investigation
//
//======================================================================
void target_extaction::execute_action(void)
{
	/*sc_dt::uchar out = 'E'		;
	extact_outport->write(out)	;*/
}