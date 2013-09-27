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

#include "initiator_gvoc.h"
#include "constants.h"
#include "reporting.h"
#include "key_functions.h"

unsigned int		gvoc_target_idx =	0	;								///< extern variable - ref constants.h
static const char	*filename		=	"initiator_gvoc.cpp";				///< filename for reporting

//================================================================================
/// @fn initiator_gvoc
//
/// @brief constructor
//
/// @details 
///		declares the thread process ( gvoc_thread() ) of this component.
///		<br>binds the initiator socket to the module. <br>
///		sets the global quantum and re-calculates the local quantum for
///		temporal decoupling.	  
//
//================================================================================
initiator_gvoc::initiator_gvoc	( sc_module_name		module_name									///< sc module name
								  , const unsigned int	ID											///< component ID
								  , const unsigned int	see_data_length								///< maximal data length for see data
								  , const unsigned int	monitor_data_length							///< maximal data length for monitor data
								  , const unsigned int	see_nr_dataset_cycle						///< number of gvoc datasets pro cycle for the see component
								  , const unsigned int	monitor_nr_dataset_cycle					///< number of gvoc datasets pro cycle for the monitor component
								  , sc_core::sc_time	glob_quantum								///< time value for the global quantum	
								): 
								  gvoc_name					(	name()						)
								, gvoc_id					(	ID							)
								, gvoc_isocket				(	"gvoc_isocket"				)
								, gvoc_see_data_length		(	see_data_length				)
								, gvoc_monitor_data_length	(	monitor_data_length			)
								, gvoc_see_dataset_cycle	(	see_nr_dataset_cycle		)
								, gvoc_m_dataset_cycle		(	monitor_nr_dataset_cycle	)
								, gvoc_m_adr_start			(	0							)
								, gvoc_see_adr_start		(	0							)
								, gvoc_see_delay			( sc_core::sc_time(0, node_time_unit) )
								, gvoc_m_delay				( sc_core::sc_time(0, node_time_unit) )
{
	SC_THREAD( gvoc_see_thread );
	SC_THREAD( gvoc_monitor_thread );

	gvoc_isocket.bind( *this );

	gvoc_see_qk.set_global_quantum ( glob_quantum );	
	gvoc_see_qk.reset();								

	gvoc_m_qk.set_global_quantum ( glob_quantum );	
	gvoc_m_qk.reset();	
	
	gvoc_data = new unsigned char[size_t(gvoc_see_data_length)];
	memset(gvoc_data, 0, size_t(gvoc_see_data_length));
}

//======================================================================
/// @fn gvoc_thread
//
/// @brief Thread process A1 of the process cycle of the Node.
//
/// @details  It sets new transactions objects with the goals, values, objectives and constraints data 
///		for the Monitor component and explicitely indicates the target before each call to ensure that 
///		IC3 forwards it to the appropriate target.
//
/// @see	initiator_gvoc::prepare_datablock()
/// @see	initiator_gvoc::set_target_idx()
///	@see	initiator_gvoc::set_trans_Obj()
/// @see 	initiator_gvoc::execute_trans()
//======================================================================
void initiator_gvoc::gvoc_monitor_thread(void)   //Process A1
{
	unsigned int j = 0;
	std::ostringstream	msg;  

	wait( SC_ZERO_TIME );
	while(true)
	{
		msg.str ("");
		msg << "\t PROZESS A1";
		INFO_LOG(filename, __FUNCTION__ , msg.str());

		gvoc_m_delay		= gvoc_m_qk.get_local_time();
		prepare_datablock();
				
		set_target_idx		( monitor_id_nr);
		set_trans_Obj		( gvoc_tObj, gvoc_m_adr_start, gvoc_data);
		execute_trans		( gvoc_tObj, monitor_id_nr, gvoc_m_delay  );
		if(gvoc_m_dataset_cycle>1)
		{
			gvoc_m_adr_start	+= gvoc_monitor_data_length;
		}

		gvoc_m_qk.set( gvoc_m_delay );							// Update quantum keeper with time consumed by target
		if ( gvoc_m_qk.need_sync() ) 
		{
			gvoc_m_adr_start	= 0; j= 0;
			trtr::follow_synch	(filename, gvoc_name, "A1", gvoc_id, gvoc_m_delay,trtr::S_NEED);
			gvoc_m_qk.sync		();								// Check local time against quantum and sync if neccessary
			trtr::follow_synch	(filename, gvoc_name, "A1", gvoc_id, gvoc_m_delay,trtr::S_RETURN);
		}
	}
}


