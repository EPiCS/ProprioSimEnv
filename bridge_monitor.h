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
///  @file bridge_monitor.h
//
///  @brief This is the monitor component of a proprioceptive Node
//
///  @details
///  	He controls the actions of the SEE and the LM components. Each of these 
///		components writes the report of their actions in their memories.  
///		The monitor readouts these report memories and just displays them. <br>
///		Its role: Bridge <br>
///	 	Its initiator: GVOC through the Interconnect3 component. <br>
///	 	Its Targets: SEE and SAE through the Interconnect2 compponent <br>
///		Its processes: E1 and E2
//
//==============================================================================

#ifndef __BRIDGE_MONITOR_H__
#define __BRIDGE_MONITOR_H__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils\tlm_quantumkeeper.h>
#include <tlm_utils\simple_initiator_socket.h>
#include <tlm_utils\simple_target_socket.h>

#include "constants.h"
#include "memory.h"

class bridge_monitor:	public sc_module,
						virtual public tlm::tlm_fw_transport_if<>,
						virtual public tlm::tlm_bw_transport_if<>
{
public:
// Constructor ================================================================= 

	bridge_monitor
	  ( sc_core::sc_module_name		module_name                 ///< sc module name
	  , const unsigned int			ID							///< Monitor ID
	  , sc_dt::uint64				mem_size					///< memory size (bytes)
	  , unsigned int				mem_width					///< memory width (bytes)
	  , unsigned int 				lm_report_data_length		///< data length for report data from LM (bytes)
	  , unsigned int 				see_report_data_length		///< data length for report data from SEE(bytes)
	  , const sc_core::sc_time		read_latency				///< read response delay on the monitor memory for a single transaction
	  , const sc_core::sc_time		write_latency				///< write response delay on the monitor memory for a single transaction
	  , const sc_core::sc_time		lm_read_latency_			///< write response delay on the LM memory for a single transaction
	  , const sc_core::sc_time		see_read_latency_			///< write response delay on the see memory for a single transaction
	  , const sc_core::sc_time		glob_quantum				///< time value for the globl quantum
	  , unsigned int				nodenbr_					///< Position index for the vectoors of events
	  );

	SC_HAS_PROCESS ( bridge_monitor );

// Method Declarations =========================================================
	
	void monitor_see_thread(); //thread
	void monitor_lm_thread();  //thread

	void execute_trans	(	tlm::tlm_generic_payload& tObj 						///< ref to transaction object
						);

	void set_trans_Obj	(	tlm::tlm_generic_payload& tObj,						///< ref to transaction object
							const sc_dt::uint64 &adr,							///< transaction start address
							unsigned char* data									///< transaction data pointer
						);


//interface-methods forward path
	void b_transport	(	tlm::tlm_generic_payload& tObj, 					///< ref to transaction object
							sc_core::sc_time& delay								///< ref to delay time
						);

	bool get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								tlm::tlm_dmi& dmi_data							///< ref to dmi-descriptor
							);
							
	/// Required and unused virtual method
	tlm::tlm_sync_enum nb_transport_fw (	tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay 			///< ref to delay time
										);

	/// Required and unused virtual method								
	unsigned int transport_dbg 	( tlm::tlm_generic_payload& tObj 				///< ref to transaction object
								);
	
//interface-methods backward path
	void invalidate_direct_mem_ptr		(	sc_dt::uint64 start_range, 			///< start address of the memory-range
											sc_dt::uint64 end_range 			///< end address of the memory-range
										); 
										
	/// Required and unused virtual method
	tlm::tlm_sync_enum nb_transport_bw	(	tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay 			///< ref to delay time
										);

	void set_target_id					(	const unsigned int &id				///< const ref to target component ID
										);

// Variable and Object Declarations ============================================
	tlm::tlm_initiator_socket<buswidth2>		m_isocket;						///< initiator socket
	tlm::tlm_target_socket<buswidth3>			m_tsocket;						///< target socket
private:
	const unsigned int				m_id;										///< monitor id
	std::string						m_name;										///< module name
	unsigned int					m_see_report_dl, m_lm_report_dl;			///< data lengths
	tlm::tlm_generic_payload		m_tObj;										///< transaction object
	tlm_utils::tlm_quantumkeeper	m_qk1, m_qk2;								///< quantum keepers
	sc_core::sc_time				m_see_delay, m_lm_delay;
	memory							m_mem;										///< memory object
	unsigned int					nodenbr;

	std::string	m_notifcount_file;
};
#endif