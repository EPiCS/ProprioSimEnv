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

#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <systemc.h>

#include "bridge_monitor.h"
#include "constants.h"
#include "reporting.h"
#include "key_functions.h"

unsigned int		m_target_idx	= 3	;
static const char	*filename		= "bridge_monitor.cpp";			///< filename for reporting
static const char	*filename1		= "bridge_monitor.cpp_E1";		///< filename for reporting
static const char	*filename2		= "bridge_monitor.cpp_E2";		///< filename for reporting
static const char	*suffix			= "notifcount_output.m";


//=============================================================================
///	@fn bridge_monitor::bridge_monitor
//	    
///	@brief bridge_monitor constructor
//	    
///	@details
///		It declares the thread processes ( monitor_see_thread(), monitor_lm_thread() ) 
///		of the componenent.<br>
///		It binds the initiator and target sockets to the module.<br>
///		It sets the global quantum and initializes the local quantum of each 
///		process for temporal decoupling.
// 
//=============================================================================
bridge_monitor::bridge_monitor							///< constructor
( sc_core::sc_module_name   module_name					///< SC module name
, const unsigned int        ID							///< monitor ID
, sc_dt::uint64             mem_size					///< memory size  (bytes)
, unsigned int              mem_width					///< memory width (bytes)
, unsigned int 				lm_report_data_length		///< data length for report data from LM  (bytes)
, unsigned int 				see_report_data_length		///< data length for report data from SEE (bytes)
, const sc_core::sc_time    read_latency				///< read response delay 
, const sc_core::sc_time    write_latency				///< write response delay
, const sc_core::sc_time	lm_read_latency_			///< write response delay 
, const sc_core::sc_time	see_read_latency_			///< write response delay 
, const sc_core::sc_time	glob_quantum				///< global time quantum
, unsigned int				nodenbr_					///< Position index for the vectoors of events
)		
:	
  m_id						( ID					)
, m_name					( name()				)
, m_isocket					( "isocket"				) 
, m_tsocket					( "tsocket"				)
, m_see_delay				( sc_core::SC_ZERO_TIME	)
, m_lm_delay				( sc_core::SC_ZERO_TIME	)
, m_see_report_dl			( see_report_data_length)
, m_lm_report_dl			( lm_report_data_length	)
, m_mem
	(
	  ID
	, read_latency
	, write_latency
	, mem_size
	, mem_width
	)
	, nodenbr(nodenbr_)
{
	SC_THREAD(monitor_see_thread);				
	SC_THREAD(monitor_lm_thread);				

	m_qk1.set_global_quantum(glob_quantum);
	m_qk2.set_global_quantum(glob_quantum);

	m_qk1.reset();											
	m_qk2.reset();							

	m_isocket.bind( *this );
	m_tsocket.bind( *this );
	
	m_notifcount_file = file_ops::build_filename(m_name, suffix);

}


//=============================================================================
///	@fn bridge_monitor::monitor_see_thread
//	    
///	@brief Thread process E2 of the process cycle of the Node.
//	    
///	@details
///		It sets new transactions objects to read the report data from the SEE  
///		memory. It suspends after every performed transaction and wait until
///		it receives the SEE notification.
///		<br> The funtion trtr::follow_transactions() is called before and 
///		after every process synchronization to report or monitor them.
//
///	@see 	bridge_monitor::set_trans_Obj()
/// @see 	bridge_monitor::execute_trans()
///	@see	trtr::follow_transactions()
//
///	@retval none    
//=============================================================================
void bridge_monitor::monitor_see_thread(void)			// zu bearbeiten //E2
{
	std::ostringstream	msg;							// log message
	msg.str ("");

	sc_core::sc_time	wait_time		= sc_core::SC_ZERO_TIME;
	int					from_see_adr	= 0; 
	unsigned char		*data			= new unsigned char[size_t(m_see_report_dl)];			///< The size of the array in which the data is copied is set to the value of the constant variable #dl_report_data.
	unsigned int		burst_length	= (unsigned int)ceil((double)m_see_report_dl/buswidth2);

	while ( true )
	{
		m_see_delay = m_qk1.get_local_time();
		wait( see_to_monitor_vector.at(nodenbr) );

		set_target_id	( see_id_nr );
		set_trans_Obj	( m_tObj, from_see_adr, data);
		trtr::follow_transactions(filename2, m_name, m_id, m_see_delay, trtr::T_CALL, trtr::T_IGNORE, lmodel_id_nr);
		execute_trans	( m_tObj	);

		m_qk1.set( m_see_delay );						// Update quantum keeper with time consumed by target	
		wait(wait_time);	

		if ( m_qk1.need_sync() )						// Check local time against quantum and sync if neccessary
		{
			trtr::follow_synch(filename, m_name, "E2", m_id, m_see_delay, trtr::S_NEED);
			m_qk1.sync();
			trtr::follow_synch(filename, m_name, "E2", m_id, m_see_delay, trtr::S_RETURN);
		}
		memset( data, 0, size_t(m_see_report_dl) );
	}

}


