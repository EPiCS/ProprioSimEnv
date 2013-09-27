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
*===========================================================================
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

#include "initiator_sensorenv.h"
#include "constants.h"
#include "reporting.h"
#include "key_functions.h"

using namespace std;

/// filenames for reporting
static const char			*filename	= "initiator_sensorenv.cpp";	
static const char			*suffix		= "data_output.m";
//======================================================================
/// @fn initiator_sensorenv
//
/// @brief constructor
//
/// @details
///		declares the thread process ( sensor_thread() ) of this component.
///		<br> binds the initiator socket to the module. <br>
///		<br> initializes some data block
///		sets the global quantum and re-calculates the local quantum for
///		temporal decoupling.
//
//======================================================================
initiator_sensorenv::initiator_sensorenv(	sc_module_name		module_name							///< sc module name
										, 	const unsigned		int ID								///< component ID
										, 	unsigned int		data_length							///< transaction data length
										,	unsigned int		nr_dataset_cycle					///< number of sensor datasets pro cycle
										,	sc_core::sc_time	glob_quantum						///< time value for the global quantum
										,	unsigned int		fifo_size							///< size of the fifo channel bound to its input port
										)
										: 
										  s_name			(  name()				)
										, s_id				(  ID					)
										, s_isocket			(  "s_isocket"			) 
										, s_mem_adr_start	(  0					)
										, s_data_length		(  data_length			)
										, s_dataset_cycle	(  nr_dataset_cycle		)
										, s_delay			(  sc_core::sc_time( 0, node_time_unit ) )
										, s_fifo_size		(	fifo_size)
{
	SC_THREAD(sensor_thread);
	s_qk.set_global_quantum ( glob_quantum );	
	s_qk.reset();								
	s_isocket.bind(*this);

	if ( (s_fifo_size/s_dataset_cycle) < 1 )
		ERROR_LOG( filename, __FUNCTION__ , "Number of Data block per cycle is bigger than the fifo_size!!!");

	senv_dataoutput_file = file_ops::build_filename( s_name, suffix);
};

//======================================================================
/// @fn sensor_thread
//
/// @brief Thread process B of the process cycle of the Node.
//
/// @details  It generates new data, sets new transactions objects to  
///		perform each transaction or write operation on the SAE Memory.
//
///	@see 	initiator_sensorenv::set_trans_Obj()
/// @see 	initiator_sensorenv::execute_trans(tlm::tlm_generic_payload& tObj)
//
//======================================================================
void initiator_sensorenv::sensor_thread(void)
{
	sc_core::sc_time	wait_time	=	sc_core::SC_ZERO_TIME;

	wait( 0.2, node_time_unit );
	while( true )
	{ 
		s_delay				=	s_qk.get_local_time();				// Annotate b_transport with local time	
		read_out_fifo();											// daten aus dem FiFO lesen
		set_trans_Obj	(	s_tObj, s_mem_adr_start, &senv_data	);
		execute_trans	(	s_tObj	);		

		if (s_dataset_cycle>1)
		{
			s_mem_adr_start += s_data_length;					
		}
		memset(&senv_data, 0, size_t(s_data_length));

		s_qk.set( s_delay );										// Update quantum keeper with time consumed by target
		if ( s_qk.need_sync() )										// Check local time against quantum and sync if neccessary
		{
			s_mem_adr_start = 0;
			trtr::follow_synch(filename, s_name, "B", s_id, s_delay, trtr::S_NEED);
			s_qk.sync();
			trtr::follow_synch(filename, s_name, "B", s_id, s_delay, trtr::S_RETURN);
		}	
		
	}
};

//======================================================================
/// @fn execute_trans
//
/// @brief performs b_tranport call for each generated transaction in 
///		the thread process of the component.
//
/// @details
///		The funtion trtr::follow_transactions() is called before and 
///		after every b_transport method call and every process synchro
///		nization to report or monitor them.
//
///	@see	trtr::follow_transactions()
//
//======================================================================
void initiator_sensorenv::execute_trans(	tlm::tlm_generic_payload& tObj)
{
	trtr::follow_transactions(filename, s_name, s_id, s_delay, trtr::T_CALL, trtr::T_IGNORE, sae_id_nr);

	s_isocket -> b_transport(tObj, s_delay);

	if ( tObj.is_response_ok() )
	{
		trtr::follow_transactions(filename, s_name, s_id, s_delay, trtr::T_RETURN, trtr::T_SUCCESS, sae_id_nr); 	
	}
	else
	{
		trtr::follow_transactions(filename, s_name, s_id, s_delay, trtr::T_RETURN, trtr::T_FAILURE, sae_id_nr , tObj.get_response_string().c_str() );
	}
};


//======================================================================
/// @fn invalidate_direct_mem_ptr
//
/// @brief invalidates a DMI pointer on the SAE memory subarea, which is
///		designated by the given address parameters
//
/// @details
///		not yet implemented
//
//======================================================================
void initiator_sensorenv::invalidate_direct_mem_ptr(	sc_dt::uint64 start_range,   
													sc_dt::uint64 end_range		)
{
	return;
}

tlm::tlm_sync_enum initiator_sensorenv::nb_transport_bw(tlm::tlm_generic_payload& tObj, 
														tlm::tlm_phase& phase, 
														sc_core::sc_time& delay )
{
	return tlm::TLM_ACCEPTED;
}

//======================================================================
/// @fn set_trans_Obj
//
/// @brief uses the given parameters to build a new transaction object for
///		the next transaction 
//
//======================================================================
void initiator_sensorenv::set_trans_Obj(tlm::tlm_generic_payload& tObj,
										const sc_dt::uint64 &adr,
										unsigned char* data)
{
	tObj.set_command			( tlm::TLM_WRITE_COMMAND		);
	tObj.set_address			( adr							);
	tObj.set_data_length		( s_data_length					);
	tObj.set_data_ptr			( data							);
	tObj.set_streaming_width	( s_data_length					);
	tObj.set_response_status	( tlm::TLM_INCOMPLETE_RESPONSE	);
	tObj.set_gp_option			( tlm::TLM_MIN_PAYLOAD			); 
	tObj.set_dmi_allowed		( false							);
	tObj.set_byte_enable_ptr	( 0								);
	tObj.set_byte_enable_length ( 0								);
}



//======================================================================
/// @fn read_out_fifo
//
/// @brief reads a dataset from the fifo channel linked to the input port
///			of this component and stores it in the private class
///			variable #senv_data
//
/// @attention The port and fifo channel have the data type datyp, which is 
///			 unsigned char. This is why data type conversion isn't performed 
///			here. But it you use a different data type, you must convert the
///			data to unsigned char or a pointer of type unsigned char after 
///			the readout and before every transaction.
//
//======================================================================
void initiator_sensorenv::read_out_fifo()
{
	while ( senv_data_inport->num_available() >0 )
	{
		senv_data = 	senv_data_inport->read();
	} 
}