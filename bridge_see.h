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
///  @file bridge_see.h
//
///  @brief This is the self-expressive engine of a proprioceptive Node
//
///  @details
///  	It takes decision about the measures and actions which should be taken
///		by the node as a result to the data received from the LModel component
///		and with respect to the g.v.o.c data. <br>
///		Its role: Bridge <br>
///	 	Its Initiators: LM - Monitor - GVOC through the Interconnect3 component <br>	
///		Its Targets: Actuators through the Interconnect4 compponent <br>
///		Its process: D
//
//==============================================================================

#ifndef __BRIDGE_SEE_H__
#define __BRIDGE_SEE_H__
 
#include <systemc.h>
#include <tlm.h>
#include <tlm_utils\tlm_quantumkeeper.h>
#include <tlm_utils\simple_target_socket.h>

#include "constants.h"
#include "memory.h"
#include "see_core.h"

class bridge_see:		public sc_module,
						virtual public tlm::tlm_bw_transport_if<>
{
public:
// Constructor =================================================================
	//bridge_see ( sc_module_name name);

	bridge_see	( sc_core::sc_module_name   module_name							///< SC module name
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
				);

	SC_HAS_PROCESS ( bridge_see );

// Method Declarations =========================================================
	void see_thread(); //thread


	void execute_trans	( tlm::tlm_generic_payload& tObj								///< ref to transaction object
						);

	void do_report();

	void set_trans_Obj	(	tlm::tlm_generic_payload& tObj,								///< ref to transaction object
							unsigned int adr,											///< transaction start address
							unsigned char* data											///< pointer to transaction data
						);

	//interface-methods forward path
	void b_transport	(	int id,														///< ID of the simple tagged target socket receiving the call
							tlm::tlm_generic_payload& tObj, 							///< ref to transaction object
							sc_core::sc_time& delay										///< ref to delay time		
						);

	bool get_direct_mem_ptr	(	int id,													///< ID of the simple tagged target socket receiving the call
								tlm::tlm_generic_payload& tObj, 						///< ref to transaction object
								tlm::tlm_dmi& dmi_data									///< ref to dmi-descriptor	
							);

	tlm::tlm_sync_enum nb_transport_fw	(	int id,										///< ID of the simple tagged target socket receiving the call
											tlm::tlm_generic_payload& tObj, 			///< ref to transaction object
											tlm::tlm_phase& phase, 						///< ref to transaction phase
											sc_core::sc_time& delay 					///< ref to delay time
										);

	unsigned int transport_dbg	( int id,												///< ID of the simple tagged target socket receiving the call
								  tlm::tlm_generic_payload& tObj 						///< ref to transaction object
								);
	
	//interface-methods backward path
	void invalidate_direct_mem_ptr		(	sc_dt::uint64 start_range, 					///< start address of the memory-range
											sc_dt::uint64 end_range 					///< end address of the memory-range
										);  

	tlm::tlm_sync_enum nb_transport_bw	(	tlm::tlm_generic_payload& tObj, 			///< ref to transaction object
											tlm::tlm_phase& phase, 						///< ref to transaction phase
											sc_core::sc_time& delay 					///< ref to delay time
										);

	//generate data to be transferred 
	unsigned int see_eval_data	(	const unsigned int &adr, 							///< const ref to the adr
									const unsigned int &data_length						///< const ref to the data length
								);

// Variable and Object Declarations ============================================
	tlm::tlm_initiator_socket<buswidth4>	see_isocket;								///< initiator socket for the communication with ic4

	tlm_utils::simple_target_socket_tagged<bridge_see, buswidth2> see_monitor_tsocket;	///< tagged simple target socket for the communication with monitor
	tlm_utils::simple_target_socket_tagged<bridge_see, buswidth5> see_lmodel_tsocket;	///< tagged simple target socket for the communication with LM
	tlm_utils::simple_target_socket_tagged<bridge_see, buswidth3> see_gvoc_tsocket;		///< tagged simple target socket for the communication with gvoc				

private:
	std::string						see_name	;										///< module name
	const unsigned int				see_id		;										///< see component id
	const unsigned int				see_gvoc_mem_size, see_lm_mem_size, see_rep_mem_size; ///< sizes of the reserved memory blocks
	tlm::tlm_generic_payload		see_tObj	;										///< transaction object
	unsigned int					see_data, see_adr;									///< transaction data and start adress
	unsigned int					see_rep_data_length, see_act_data_length;			///< transaction data length
	tlm_utils::tlm_quantumkeeper	see_qk		;										///< quantum keeper
	sc_core::sc_time 				see_idelay	;										///< transaction delay for temporal decoupling
	memory							see_lm_mem, see_gvoc_mem, see_rep_mem;				///< memory objects
	unsigned int					m_cycle_nr_trigger;									///< user specification with which the monitor component
	see_core						*see_core_obj;										
	sc_dt::uchar					*see_dec_data;										///< data pointer 

	void set_target_idx				(	int id	);										/// defines the target component of the transaction by setting #see_target_idx	
	unsigned int					nodenbr;
	std::string						see_notifcount_file;

};
#endif /*__BRIDGE_SEE_H__*/