//=============================================================================
///	@fn bridge_monitor::monitor_lm_thread
//	    
///	@brief Thread process E1 of the process cycle of the Node.
///
//  
///	@details
///		It sets new transactions objects to read the report data from the LM  
///		memory. It suspends after every performed transaction and wait until
///		it receives the LM notification.
///		<br> The funtion trtr::follow_transactions() is called before and 
///		after every process synchronization to report or monitor them.
//
///	@see 	bridge_monitor::set_trans_Obj()
/// @see 	bridge_monitor::execute_trans()
///	@see	trtr::follow_transactions() 
//=============================================================================
void bridge_monitor::monitor_lm_thread(void) //E1
{
	std::ostringstream	msg;							// log message
	msg.str ("");

	unsigned char		*data			= new unsigned char[size_t(m_lm_report_dl)];	
	unsigned int		burst_length	= (unsigned int)ceil((double)m_lm_report_dl/buswidth2);
	unsigned int		from_lm_adr		= 0;
	
	while( true )
	{
		wait(lmodel_to_monitor_vector.at(nodenbr)); 

		m_lm_delay = m_qk2.get_local_time();
		set_target_id	( lmodel_id_nr );
		set_trans_Obj	( m_tObj, from_lm_adr, data );
		trtr::follow_transactions(filename1, m_name, m_id, m_lm_delay, trtr::T_CALL, trtr::T_IGNORE, lmodel_id_nr);
		execute_trans	(m_tObj);

		monitor_ev_vector.at(nodenbr).notify();

		m_qk2.set( m_lm_delay );					// Update quantum keeper with time consumed by target
		wait(sc_core::SC_ZERO_TIME);

		if ( m_qk2.need_sync() ) 					// Check local time against quantum and sync if neccessary
		{
			trtr::follow_synch(filename, m_name, "E1", m_id, m_lm_delay, trtr::S_NEED);
			m_qk2.sync();
			trtr::follow_synch(filename, m_name, "E1", m_id, m_lm_delay, trtr::S_RETURN);
		}

		memset( data, 0, size_t(m_lm_report_dl) );
	}	
}


void bridge_monitor::execute_trans( tlm::tlm_generic_payload& tObj)
{
	switch(m_target_idx)
	{
	case 0:	
		m_isocket -> b_transport(m_tObj, m_lm_delay);
		if ( m_tObj.is_response_ok() )				//in case of a successful transaction 
		{
			trtr::follow_transactions(filename1, m_name, m_id, m_lm_delay, trtr::T_RETURN, trtr::T_SUCCESS, lmodel_id_nr);
		}
		else										//could not read
		{
			trtr::follow_transactions(filename1, m_name, m_id, m_lm_delay, trtr::T_RETURN, trtr::T_FAILURE, lmodel_id_nr , m_tObj.get_response_string().c_str() );
		}
		break;
		
	case 1:
		m_isocket -> b_transport(m_tObj, m_see_delay);
		if ( m_tObj.is_response_ok() )				//in case of a successful transaction 
		{
			trtr::follow_transactions(filename2, m_name, m_id, m_see_delay, trtr::T_RETURN, trtr::T_SUCCESS, see_id_nr);
		}
		else 										//could not read
		{
			trtr::follow_transactions(filename2, m_name, m_id, m_see_delay, trtr::T_RETURN, trtr::T_FAILURE, see_id_nr , m_tObj.get_response_string().c_str() );
		}
		break;

	default:
		break;
	}	
}


