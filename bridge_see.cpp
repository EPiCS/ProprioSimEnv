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
#include <string>

#include "bridge_see.h"
#include "reporting.h"

unsigned int				see_target_idx			=	0							;
unsigned int				see_actions_status		= notification::ACTION_FAILED	;
static const char			*filename				= "bridge_see.cpp_D"			;			///< filename for reporting
static const char			*suffix					= "notifcount_output.m"			;

//extern deklarierte Variable
//sc_core::sc_event see_to_monitor;

//======================================================================
/// @fn bridge_see
//
/// @brief constructor
//
/// @details
///		It declares the thread process ( see_thread() ) of this component.<br>
///		It sets the global quantum and re-calculates the local quantum of 
///		each process for temporal decoupling. It instantiates the memory objects
//
//======================================================================
bridge_see::bridge_see
						( sc_core::sc_module_name   module_name							///< SC module name
						, const unsigned int        ID									///< component ID
						, sc_dt::uint64             lm_memory_size						///< memory size for lm_data     (bytes)
						, sc_dt::uint64             gvoc_memory_size					///< memory size for gvoc data   (bytes)
						, sc_dt::uint64				report_memory_size					///< memory size for report data (bytes)
						, unsigned int				report_data_length					///< maximal length for the report data		(bytes)
						, unsigned int				actions_data_length					///< maximal length for the actions data	(bytes)
						, unsigned int              gvoc_memory_width					///< memory width (bytes)
						, unsigned int              lm_memory_width						///< memory width (bytes)
						, unsigned int              rep_memory_width					///< memory width (bytes)
						, sc_core::sc_time			read_latency						///< read response delay
						, sc_core::sc_time			gvoc_write_latency					///< write response delay for a single transaction on the gvoc memory
						, sc_core::sc_time			lm_write_latency					///< write response delay for a single transaction on the LM memory
						, unsigned int				cycle_trigger_nr_					///< number of interval between the execution of the monitor processes (in process cycles)
						, sc_core::sc_time			glob_quantum						///< time value for the globl quantum
						, unsigned int				nodenbr_							///< Position index for the vectoors of events
						):
						  see_name					( name()			)
						, see_id					( ID				)
						, see_isocket				( "see_isocket"		) 
						, see_adr					( 0					)
						, see_lm_mem_size			( lm_memory_size	)
						, see_gvoc_mem_size			( gvoc_memory_size	)
						, see_rep_mem_size			( report_memory_size)
						, see_rep_data_length		( report_data_length)
						, see_act_data_length		( actions_data_length)
						, see_idelay				( sc_core::sc_time( 0, node_time_unit)	)
						, m_cycle_nr_trigger		( cycle_trigger_nr_						)
						, see_lm_mem
							( ID					// initiator ID for messaging
							, read_latency			// delay for reads
							, lm_write_latency		// delay for writes
							, lm_memory_size		// memory size (bytes)
							, lm_memory_width		// memory width (bytes)
							),
						see_gvoc_mem
							( ID					// initiator ID for messaging
							, read_latency			// delay for reads
							, gvoc_write_latency	// delay for writes
							, gvoc_memory_size		// memory size (bytes)
							, gvoc_memory_width		// memory width (bytes) 
							),
						see_rep_mem
							( ID					// initiator ID for messaging
							, read_latency			// delay for reads
							, read_latency			// delay for writes
							, report_memory_size	// memory size (bytes)
							, rep_memory_width		// memory width (bytes) 
							)
							, nodenbr(nodenbr_)
{
	SC_THREAD(see_thread);

	see_qk.set_global_quantum(glob_quantum);
	see_qk.reset();									// Re-calculate the local quantum
	see_isocket.bind( *this );

	see_gvoc_tsocket.register_b_transport			(	this, &bridge_see::b_transport,			0	);
	see_gvoc_tsocket.register_get_direct_mem_ptr	(	this, &bridge_see::get_direct_mem_ptr,	0	);
	see_gvoc_tsocket.register_nb_transport_fw		(	this, &bridge_see::nb_transport_fw,		0	);
	see_gvoc_tsocket.register_transport_dbg			(	this, &bridge_see::transport_dbg,		0	);

	see_monitor_tsocket.register_b_transport		(	this, &bridge_see::b_transport,			1	);
	see_monitor_tsocket.register_get_direct_mem_ptr	(	this, &bridge_see::get_direct_mem_ptr,	1	);
	see_monitor_tsocket.register_nb_transport_fw	(	this, &bridge_see::nb_transport_fw,		1	);
	see_monitor_tsocket.register_transport_dbg		(	this, &bridge_see::transport_dbg,		1	);

	see_lmodel_tsocket.register_b_transport			(	this, &bridge_see::b_transport,			2	);
	see_lmodel_tsocket.register_get_direct_mem_ptr	(	this, &bridge_see::get_direct_mem_ptr,	2	);
	see_lmodel_tsocket.register_nb_transport_fw		(	this, &bridge_see::nb_transport_fw,		2	);
	see_lmodel_tsocket.register_transport_dbg		(	this, &bridge_see::transport_dbg,		2	);

	std::string parname = sc_core::sc_get_parent(this)->name();
	see_core_obj = new see_core(parname);

	memset( see_lm_mem.get_mem_ptr()	, 0, size_t(lm_memory_size)		);
	memset( see_gvoc_mem.get_mem_ptr()	, 0, size_t(gvoc_memory_size)	);
	memset( see_rep_mem.get_mem_ptr()	, 0, size_t(report_memory_size) );

	see_dec_data = new unsigned char[size_t(see_act_data_length)];
	see_notifcount_file = file_ops::build_filename( see_name, suffix);
								
}


