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

#include "interconnect2.h"
#include "constants.h"

static const char *filename = "interconnect2.cpp"; ///< filename for reporting


//=======================================================================
/// @fn interconnect2
//
/// @brief constructor
//
/// @details 
///		It initializes the vector <i> <b> ic2_isocket_array </b> </i> (private member).
/// 	of initiator sockets and binds each of them to the module. It finally bind the target
///		socket to the module.
//
//=======================================================================
interconnect2::interconnect2( sc_module_name name)
							: 
							  ic2_tsocket("ic2_tsocket")
							, ic2_isocket_array("ic2_isocket")
							, ic2_target_nr(ic2_target_number)
{

	ic2_isocket_array.init(ic2_target_nr);
	for (int i = 0; i<ic2_target_nr; i++)
	{	
		ic2_isocket_array[i].bind( *this ) ;
	}
	ic2_tsocket.bind( *this );
}

//=======================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief 
///		forwards the invalidate_direct_mem_ptr method call from the LM and  
/// 	the SEE to the monitor through the appropriate tagged initiator
/// 	socket <br>
//
//=======================================================================
void interconnect2::invalidate_direct_mem_ptr ( sc_dt::uint64 start_range, 
												sc_dt::uint64 end_range		)
{

	return ic2_tsocket->invalidate_direct_mem_ptr(start_range, end_range);
}

tlm::tlm_sync_enum interconnect2::nb_transport_bw ( tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	return ic2_tsocket->nb_transport_bw( tObj, phase, delay );
}

//==============================================================================
/// @fn b_transport
//
/// @brief forwards the b_transport method call from the monitor to the LM  
/// and the SEE components. 
//
/// @details #m_target_idx indicates the appropriate socket through which the 
/// 	call should be and is forwarded. It is set by the monitor component with 
/// 	bridge_monitor::void set_target_id(const unsigned int &id)
//
/// @see bridge_monitor::void set_target_id(const unsigned int &id) 
//==============================================================================
void interconnect2::b_transport (	tlm::tlm_generic_payload& tObj, 
									sc_core::sc_time& delay				)
{
	return ic2_isocket_array[m_target_idx]->b_transport( tObj, delay );
}


//==============================================================================
/// @fn get_direct_mem_ptr
//
/// @brief forwards the get_direct_mem_ptr method call from the monitor to the LM  
/// and the SEE components. 
//
/// @details #m_target_idx indicates the appropriate socket through which the 
/// 	call should be and is forwarded. It is set by the monitor component with 
/// 	bridge_monitor::void set_target_id(const unsigned int &id)
//
/// @see bridge_monitor::void set_target_id(const unsigned int &id) 
//==============================================================================  
bool interconnect2::get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 
											tlm::tlm_dmi& dmi_data		)
{
	return ic2_isocket_array[m_target_idx]->get_direct_mem_ptr	(	tObj, dmi_data	); 
}


tlm::tlm_sync_enum interconnect2::nb_transport_fw ( tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	return ic2_isocket_array[m_target_idx]->nb_transport_fw	( tObj, phase, delay );
}
	

unsigned int interconnect2::transport_dbg( tlm::tlm_generic_payload& tObj )
{
	return ic2_isocket_array[m_target_idx]->transport_dbg	( tObj );
	
}

