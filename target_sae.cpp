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
#include <cstdio>
#include <stdlib.h>
#include <systemc.h>

#include "target_sae.h"
#include "reporting.h"
#include "memory.h"

using namespace std;

static const char			*filename		= "target_sae.cpp"; /// filename for reporting
static std::ostringstream	msg;
unsigned int ic1_initiator_idx				=	0;


//======================================================================
/// @fn target_sae
//
/// @brief constructor
//
/// @details
///		It initializes the vector <b> <em> sae_tsocket_array </em> </b> 
///		(private member) with the number of its initiators stored in the 
///		constant #sae_initiator_number.
///		<br> It registers the callbacks for the use of convenience sockets
///		defined here.
//
//======================================================================
target_sae::target_sae
						( sc_core::sc_module_name   module_name				///< SC module name
						, const unsigned int        ID						///< target ID
						, sc_dt::uint64             memory_size				///< memory size (bytes)
						, unsigned int              memory_width			///< memory width (bytes)
						, const sc_core::sc_time    read_latency			///< read response delay 
						, const sc_core::sc_time    write_latency			///< write response delay
						)	
						:	
						  sae_id					( ID			)
						, sae_mem_size				( memory_size	)
						, sae_read_latency			( read_latency	)
						, sae_write_latency			( write_latency	)
						, sae_mem_width				( memory_width	)
						, sae_initiator_nr			( sae_initiator_number	)
						, sae_mem 
						(	  ID 
							, read_latency									// delay for reads
							, write_latency									// delay for writes
							, memory_size									// memory size (bytes)
							, memory_width									// memory width (bytes)
						)
						, sae_tsocket_array("TSOCKET")
{ 
	sae_tsocket_array.init(sae_initiator_number);
	for (unsigned int i = 0; i < sae_initiator_number; i++)
    {
		sae_tsocket_array[i].register_b_transport			( this, &target_sae::b_transport,			i	);
		sae_tsocket_array[i].register_get_direct_mem_ptr	( this, &target_sae::get_direct_mem_ptr,	i	);
		sae_tsocket_array[i].register_nb_transport_fw		( this, &target_sae::nb_transport_fw,		i	);
		sae_tsocket_array[i].register_transport_dbg			( this, &target_sae::transport_dbg,			i	);
    }
}


//======================================================================
/// @fn b_transport
//
/// @brief implementation of the blocking transport for transactions on
///		the SAE memory
//
/// @details IC1 initiators only have write access und  LM only has 
///		read access. This funtion first verifies that the requested 
///		memory access is allowed before executing the operation. <br>
///		
//
//======================================================================
void target_sae::b_transport (	int id,
								tlm::tlm_generic_payload& tObj, 
								sc_core::sc_time& delay				)
{	
	tlm::tlm_command	cmd			= tObj.get_command();

	// control access rights 
	switch(id)
	{
		case 1:					//lmodel
			sae_mem.operation(lmodel_id_nr, tObj, delay);
			break;
		case 0:					//ic1
			if (cmd == tlm::TLM_READ_COMMAND )
			{
				tObj.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
				msg << "this component has write-access only!";
				ERROR_LOG(filename, __FUNCTION__ , msg.str());
				return;
			}
			sae_mem.operation(ic1_id_nr, tObj, delay);
			break;
	}
	
}


//======================================================================
/// @fn get_direct_mem_ptr
//
/// @brief provides dmi pointer for direct access on the SAE memory
//
/// @details not yet implemented
//
//======================================================================
bool target_sae::get_direct_mem_ptr( int id,
									 tlm::tlm_generic_payload& tObj, 
									 tlm::tlm_dmi& dmi_data			)
{
	return false;
}

tlm::tlm_sync_enum target_sae::nb_transport_fw( int id,
												tlm::tlm_generic_payload& tObj, tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
	return tlm::TLM_ACCEPTED;
}

unsigned int target_sae::transport_dbg( int id,
										tlm::tlm_generic_payload& tObj )
{
	return 0;
}


void set_target_idx_for_invalidate_ptr	(	const unsigned int& start_addr
										 ,	const unsigned int& end_addr	
										)
{
	ic1_initiator_idx = 0; 
}

unsigned int target_sae::get_id(void)
{
	return sae_id;
}