//======================================================================
/// @fn see_thread
//
/// @brief Thread process D of the Node.
//
/// @details
///		By every execution, it evaluates the data received from the LM 
///		component with respect to the received gvoc data and decides 
///		which action(s) should be taken by the node. Then it generates a write transaction
///		to activate the corresponding actuator and pass its decision data.<br>
///		After that read transaction is completed, it notifies the monitor,  and  
///		suspends. It runs again after process C2.
//
///	@see 	bridge_see::set_trans_Obj()
/// @see 	bridge_see::execute_trans()
///	@see	trtr::follow_transactions()
//
//======================================================================
void bridge_see::see_thread(void) //D
{
	unsigned int cycles_counter	= 1;

	while (true)
	{
		wait( lmodel_to_see_vector.at(nodenbr) );

		see_idelay		=	see_qk.get_local_time();
		trtr::follow_transactions(filename, see_name, see_id,  see_idelay, trtr::T_CALL, trtr::T_IGNORE, see_id_nr);
		see_core_obj->eval_and_decide(see_lm_mem, see_gvoc_mem, see_dec_data);
		do_report();
		set_trans_Obj	( see_tObj, see_adr, see_dec_data );
		execute_trans	( see_tObj );				

		if ( ( cycles_counter == m_cycle_nr_trigger) && (m_cycle_nr_trigger > 0) )
		{
			see_to_monitor_vector.at(nodenbr).notify();	
			cycles_counter = 0;
		}

		see_qk.set( see_idelay );							// Update quantum keeper with time consumed by target
		wait( sc_core::SC_ZERO_TIME );

		if ( see_qk.need_sync() )							// Check local time against quantum and sync if neccessary
		{
			cycles_counter++;
			trtr::follow_synch(filename, see_name, "D", see_id, see_idelay, trtr::S_NEED);
			see_qk.sync();
			trtr::follow_synch(filename, see_name, "D", see_id, see_idelay, trtr::S_RETURN);
		}
	
	}
}

