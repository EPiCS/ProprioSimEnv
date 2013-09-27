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
#include "interconnect4.h"
#include "constants.h"

static const char *filename = "interconnect4.cpp"; ///< filename for reporting

//================================================================================
/// @fn interconnect4
//
/// @brief constructor
//
/// @details 
///     It initializes the vector <i> <b> ic4_isocket_array </b> </i> (private member) 
/// 	with the number of the iC4 Targets specified during instantiation.<br>
///		Finally the callbacks methods are registered for their use.
///		
//
//================================================================================
interconnect4::interconnect4(	sc_module_name name										///< sc module name
							,	const unsigned int isocket_number						///< number of initiator sockets
							)
							:
							  ic4_isocket_nr(isocket_number)
							, ic4_tsocket("ic4_tsocket")
{

	ic4_isocket_array.init(ic4_isocket_nr);
	for (int i = 0; i<ic4_isocket_nr; i++)
	{	
		ic4_isocket_array[i].register_invalidate_direct_mem_ptr	( this, &interconnect4::invalidate_direct_mem_ptr,	i	);
		ic4_isocket_array[i].register_nb_transport_bw			( this, &interconnect4::nb_transport_bw,			i	);
	}
	ic4_tsocket.register_b_transport		( this, &interconnect4::b_transport			);
	ic4_tsocket.register_get_direct_mem_ptr	( this, &interconnect4::get_direct_mem_ptr	);
	ic4_tsocket.register_nb_transport_fw	( this, &interconnect4::nb_transport_fw		);
	ic4_tsocket.register_transport_dbg		( this, &interconnect4::transport_dbg		);
}

//===============================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief 
///		forwards the invalidate_direct_mem_ptr method call from the SEE  
/// 	to the Actuators and to the outside of the node through the appropriate 
/// 	tagged initiator socket. <br>
//
//================================================================================
void interconnect4::invalidate_direct_mem_ptr (	int id,								///< simple tagged initiator socket receiving the call
												sc_dt::uint64 start_range, 			///< start address of the memory range
												sc_dt::uint64 end_range				///< end address of the memory range
											)
{
	//performs Adress Mapping using id??!!
	return ic4_tsocket->invalidate_direct_mem_ptr(start_range, end_range);
}

tlm::tlm_sync_enum interconnect4::nb_transport_bw (	int id,							///< ID of the simple tagged initiator socket receiving the call
													tlm::tlm_generic_payload& tObj, ///< ref to transaction object
													tlm::tlm_phase& phase, 			///< ref to transaction phase
													sc_core::sc_time& delay			///< ref to time delay	
												  )
{
	return ic4_tsocket->nb_transport_bw( tObj, phase, delay );
}

//==================================================================================
/// @fn b_transport
//
/// @brief forwards the b_transport method call from the SEE to the Actuators 
/// 	and to the outside of the node through the appropriate tagged initiator
/// 	socket of IC4.
//
/// @details #see_target_idx indicates the appropriate initiator socket through 
/// 	which the call should be and is forwarded. It is set by the SEE component.
/// 	
// 
//==================================================================================
void interconnect4::b_transport (	tlm::tlm_generic_payload& tObj, 					///< ref to transaction object
									sc_core::sc_time& delay								///< ref to time delay	
								)
{
	return ic4_isocket_array[see_target_idx]->b_transport( tObj, delay );
}

//==================================================================================
/// @fn get_direct_mem_ptr 
//
/// @brief forwards the get_direct_mem_ptr method call from the SEE to the Actuators 
/// 	and to the outside of the node through the appropriate tagged initiator
/// 	socket of IC4.
//
/// @details #see_target_idx indicates the appropriate initiator socket through 
/// 	which the call should be and is forwarded. It is set by the SEE component.
//
//=================================================================================== 
bool interconnect4::get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 			///< ref to transaction object 
											tlm::tlm_dmi& dmi_data						///< ref to dmi descriptor				
										)
{
	return ic4_isocket_array[see_target_idx]->get_direct_mem_ptr	(	tObj, dmi_data	); 
}


tlm::tlm_sync_enum interconnect4::nb_transport_fw (	tlm::tlm_generic_payload& tObj,  	///< ref to transaction object 
													tlm::tlm_phase& phase, 				///< ref to transaction phase 
													sc_core::sc_time& delay				///< ref to time delay
												  )
{
	return ic4_isocket_array[see_target_idx]->nb_transport_fw	( tObj, phase, delay );
}
	

unsigned int interconnect4::transport_dbg ( tlm::tlm_generic_payload& tObj 				///< ref to transaction object
										  )
{
	return ic4_isocket_array[see_target_idx]->transport_dbg	( tObj );
	
}
