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
*================================================================================
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

#include "initiator_othernode.h"
#include "reporting.h"
#include "constants.h"

static const char			*filename = "initiator_othernode.cpp_B"	;	/// filename for reporting


//================================================================================
/// @fn initiator_othernode
//
/// @brief constructor
//
/// @details 
///		declares the thread process ( on_thread() ) of this component.
///		<br>binds the initiator socket to the module. <br>
///		sets the global quantum and re-calculates the local quantum for
///		temporal decoupling.	  
//
//================================================================================
initiator_othernode::initiator_othernode	(	sc_module_name module_name						///< sc module name
											, 	const unsigned int ID							///< component ID
											, 	unsigned int data_length						///< maximal length for sensor data
											,	unsigned int nr_dataset_cycle					///< number of sensor datasets pro cycle
											,	sc_core::sc_time	glob_quantum				///< time value for the global quantum
											):
											  on_id				(	ID				)
											, on_name			(	name()			)
											, on_data_length	(	data_length		)
											, on_dataset_cycle	(	nr_dataset_cycle)
											, on_mem_adr_start	(	0				)
											, on_isocket		(	"on_isocket"	)
											
{
	SC_THREAD( on_thread );

	on_isocket.bind( *this );
	on_qk.set_global_quantum ( glob_quantum );		
	on_qk.reset();								
}


//======================================================================
/// @fn on_thread
//
/// @brief Thread process B of the process cycle of the Node.
//
/// @details
///		It readouts data, which were sent from others nodes, from the fifo vector
///		of this component and generate new transactions to send it to the SAE component. 
//
//	@see	initiator_gvoc::read_out_fifo()
/// @see	initiator_gvoc::set_target_idx()
///	@see	initiator_gvoc::set_trans_Obj()
/// @see 	initiator_gvoc::execute_trans()
//======================================================================
void initiator_othernode::on_thread(void)
{
	std::ostringstream  msg;
	sc_core::sc_time	wait_time		=	sc_core::SC_ZERO_TIME;
	sc_dt::uchar		data;

	wait( 0.3, node_time_unit );
	while( true )
	{ 
		on_delay						=	on_qk.get_local_time();		
		
		read_out_fifo();
		if ( on_queue.empty() == false )
		{
			data = on_queue.front();
		}
		else 
		{
			msg<< "\t No Data available!";
			ERROR_LOG(filename, __FUNCTION__ , msg.str());
		}
		on_data = &data; 
		set_trans_Obj(	on_tObj, on_mem_adr_start, on_data);
		execute_trans(	on_tObj );

		if (on_dataset_cycle>1)
		{
			memset(on_data, 0, size_t(on_data_length)); 
			on_mem_adr_start += on_data_length;
		}

		on_qk.set( on_delay );							// Update quantum keeper with time consumed by target
		if ( on_qk.need_sync() )						// Check local time against quantum and sync if neccessary
		{
			on_mem_adr_start = 0;

			trtr::follow_synch(filename, on_name, "B", on_id, on_delay, trtr::S_NEED);
			on_qk.sync();
			trtr::follow_synch(filename, on_name, "B", on_id, on_delay, trtr::S_RETURN);
		}	

	}
}


//======================================================================
/// @fn execute_trans
//
/// @brief performs b_tranport call for each generated transaction in 
///		the thread processes of this component.
//
//======================================================================
void initiator_othernode::execute_trans( tlm::tlm_generic_payload& tObj )
{
	trtr::follow_transactions(filename, on_name, on_id, on_delay, trtr::T_CALL, trtr::T_IGNORE, sae_id_nr);

	on_isocket -> b_transport(tObj, on_delay);

	if ( tObj.is_response_ok() )
	{
		trtr::follow_transactions(filename, on_name, on_id, on_delay, trtr::T_RETURN, trtr::T_SUCCESS, sae_id_nr); 	
	}
	else
	{
		trtr::follow_transactions(filename, on_name, on_id, on_delay, trtr::T_RETURN, trtr::T_FAILURE, sae_id_nr , tObj.get_response_string().c_str() );
	}
}

void initiator_othernode::invalidate_direct_mem_ptr	( 	sc_dt::uint64 start_range, 				///< start address of the memory range
															sc_dt::uint64 end_range				///< end address of the memory range
														)
{
	return;
}

tlm::tlm_sync_enum initiator_othernode::nb_transport_bw 	( 	tlm::tlm_generic_payload& tObj, ///< ref to transaction object
															tlm::tlm_phase& phase, 				///< ref to transaction phase
															sc_core::sc_time& delay 			///< ref to time delay
															)
{
	return tlm::TLM_COMPLETED;
}


//======================================================================
/// @fn set_trans_Obj
//
/// @brief uses the given parameters to build a new transaction object for
///		each transaction 
//
//======================================================================
void initiator_othernode::set_trans_Obj(	tlm::tlm_generic_payload& tObj,
											const sc_dt::uint64 &adr,
											unsigned char* data)
{
	tObj.set_command			( tlm::TLM_WRITE_COMMAND		);
	tObj.set_address			( adr							);
	tObj.set_data_length		( on_data_length				);
	tObj.set_data_ptr			( data							);
	tObj.set_streaming_width	( on_data_length				);
	tObj.set_response_status	( tlm::TLM_INCOMPLETE_RESPONSE	);
	tObj.set_gp_option			( tlm::TLM_MIN_PAYLOAD			); 
	tObj.set_dmi_allowed		( false							);
	tObj.set_byte_enable_ptr	( 0								);
	tObj.set_byte_enable_length ( 0								);
}


//======================================================================
/// @fn read_out_fifo
//
/// @brief It reads out the available dataset from the fifo channel to which the input port 
///			of this component is bounded and pushed them one by one in a vector. <br>
///			This is performed by every execution of Process D to always make empty slots available 
///			for other nodes to write in and thus ensure an asynchron communication
///			between the nodes.
//
/// @details just an example
//
//======================================================================
void initiator_othernode::read_out_fifo()
{
	while ( on_data_inport->num_available() >0 )
	{
		on_queue.push(	on_data_inport->read()	);
	} 
}