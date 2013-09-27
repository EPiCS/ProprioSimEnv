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

//==============================================================================
///  @file initiator_othernode.h
//
///  @brief This is the external sensor component  of the proprioceptive 
///		node termed "Other Nodes"
//
///  @details
///		It writes datasets into the SAE memory by performing blocking transport calls.<br>
///		Its role: Initiator <br>
///		His target: SAE through the interconnect component IC1 <br>
///		Its process: B
//		
//==============================================================================

#ifndef __INITIATOR_OTHERNODE_H__
#define __INITIATOR_OTHERNODE_H__

#include <systemc.h>
#include "tlm.h"																// TLM headers
#include <tlm_utils\tlm_quantumkeeper.h>
#include <queue>

#include "constants.h"

class initiator_othernode:	public sc_module, 
							virtual public tlm::tlm_bw_transport_if<>{

public:	
// constructor
	initiator_othernode	(		sc_module_name module_name						///< sc module name
							, 	const unsigned int ID							///< component ID
							, 	unsigned int data_length						///< maximal length for sensor data
							,	unsigned int nr_dataset_cycle					///< number of sensor datasets pro cycle
							,	sc_core::sc_time	glob_quantum				///< time value for the global quantum
						);

	SC_HAS_PROCESS(initiator_othernode);

// Methods declarations
	void on_thread(void);

	void set_trans_Obj	(	tlm::tlm_generic_payload& tObj,						///< ref transaction object
							const sc_dt::uint64 &adr,							///< const ref to transaction data
							unsigned char* data									///< pointer to the transaction data
						);

	void initiator_othernode::execute_trans	(  tlm::tlm_generic_payload& tObj	///< ref transaction object
											);

	void invalidate_direct_mem_ptr	( 	sc_dt::uint64 start_range, 				///< start address of the memory range
										sc_dt::uint64 end_range					///< end address of the memory range
									);  

	/// Required und unused interface method
	tlm::tlm_sync_enum nb_transport_bw 	( 	tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay 			///< ref to time delay
										);
	void read_out_fifo();


// Variable and Object Declarations ============================================
	tlm::tlm_initiator_socket<buswidth1>						on_isocket;		///< initiator socket for communication with ic1
	sc_core::sc_port< sc_fifo_in_if<sc_dt::uchar> >				on_data_inport;	///< input port for incoming data from other nodes.

private:

	unsigned int					on_id				;						///< component id
	std::string 					on_name				;						///< component name
	tlm::tlm_generic_payload		on_tObj				;						///<transaction object
	unsigned int					on_data_length, on_dataset_cycle;			///< transaction data length, number of dataset pro cycle which will be sent to the SAE
	sc_core::sc_time				on_delay			;						///< transaction delay
	sc_dt::uint64					on_mem_adr_start; //on_mem_adr_end;			///< transaction start address
	sc_dt::uchar					*on_data			;						///< data pointer
	tlm_utils::tlm_quantumkeeper	on_qk				;						///< quantum keeper
	std::queue< sc_dt::uchar, std::deque<sc_dt::uchar> > on_queue;				///< data container
}; 

#endif /* __INITIATOR_OTHERNODE_H__ */