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

#include "bridge_lmodel.h"
#include <systemc.h>
#include <tlm.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "constants.h"
#include "reporting.h"
#include "memory.h"
#include "reporting.h"

using namespace std;

static const char				*filename				= "bridge_lmodel.cpp";					///filename for synch reports
static const char				*filename1				= "bridge_lmodel.cpp_C1";				///filename for transaction reports of process C1
static const char				*filename2				= "bridge_lmodel.cpp_C2";				///filename for transaction reports of process C2
static const char				*suffix					= "notifcount_output.m";

notification::report_actions	lm_actions_status		;

//======================================================================
/// @fn initiator_lmodel
//
/// @brief constructor
//
/// @details
///		It declares the thread processes ( lmodel_sae_thread(), lmodel_see_thread() ) 
///		of this component.<br>
///		It binds the initiator and targets sockets to the module. <br>
///		It sets the global quantum and re-calculates the local quantum of 
///		each process for temporal decoupling. <br>
///		It instantiates the blocks of memory, which will be used for the sae data and 
///		and the result data of LM, and fills them with zeros <br>
///		Finally it instantiates the computation core of LM.
//
//======================================================================
bridge_lmodel::bridge_lmodel
							( 
							  sc_module_name module_name								///< sc module name
							, const unsigned int ID										///< component ID
							, sc_dt::uint64             mem_size						///< memory size (bytes) = max length of Report Data
		    				, unsigned int              mem_width						///< memory width (bytes)
							, unsigned int 				result_data_length				///< maximal length for result data
							, unsigned int 				report_data_length				///< maximal length for report data
							, unsigned int				sae_data_length					///< maximal length for sensor data
							, sc_core::sc_time			read_latency					///< read response delay
							, sc_core::sc_time			write_latency					///< write response delay
							, sc_core::sc_time			see_write_latency				///< write response delay on the see memory for a single transaction
							, sc_core::sc_time			sae_read_latency				///< read response delay on the sae memory for a single transaction
							, unsigned int				cycle_trigger_nr_				///< number of interval between the execution of the monitor processes (in process cycles)
							, sc_core::sc_time			glob_quantum					///< time value for the globl quantum
							, unsigned int				nodenbr_						///< Position index for the vectoors of events
							)
							: 
							  lm_id					(	ID					)
							, lm_name				(	name()				)
							, lm_see_isocket		(	"lm_see_isocket"	) 
							, lm_sae_isocket		(	"lm_sae_isocket"	)
							, see_adr_start			(	0					)
							, sae_adr_start			(	0					)
							, lm_res_dl				(   result_data_length	)			// length for result data
							, lm_rep_dl				(   report_data_length	)			// length for result data
							, lm_sae_dl				(	sae_data_length		)
							, lm_sae_delay			(	sc_core::sc_time( 0.4, node_time_unit ) )
							, lm_see_delay			(	sc_core::sc_time( 0.4, node_time_unit ) )
							, m_cycle_nr_trigger	(	cycle_trigger_nr_	)
							, lm_repmem													///< memory object for report data
								(
								  ID
								, read_latency
								, write_latency
								, mem_size
								, mem_width
								)
								, nodenbr(nodenbr_)
{
	lm_sae_data = new unsigned char[size_t(lm_sae_dl)];
	memset( lm_sae_data, 0, size_t(lm_sae_dl) );

	lm_res_data = new unsigned char[size_t(lm_res_dl)];
	memset( lm_res_data, 0, size_t(lm_res_dl) );

	SC_THREAD(lmodel_sae_thread);
	SC_THREAD(lmodel_see_thread);

	lm_qk1.set_global_quantum(glob_quantum);
	lm_qk2.set_global_quantum(glob_quantum);

	lm_qk1.reset();										
	lm_qk2.reset();										

	lm_see_isocket.bind	( *this );	
	lm_sae_isocket.bind	( *this );
	lm_m_tsocket.bind	( *this );

	std::string parname = sc_core::sc_get_parent(this)->name();
	lm_core_obj = new lm_core(parname);
	lm_notifcount_file = file_ops::build_filename(lm_name, suffix);


};