//======================================================================
/// @fn gvoc_thread
//
/// @brief Thread process A2 of the process cycle of the Node.
//
/// @details  It sets new transactions objects with the goals, values, objectives and constraints data 
///		for the SEE component and explicitely indicates the target before each call to ensure that 
///		IC3 forwards the call to the appropriate target.
//
/// @see	initiator_gvoc::prepare_datablock()
/// @see	initiator_gvoc::set_target_idx()
///	@see	initiator_gvoc::set_trans_Obj()
/// @see 	initiator_gvoc::execute_trans()
//======================================================================
void initiator_gvoc::gvoc_see_thread(void)   //Process A2
{
	//schickt values-constraints an Monitor
	unsigned int j = 0;
	std::ostringstream	msg;
		
	wait( 0.1, node_time_unit );
	while(true)
	{
		msg.str ("");
		msg << "\t PROZESS A2";
		INFO_LOG(filename, __FUNCTION__ , msg.str());

		prepare_datablock();

		gvoc_see_delay				=	gvoc_see_qk.get_local_time();

		set_target_idx		( see_id_nr );
		set_trans_Obj		( gvoc_tObj, gvoc_see_adr_start, gvoc_data );
		execute_trans		( gvoc_tObj, see_id_nr, gvoc_see_delay );

		if(gvoc_see_dataset_cycle>1)
		{
			gvoc_see_adr_start	+= gvoc_see_data_length;
		}

		gvoc_see_qk.set( gvoc_see_delay );						// Update quantum keeper with time consumed by target

		if ( gvoc_see_qk.need_sync() ) 
		{
			gvoc_see_adr_start	= 0; j = 0;
			trtr::follow_synch	(filename, gvoc_name, "A2", gvoc_id, gvoc_see_delay,trtr::S_NEED);

			gvoc_see_qk.sync	();								// Check local time against quantum and sync if neccessary
			trtr::follow_synch	(filename, gvoc_name, "A2", gvoc_id, gvoc_see_delay,trtr::S_RETURN);			
		}
	}
}


//======================================================================
/// @fn execute_trans
//
/// @brief performs b_tranport call for each generated transaction in 
///		the thread process of the component.
//
/// @details
///		The funtion trtr::follow_transactions() is called before and 
///		after every b_transport method call and every process synchro
///		nization to report or monitor them (in the output file).
//
///	@see	trtr::follow_transactions()
//
//======================================================================
void initiator_gvoc::execute_trans	( tlm::tlm_generic_payload& tObj, 
									  const unsigned int &target_id,
									  sc_core::sc_time &delay 
									)
{
	trtr::follow_transactions(filename, gvoc_name, gvoc_id, delay, trtr::T_CALL, trtr::T_IGNORE, target_id);

	gvoc_isocket->b_transport(tObj, delay);

	if ( tObj.is_response_ok() )
	{
		trtr::follow_transactions(filename, gvoc_name, gvoc_id, delay, trtr::T_RETURN, trtr::T_SUCCESS, target_id);
	}
	else //if (tObj.is_response_error())
	{
		trtr::follow_transactions(filename, gvoc_name, gvoc_id, delay, trtr::T_RETURN, trtr::T_FAILURE, target_id , tObj.get_response_string().c_str() );
	}
}


//======================================================================
/// @fn set_target_idx
//
/// @brief modifies the value of the variable #gvoc_target_idx to indicate
/// 	the appropriate target component to the IC3 component.
//
/// @details
///		The index is used by the IC3 component to choose the appropriate
/// 	initiator socket for the forwarding of method calls
//
//======================================================================
void initiator_gvoc::set_target_idx( const unsigned int &id ) 
{
	//according to the socket bindings in lt_pnode_top
	if (id == see_id_nr)
		gvoc_target_idx = 1;		
	else if (id == monitor_id_nr)
		gvoc_target_idx = 0;		
}

//======================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief invalidates a DMI pointer on the Monitor or SEE memory subarea, 
///		which is designated by the given address parameters
//
/// @details
///		not yet implemented
//
//======================================================================
void initiator_gvoc::invalidate_direct_mem_ptr(	sc_dt::uint64 start_range,   //***here***
												sc_dt::uint64 end_range		)
{
	return;
}

tlm::tlm_sync_enum initiator_gvoc::nb_transport_bw( tlm::tlm_generic_payload& tObj, 
										tlm::tlm_phase& phase, 
										sc_core::sc_time& delay )
{ 
	return tlm::TLM_COMPLETED; 
}


//======================================================================
/// @fn set_trans_Obj
//
/// @brief uses the given parameters to set the parameters of the given transaction object
///		for the next transaction 
//
//======================================================================
void initiator_gvoc::set_trans_Obj(	tlm::tlm_generic_payload& tObj,
									const sc_dt::uint64 &adr,
									unsigned char* data)
{
	unsigned int data_length;
			
	tObj.set_command			(	tlm::TLM_WRITE_COMMAND	);
	tObj.set_address			(	adr						);
	tObj.set_data_ptr			(	data					);
	tObj.set_response_status	(	tlm::TLM_INCOMPLETE_RESPONSE);
	tObj.set_gp_option			(	tlm::TLM_MIN_PAYLOAD	); 
	tObj.set_dmi_allowed		(	false					);
	tObj.set_byte_enable_ptr	(	0						);
	tObj.set_byte_enable_length (	0						);

	if (gvoc_target_idx == 0)
	{
		tObj.set_data_length	(	gvoc_monitor_data_length	);
		tObj.set_streaming_width(	gvoc_monitor_data_length	);
	}
	else if (gvoc_target_idx == 1) 
	{
		tObj.set_data_length	(	gvoc_see_data_length		);
		tObj.set_streaming_width(	gvoc_see_data_length		);
	}

}



//======================================================================
/// @fn prepare_datablock
//
/// @brief converts to the data for the next transaction in string and store it in
///		the memory block where #gvoc_data points to. 
//
///	@details This is just an example. So the user should and can changed it according
///		its needs
//
//======================================================================
void initiator_gvoc::prepare_datablock(void)
{
	datyp gvoc_databuf =	1;
	std::string tmp = data_conversion::toString(gvoc_databuf);
	std::memcpy( gvoc_data, tmp.data(), size_t(str_size) );
}