//======================================================================
/// @fn execute_trans
//
/// @brief performs the call of the blocking transport method b_transport for transactions between the SEE
///		and the actuators generated in the thread process D.
//
//======================================================================
void bridge_see::execute_trans( tlm::tlm_generic_payload& tObj)
{
	unsigned char* see_rep_mem_ptr = see_rep_mem.get_mem_ptr();
	see_isocket -> b_transport(tObj, see_idelay);

	if ( tObj.is_response_ok() )				
	{
		see_actions_status = notification::WRITE;
		trtr::follow_transactions(filename, see_name, see_id, see_idelay, trtr::T_RETURN, trtr::T_SUCCESS);
	} 
	else
	{
		see_actions_status = notification::ACTION_FAILED;
		trtr::follow_transactions(filename, see_name, see_id, see_idelay, trtr::T_RETURN, trtr::T_FAILURE, 0 , tObj.get_response_string().c_str() );
	}
}

//======================================================================
/// @fn set_trans_Obj
//
/// @brief uses the given parameters to build a new transaction object for
///		each transactions initiated by SEE.
//
//======================================================================
void bridge_see::set_trans_Obj	(	tlm::tlm_generic_payload& tObj,
									unsigned int adr,
									unsigned char* data
								)
{
	tObj.set_command			( tlm::TLM_WRITE_COMMAND		);
	tObj.set_address			( see_adr						);
	tObj.set_data_length		( see_act_data_length			);
	tObj.set_data_ptr			( data							);
	tObj.set_streaming_width	( see_act_data_length			);
	tObj.set_response_status	( tlm::TLM_INCOMPLETE_RESPONSE	);
	tObj.set_gp_option			( tlm::TLM_MIN_PAYLOAD			);
	tObj.set_dmi_allowed		( false							);
	tObj.set_byte_enable_ptr	( 0								);
	tObj.set_byte_enable_length ( 0								);
}

void bridge_see::b_transport	(	int id,
									tlm::tlm_generic_payload& tObj, 
									sc_core::sc_time& delay		)
{
	std::ostringstream	msg;
	msg.str("");
				
	switch(id)
	{
		case 2:  // (lmodel_id_nr):
			see_lm_mem.operation(id, tObj, delay);
			break;

		case 1: //(monitor_id_nr) 
			see_rep_mem.operation(id, tObj, delay);
			break;

		case 0: //(gvoc_id_nr):
			see_gvoc_mem.operation(id, tObj, delay);
			break;
				
		default:
			msg << "\t SEE: " << see_id
				<< "\t Invalid Initiator ID: " << id << " | delay: " << delay;
			ERROR_LOG(filename, __FUNCTION__ , msg.str());
			return;
			break;
	}
}

//======================================================================
/// @fn get_direct_mem_ptr
//
/// @brief gets a DMI pointer on a memory subarea of the SEE.
//
/// @details not yet implemented
//
//======================================================================
bool bridge_see::get_direct_mem_ptr	(	int id,
										tlm::tlm_generic_payload& tObj, 
										tlm::tlm_dmi& dmi_data	)
{
	return false;
}

tlm::tlm_sync_enum bridge_see::nb_transport_fw	(	int id,
													tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	return tlm::TLM_ACCEPTED;
}

unsigned int bridge_see::transport_dbg	(	int id,
											tlm::tlm_generic_payload& tObj )
{
	return 0;
}
	
//======================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief invalidates a DMI pointer on the actuator memory subarea which
///		is designated by the given address parameters.
//
/// @details
///		not yet implemented
//
//======================================================================
void bridge_see::invalidate_direct_mem_ptr	(	sc_dt::uint64 start_range,  //??? zu bearbeiten
												sc_dt::uint64 end_range )
{
	return;
}


tlm::tlm_sync_enum bridge_see::nb_transport_bw	(	tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay )
{
	return tlm::TLM_ACCEPTED;
}

//======================================================================
/// @fn set_target_id
//
/// @brief It uses the ID of the SEE target component to determine the 
///		correspondant index of the target components. The result is stored in the variable 
///		#see_target_idx. 
//
//======================================================================
void bridge_see::set_target_idx ( int id)
{
	see_target_idx = id;
}


//======================================================================
/// @fn do_report
//
/// @brief It write the report data in the see's report memory.
//
//======================================================================
void bridge_see::do_report(void)
{
	see_rep_mem.self_write(0, see_dec_data, see_rep_data_length);
}
