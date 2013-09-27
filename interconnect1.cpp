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
#include "interconnect1.h"
#include "constants.h"

static const char *filename = "interconnect1.cpp"; ///< filename for reporting


//=======================================================================
/// @fn interconnect1
//
/// @brief constructor
//
/// @details It initializes the vector of target sockets with the specified number of
///		target sockets which is equals to the number of its initiators (SenEnv and OtherNode).
///		<br> It binds each of them to the module and registers the callbacks for their use.
//
//
//=======================================================================

interconnect1::interconnect1 ( sc_module_name name 																	///< sc module name
							, const unsigned int tsocket_number														///< number of target sockets
							, const unsigned int sae_memory_size_													///< memory size of the sae component 
							): 
							  ic1_isocket		("isocket")															///< initiator socket name
							, ic1_tsocket_nr	(tsocket_number)													///< number of initiators
							, sae_memory_size	(sae_memory_size_)
{
	ic1_isocket.register_invalidate_direct_mem_ptr		( this, &interconnect1::invalidate_direct_mem_ptr	);	
	ic1_isocket.register_nb_transport_bw				( this, &interconnect1::nb_transport_bw				);
	
	ic1_tsocket_array.init(ic1_tsocket_nr);			//+1 später hinzufügen, für den socket für daten von außen
	for (int i=0; i<ic1_tsocket_nr; i++)
	{
		ic1_tsocket_array[i].register_b_transport		( this, &interconnect1::b_transport			, i);
		ic1_tsocket_array[i].register_get_direct_mem_ptr( this, &interconnect1::get_direct_mem_ptr	, i);
		ic1_tsocket_array[i].register_nb_transport_fw	( this, &interconnect1::nb_transport_fw		, i);
		ic1_tsocket_array[i].register_transport_dbg		( this, &interconnect1::transport_dbg		, i);
	}
}

//=======================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief forwards the invalidate_direct_mem_ptr method call from the sae  
/// 	through the appropriate tagged initiator socket to the concerned
/// 	initiator.
//
/// @details #ic1_initiator_idx indicates the appropriate socket and is set by
/// 	the sae component with target_sae::set_target_idx_for_invalidate_ptr(const int& id) 
//
/// @see target_sae::set_target_idx_for_invalidate_ptr(const int& id)
/// @see get_direct_mem_ptr
//
//=======================================================================

void interconnect1::invalidate_direct_mem_ptr (	sc_dt::uint64 start_range, 
												sc_dt::uint64 end_range		)
{
	ic1_tsocket_array[ic1_initiator_idx]->invalidate_direct_mem_ptr(start_range, end_range);
}


tlm::tlm_sync_enum interconnect1::nb_transport_bw ( tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	address_mapping(ic1_initiator_idx, tObj, false);
	return ic1_tsocket_array[ic1_initiator_idx]->nb_transport_bw( tObj, phase, delay );
}


//=======================================================================
/// @fn b_transport
//
/// @brief forwards the b_transport method call from the sae initiators to  
/// sae component after the address mapping
//
/// @see address_mapping(int id,tlm::tlm_generic_payload& tObj, bool direction)
//
//=======================================================================

void interconnect1::b_transport (	int id,
									tlm::tlm_generic_payload& tObj, 
									sc_core::sc_time& delay				)
{
	std::ostringstream   msg;
	msg.str ("");
	address_mapping(id, tObj, true);
	ic1_isocket->b_transport(tObj, delay);
}
 

//=======================================================================
/// @fn get_direct_mem_ptr 
//
/// @brief forwards the get_direct_mem_ptr method call from the sae initiators   
/// to sae component after the address mapping
//
/// @see address_mapping(int id,tlm::tlm_generic_payload& tObj, bool direction)
//
//=======================================================================

bool interconnect1::get_direct_mem_ptr ( int id,
										 tlm::tlm_generic_payload& tObj, 
										 tlm::tlm_dmi& dmi_data			)
{
	address_mapping(id, tObj, true);
	return ic1_isocket->get_direct_mem_ptr(tObj, dmi_data);
}


tlm::tlm_sync_enum interconnect1::nb_transport_fw ( int id,
													tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	address_mapping(id, tObj, true);
	return ic1_isocket->nb_transport_fw( tObj, phase, delay );
}
	

unsigned int interconnect1::transport_dbg(	int id,
											tlm::tlm_generic_payload& tObj )
{
	address_mapping(id, tObj, true);
	return ic1_isocket->transport_dbg( tObj );	
}


//=======================================================================
/// @fn address_mapping
//
/// @brief maps the given transaction address by method calls on the 
/// 	for- und backward paths between the sae components and his initiators 
/// 	(except from LModel)
//
///	@details
/// 	The sae memory area is evenly distributed between his initiators  
/// 	(except from LModel) and each subarea has an index which is the same
///		as the id of the IC1 initiator and the id of the IC1 tagged initiator 
///		socket through which a call is received. <br>
/// 	This function uses the respective index to calculate the 
/// 	corresponding memory address for each method call <br><br>
/// 	e.g.: sae initiators: 3 sensors, sae memory length: 12 Bytes, call on 
/// 	the forward path.
/// 	<table> 
/// 	<tr> <th>sensor idx </th> <th> ic1 tsocket idx</th> <th>sensor adr</th>  <th>sae adr</th> </tr>
/// 	<tr> <td>300</td> <td>0</td> <td>3</td>  <td>3</td> </tr>
/// 	<tr> <td>301</td> <td>1</td> <td>3</td>  <td>6</td> </tr>
/// 	<tr> <td>302</td> <td>2</td> <td>3</td>  <td>10</td> </tr>
/// 	</table>
//
/// @see initiator_sensorenv::set_initiator_idx(const unsigned int &id)
//
//=======================================================================

void interconnect1::address_mapping(	int idx,
										tlm::tlm_generic_payload& tObj,
										bool direction
									)
{
	unsigned int adr		= tObj.get_address();
	unsigned int mapped_adr;

	switch(direction)
	{
	case(true):			//mapping
		mapped_adr	=	( idx  * ( sae_memory_size/ic1_tsocket_nr ) ) + adr;
		break;
	case(false):		//reverse mapping
		mapped_adr	=	 adr - ( idx * (sae_memory_size/ic1_tsocket_nr) );  //ic1_tsocket_nr = IC1_INITIATORS
		break;
	}
	tObj.set_address(mapped_adr);
}