//======================================================================
/// @fn lmodel_sae_thread
//
/// @brief Thread process C1 of the Node.
//
/// @details
///		By each execution, it generates a read transaction on the SAE Memory, evaluates the data. The same transaction object is used
///		for all transactions, but its attributes are set before each one of them. <br>
///		By every execution, it also increments the variable #cycles_counter and compares the updated variable with m_cycle_nr_trigger. If both variables 
///		have the same value, then  counter cycles_counter  is reset and the event lmodel_to_monitor_ev belonging to the dynamic sensitivity of process E1 is notified. 
///		Otherwise event monitor_ev, which rahther belongs to the dynamic sensitiviy of process C2, is notified. Finally it suspends and is resumed in the next delta cycle.
//
/// @see	initiator_sensorenv::do_report()
///	@see 	initiator_sensorenv::set_trans_Obj()
/// @see 	initiator_sensorenv::execute_trans(tlm::tlm_generic_payload& tObj)
///	@see	trtr::follow_transactions()
//
//======================================================================
void bridge_lmodel::lmodel_sae_thread()						//C1
{
	unsigned int		burst_length	= ( (unsigned int)(ceil((double)lm_sae_dl/buswidth1)));
	unsigned int		cycles_counter	= 1;

	wait(lm_sae_delay);
	while(true)
	{
		lm_sae_delay = lm_qk1.get_local_time();
		set_target_id( sae_id_nr );

		set_trans_Obj	( lm_sae_tObj, tlm::TLM_READ_COMMAND, lm_target_id, lm_sae_data, sae_adr_start );
		execute_trans	( lm_sae_tObj, lm_target_id );
		lm_core_obj->evaluate_data(lm_sae_data, lm_actions_status);
		do_report();

		sae_adr_start += lm_sae_dl;

		if ( ( cycles_counter == m_cycle_nr_trigger) && (m_cycle_nr_trigger > 0) )
		{
			lmodel_to_monitor_vector.at(nodenbr).notify();
			cycles_counter = 0;

		}
		else
		{
			monitor_ev_vector.at(nodenbr).notify();
		}

		lm_qk1.set( lm_sae_delay );							// Update quantum keeper with time consumed by target
		wait(sc_core::SC_ZERO_TIME); 

		if ( lm_qk1.need_sync() )
		{
			cycles_counter++;

			trtr::follow_synch(filename, lm_name, "C1",  lm_id, lm_sae_delay,trtr::S_NEED);
			lm_qk1.sync();									// Check local time against quantum and sync if neccessary
			trtr::follow_synch(filename, lm_name, "C1", lm_id, lm_sae_delay,trtr::S_RETURN);

			sae_adr_start	=	0;
		}
		wait(SC_ZERO_TIME);
	}
};

//======================================================================
/// @fn lmodel_see_thread
//
/// @brief Thread process C2 of the Node.
//
/// @details
///		By every execution, it sets the attributes of the transaction object and 
///		generate write transactions on the see's memory space specially intended for him. <br> 
///		It suspends after every performed transaction until its event #monitor_ev is notified.
//
///	@see 	initiator_sensorenv::set_trans_Obj()
/// @see 	initiator_sensorenv::execute_trans()
///	@see	trtr::follow_transactions()
//
//======================================================================
void bridge_lmodel::lmodel_see_thread()		//C2
{
	sc_core::sc_time	wait_time			= sc_core::SC_ZERO_TIME;
	unsigned int		burst_length		= ( (unsigned int)(ceil((double)lm_res_dl/buswidth5)));
	lm_res_data		=	new unsigned char[size_t(lm_sae_dl)];

	while ( true )
	{
		wait( monitor_ev_vector.at(nodenbr) );

		lm_see_delay	=	lm_qk2.get_local_time();

		set_target_id( see_id_nr );

		if (lm_actions_status == 33)
			lm_core_obj->send_data(lm_res_data);		//get difference result as char array
		else
			memset( lm_res_data, 0, size_t(lm_res_dl) );
		
		set_trans_Obj	( lm_see_tObj, tlm::TLM_WRITE_COMMAND, lm_target_id, lm_res_data, see_adr_start);
		execute_trans	( lm_see_tObj, lm_target_id );

		lmodel_to_see_vector.at(nodenbr).notify();

		lm_qk2.set( lm_see_delay );						// Update quantum keeper with time consumed by target
		wait(wait_time);

		if ( lm_qk2.need_sync() )
		{
			trtr::follow_synch(filename, lm_name, "C2", lm_id, lm_see_delay,trtr::S_NEED);
			lm_qk2.sync();								// Check local time against quantum and sync if neccessary
			trtr::follow_synch(filename, lm_name, "C2", lm_id, lm_see_delay,trtr::S_RETURN);
		} 
	}
}

//======================================================================
/// @fn execute_trans
//
/// @brief performs the call of the blocking transport method for each generated transaction in 
///		the thread processes of this component. 
//
///	@see	trtr::follow_transactions()
//
//======================================================================
void bridge_lmodel::execute_trans(	tlm::tlm_generic_payload& tObj, 
									const unsigned int &target_id  )
{
	
	switch(target_id)
	{
		case(sae_id_nr):

			trtr::follow_transactions(filename1, lm_name, lm_id, lm_sae_delay, trtr::T_CALL, trtr::T_IGNORE, target_id);
			lm_sae_isocket -> b_transport(tObj, lm_sae_delay);

			if (tObj.is_response_ok())
			{
				lm_actions_status = notification::READ;
				trtr::follow_transactions(filename1, lm_name, lm_id, lm_sae_delay, trtr::T_RETURN, trtr::T_SUCCESS, target_id);
			}
			else //if (tObj.is_response_error())
			{
				lm_actions_status = notification::ACTION_FAILED;
				trtr::follow_transactions(filename1, lm_name, lm_id, lm_sae_delay, trtr::T_RETURN, trtr::T_FAILURE, 0 , tObj.get_response_string().c_str() );
			}
			break;

		case(see_id_nr):

			trtr::follow_transactions(filename2, lm_name, lm_id, lm_see_delay, trtr::T_CALL, trtr::T_IGNORE, target_id);
			lm_see_isocket->b_transport(tObj, lm_see_delay); // send critical data to the see.

			if (tObj.is_response_ok())
			{
				trtr::follow_transactions(filename2, lm_name, lm_id, lm_see_delay, trtr::T_RETURN, trtr::T_SUCCESS, target_id);
			}
			else //if (tObj.is_response_error())
			{
				trtr::follow_transactions(filename2, lm_name, lm_id, lm_see_delay, trtr::T_RETURN, trtr::T_FAILURE, 0 , tObj.get_response_string().c_str() );
			}
			break;
	}
}


