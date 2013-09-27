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
#include "target_actuator.h"
#include "reporting.h"

#include <cstdio>
#include <stdlib.h>
using namespace std;

static const char *filename = "target_actuator.cpp"; ///< filename for reporting
static std::ostringstream	msg;

//======================================================================
/// @fn target_actuator
//
/// @brief constructor
//
/// @details
///		it binds the target socket to the module <br>
///		it instantiates an object of its computation core
//
//======================================================================
target_actuator::target_actuator  
								( sc_core::sc_module_name   module_name				///< SC module name
								, const unsigned int        ID						///< target ID
								, sc_dt::uint64             memory_size				///< memory size (bytes)
								, unsigned int              memory_width			///< memory width (bytes)
								, const sc_core::sc_time    read_latency			///< read response delay (SC_TIME, SC_NS)
								, const sc_core::sc_time    write_latency			///< write response delay (SC_TIME, SC_NS)
								, const sc_core::sc_time	glob_quantum_
								)	
								:
								  act_name			( name()			)
								, act_id			( ID				)
								, act_tsocket		( "actuator_tsocket")
								, act_mem
								  (	  ID 
									, read_latency									// delay for reads
									, write_latency									// delay for writes
									, memory_size									// memory size (bytes)
									, memory_width									// memory width (bytes)
								  )
{ 
	act_tsocket.bind(*this);
	std::string parname = sc_core::sc_get_parent(this)->name();
	act_core_obj = new act_core(parname);
	std::memset( act_mem.get_mem_ptr(), 0, memory_size);
}


//======================================================================
/// @fn b_transport
//
/// @brief implementation of the blocking transport for transactions
///		on the Actuator memory. 
//
/// @details It calls the funtion memory::operation() function to execute
///		the requested transaction on the actuator memory and the member function
///		execute_see_decision() of its computation core to execute the actions as decided
///		by SEE
//
//======================================================================
void target_actuator::b_transport (	tlm::tlm_generic_payload& tObj, 						///< ref to transaction object
									sc_core::sc_time& delay									///< ref to time delay
								  )
{
	act_mem.operation(act_id, tObj, delay);

	std::ostringstream  msg;
	msg.str("");

	act_core_obj->execute_see_decision(act_mem, msg);
	INFO_LOG(filename, __FUNCTION__, msg.str());

	return;
}

bool target_actuator::get_direct_mem_ptr(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
											tlm::tlm_dmi& dmi_data							///< ref to dmi descriptor
										)
{
	return false;
}

tlm::tlm_sync_enum target_actuator::nb_transport_fw (	tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
														tlm::tlm_phase& phase, 				///< ref to transaction phase
														sc_core::sc_time& delay				///< ref to time delay 
													)
{
	return tlm::TLM_ACCEPTED;
}

unsigned int target_actuator::transport_dbg (	tlm::tlm_generic_payload& tObj 			 	///< ref to transaction object
											)
{
	return 0;
}