//=============================================================================
///	@fn bridge_monitor::b_transport
//	    
///	@brief Implementation of the blocking transport method for access on the 
///		monitor memory.
//
//=============================================================================
void bridge_monitor::b_transport	(	tlm::tlm_generic_payload& tObj, 
										sc_core::sc_time& delay		)
{
	m_mem.operation(gvoc_id_nr, tObj, delay);
}

//==============================================================================
///  @fn initiator_top::get_direct_mem_ptr
//         
///  @brief It gets a direct memory pointer on the target component memory.
//  
///  @details
///    not yet implemented.
//
//============================================================================== 
bool bridge_monitor::get_direct_mem_ptr		(	tlm::tlm_generic_payload& tObj, 
												tlm::tlm_dmi& dmi_data	)
{
	return false;
}


tlm::tlm_sync_enum bridge_monitor::nb_transport_fw	(	tlm::tlm_generic_payload& tObj, 
														tlm::tlm_phase& phase, 
														sc_core::sc_time& delay )
{
	return tlm::TLM_ACCEPTED;
}


unsigned int bridge_monitor::transport_dbg	( tlm::tlm_generic_payload& tObj )
{
	return 0;
}
	

//=============================================================================
///	@fn bridge_monitor::invalidate_direct_mem_ptr
//	    
/// @brief invalidates a DMI pointer on the SEE or LM memory subarea which is
///		designated by the given address parameters
//
/// @details
///		not yet implemented
//	  
//=============================================================================
void bridge_monitor::invalidate_direct_mem_ptr		(	sc_dt::uint64 start_range, 
														sc_dt::uint64 end_range )
{
	return;
}


tlm::tlm_sync_enum bridge_monitor::nb_transport_bw	(	tlm::tlm_generic_payload& tObj, 
														tlm::tlm_phase& phase, 
														sc_core::sc_time& delay )
{
	return tlm::TLM_ACCEPTED;
}


//=============================================================================
///	@fn bridge_monitor::set_trans_Obj
//	    
///	@brief uses the given parameters to build a new transaction object for
///		each transaction initiated by the monitor component.
//  
//=============================================================================
void bridge_monitor::set_trans_Obj	(	tlm::tlm_generic_payload& tObj,
										const sc_dt::uint64 &adr,
										unsigned char* data
									)
{
	std::ostringstream			msg;
	unsigned int				data_length;

	switch(m_target_idx)
	{
	case 0: //lmodel
		data_length = m_lm_report_dl;
		break;
	case 1: //see
		data_length = m_see_report_dl;
		break;
	default:
		msg << "\t MONITOR: " << m_id
			<< "\t Invalid Target ID: "<< m_target_idx;
		ERROR_LOG(filename, __FUNCTION__ , msg.str());
		break;
	}

	tObj.set_command			( tlm::TLM_READ_COMMAND		)	;
	tObj.set_address			( adr 						)	;
	tObj.set_data_ptr			( data 						)	;
	tObj.set_data_length		( data_length				)	;
	tObj.set_byte_enable_ptr	( 0 						)	;
	tObj.set_byte_enable_length ( 0 						)	;
	tObj.set_streaming_width	( data_length 				)	;
	tObj.set_gp_option			( tlm::TLM_MIN_PAYLOAD		 )	;
	tObj.set_dmi_allowed		( false 					)	;
	tObj.set_response_status	( tlm::TLM_INCOMPLETE_RESPONSE );
	
}


//=============================================================================
///	@fn bridge_monitor::set_target_id
//	    
///	@brief indicates the target of a monitor transaction
//	    
///	@details
///		sets \a m_target_idx to \a 0 if the target is the LModel component 
///		or to \a 1 if the target is the see component.
//	
///	@param id	predefined ID of a component
//	   
//=============================================================================
void bridge_monitor::set_target_id ( const unsigned int &id )
{
	std::ostringstream			msg;
	msg.str("");

	switch(id)
	{
		case lmodel_id_nr:
			m_target_idx = 0;
			break;
		case see_id_nr:
			m_target_idx = 1;
			break;
		default:	
			msg << "\t MONITOR: " << m_id
				<< "\t Invalid Target ID: "<< id;
			INFO_LOG(filename, __FUNCTION__ , msg.str());
			m_target_idx = 3;
			break;
			
	}
}