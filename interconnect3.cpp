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

#include "interconnect3.h"
#include "constants.h"

static const char *filename = "interconnect3.cpp"; ///< filename for reporting

//================================================================================
/// @fn interconnect3
//
/// @brief constructor
//
/// @details 
///		It initializes the vector of initiator sockets with the specified value of #ic3_target_nr which corresponds to o the module. 
///		<br>The number of initiator sockets is equal to the number of the GVOC or IC3 
///		Targets stored in the constant variable #ic3_target_nr.  
//
//================================================================================
interconnect3::interconnect3( sc_module_name name)
							: 
							  ic3_tsocket("ic3_tsocket")
							, ic3_isocket_array("ic3_isocket")
							, ic3_target_nr(ic3_target_number)
{
	ic3_isocket_array.init(ic3_target_nr);
	for (int i = 0; i<ic3_target_nr; i++)
	{	
		ic3_isocket_array[i].bind( *this ) ;
	}
	ic3_tsocket.bind( *this );
}

//=================================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief 
///		forwards the invalidate_direct_mem_ptr method call from the GVOC  
/// 	to the Monitor and the SEE through the appropriate initiator socket
/// 	<br>
//
//=================================================================================
void interconnect3::invalidate_direct_mem_ptr ( sc_dt::uint64 start_range, 
												sc_dt::uint64 end_range		)
{

	return ic3_tsocket->invalidate_direct_mem_ptr(start_range, end_range);
}

tlm::tlm_sync_enum interconnect3::nb_transport_bw ( tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	return ic3_tsocket->nb_transport_bw( tObj, phase, delay );
}

//==================================================================================
/// @fn b_transport
//
/// @brief forwards the b_transport method call from the GVOC to 
/// 	the Monitor and the SEE. 
//
/// @details #gvoc_target_idx indicates the appropriate initiator socket 
/// 	through which the call should be and is forwarded. It is set by 
/// 	the GVOC component.
// 
//==================================================================================
void interconnect3::b_transport (	tlm::tlm_generic_payload& tObj, 
									sc_core::sc_time& delay			  )
{
	return ic3_isocket_array[gvoc_target_idx]->b_transport( tObj, delay );
}

//==================================================================================
/// @fn get_direct_mem_ptr 
//
/// @brief forwards the get_direct_mem_ptr method call from the GVOC to 
/// 	the Monitor and the SEE through the appropriate initiator socket
//
/// @details #gvoc_target_idx indicates the appropriate initiator socket 
/// 	through which the call should be and is forwarded. It is set by 
/// 	the gvoc component.
//
//=================================================================================== 
bool interconnect3::get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 
											tlm::tlm_dmi& dmi_data		)
{
	return ic3_isocket_array[gvoc_target_idx]->get_direct_mem_ptr	(	tObj, dmi_data	); 
}


tlm::tlm_sync_enum interconnect3::nb_transport_fw ( tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	return ic3_isocket_array[gvoc_target_idx]->nb_transport_fw	( tObj, phase, delay );
}
	

unsigned int interconnect3::transport_dbg( tlm::tlm_generic_payload& tObj )
{
	return ic3_isocket_array[gvoc_target_idx]->transport_dbg	( tObj );
	
}