//======================================================================
/// @fn set_trans_Obj
//
/// @brief uses the given parameters to build a new transaction object for
///		each transaction initiated by the LM component.
//
//======================================================================
void bridge_lmodel::set_trans_Obj(	tlm::tlm_generic_payload& tObj, 
									tlm::tlm_command cmd, 
									const unsigned int &target_id,
									unsigned char* data,
									sc_dt::uint64 adr)
{
	unsigned int	data_length;
	
	switch(target_id)
	{
		case(sae_id_nr):
			tObj.set_command		(	tlm::TLM_READ_COMMAND	);
			tObj.set_data_length	(	lm_sae_dl				);
			tObj.set_streaming_width(	lm_sae_dl				);		
			break;

		case(see_id_nr):
			tObj.set_command		(	tlm::TLM_WRITE_COMMAND	);
			tObj.set_data_length	(	lm_res_dl				);
			tObj.set_streaming_width(	lm_res_dl				);
			break;
	}
		tObj.set_address			(	adr						);
		tObj.set_data_ptr			(	data					);
		tObj.set_response_status	(	tlm::TLM_INCOMPLETE_RESPONSE);
		tObj.set_gp_option			(	tlm::TLM_MIN_PAYLOAD	); //??
		tObj.set_dmi_allowed		(	false					);
		tObj.set_byte_enable_ptr	(	0						);
		tObj.set_byte_enable_length (	0						);
	}


//======================================================================
/// @fn b_transport
//
/// @brief Implementation of the blocking transport for incoming transactions on 
///		the LM report memory.
//
/// @details
///		Only the Monitor has read access to the LM memory. The data stored on the report memory
///		represents the status report of the LM actions.
//
//======================================================================
//interface methods - forward path
	void bridge_lmodel::b_transport	(	tlm::tlm_generic_payload& tObj, 
											sc_core::sc_time& delay	)
	{
		lm_repmem.operation(monitor_id_nr, tObj, delay);
		return;
	}

//======================================================================
/// @fn get_direct_mem_ptr
//
/// @brief gets a DMI pointer on a SAE memory subarea
//
/// @details not yet implemented
//
//======================================================================
	bool bridge_lmodel::get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 
												tlm::tlm_dmi& dmi_data	)
	{
		return false;
	}

	tlm::tlm_sync_enum bridge_lmodel::nb_transport_fw	(	tlm::tlm_generic_payload& tObj, 														
															tlm::tlm_phase& phase, 
															sc_core::sc_time& delay )
	{
		return tlm::TLM_ACCEPTED;
	}

	unsigned int bridge_lmodel::transport_dbg	(	tlm::tlm_generic_payload& tObj )
	{
		return 0;
	}


//======================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief invalidates a DMI pointer on the SAE memory subarea which is
///		designated by the given address parameters
//
/// @details
///		not yet implemented
//
//======================================================================
void bridge_lmodel::invalidate_direct_mem_ptr(	sc_dt::uint64 start_range, 
												sc_dt::uint64 end_range		)
{
	return;
}

tlm::tlm_sync_enum bridge_lmodel::nb_transport_bw(	tlm::tlm_generic_payload& tObj, 
													tlm::tlm_phase& phase, 
													sc_core::sc_time& delay		)
{
	return tlm::TLM_ACCEPTED;
}


//================================================================================
/// @fn do_report
//
/// @brief It writes the report data on the report memory of the LM component.
//
/// @details the writen values depends on the evaluation results of the sensor data 
///		readout from the sae's memory.
//
/// @see self_write
//=================================================================================
void bridge_lmodel::set_target_id( const unsigned int &id	)
{
	lm_target_id = id;
}

void bridge_lmodel::do_report(void)
{
	if (lm_actions_status == 33)
			lm_repmem.self_write(0, (sc_dt::uchar*)&lm_neg_result, lm_rep_dl);
		else
			lm_repmem.self_write(0, (sc_dt::uchar*)&lm_pos_result, lm_rep_dl);

}