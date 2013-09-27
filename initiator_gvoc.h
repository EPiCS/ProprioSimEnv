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
///  @file initiator_gvoc.h
//
///  @brief This the Goals-Values-Objectives-Constants component of 
///			a proprioceptive Node
//
///  @details It transfers the g.v.o.c. data to the self-expressive engine and the 
///		the monitor of the node. <br>
///		Its Targets: Monitor and SEE through the IC3 component
// 
//==============================================================================

#ifndef __INITIATOR_GVOC_H__
#define __INITIATOR_GVOC_H__

#include <systemc.h>
#include <tlm.h>  
#include <tlm_utils\tlm_quantumkeeper.h>

#include "constants.h"

class initiator_gvoc :	public sc_module,
						virtual public tlm::tlm_bw_transport_if<>{
public:
// Constructor ================================================================= 
	initiator_gvoc( 
					sc_module_name		module_name									///< sc module name
				  , const unsigned int	ID											///< component ID
				  , const unsigned int	see_data_length								///< maximal data length for see data
				  , const unsigned int	monitor_data_length							///< maximal data length for monitor data
				  , const unsigned int	see_nr_dataset_cycle						///< number of gvoc datasets pro cycle for the see component
				  , const unsigned int	monitor_nr_dataset_cycle					///< number of gvoc datasets pro cycle for the monitor component
				  , sc_core::sc_time	glob_quantum								///< time value for the global quantum
				  );

	SC_HAS_PROCESS(initiator_gvoc);

// Method Declarations =========================================================
	void gvoc_see_thread( void );													
	void gvoc_monitor_thread( void );
	
	void execute_trans	( 	tlm::tlm_generic_payload& tObj, 						///< ref to transaction object
							const unsigned int &target_id,							///< const ref to target component ID
							sc_core::sc_time &delay
						);

	void set_trans_Obj	(	tlm::tlm_generic_payload& tObj,							///< ref to transaction object
							const sc_dt::uint64 &adr,								///< cont ref to transaction start address
							unsigned char* data										///< pointer to transaction data
						);

//interface methods

	void invalidate_direct_mem_ptr	(	sc_dt::uint64 start_range,  				///< start address of the memory range
										sc_dt::uint64 end_range						///< end address of the memory range
									);
									
	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_bw	( 	tlm::tlm_generic_payload& tObj, 		///< ref to transaction object
											tlm::tlm_phase& phase, 					///< ref to transaction phase
											sc_core::sc_time& delay 				///< ref to delay time 
										);

	void prepare_datablock(void);

// Variable and Object Declarations ============================================
	tlm::tlm_initiator_socket<buswidth3> gvoc_isocket;								///< initiator socket for communication with iC3
	
private:
	std::string						gvoc_name					;					///< sc module name
	unsigned int					gvoc_id						;					///< component id
	tlm::tlm_generic_payload		gvoc_tObj					;					///< transaction object
	unsigned int					gvoc_see_data_length, gvoc_see_dataset_cycle	;	///< data length, number of dataset pro cycle to be send to the SEE component
	unsigned int					gvoc_monitor_data_length, gvoc_m_dataset_cycle	;	///< data length, number of dataset pro cycle to be send to the Monitor component
	sc_core::sc_time				gvoc_m_delay, gvoc_see_delay;					///< transaction time delays
	sc_dt::uint64					gvoc_see_adr_start, gvoc_m_adr_start;			///< transaction start adresses
	tlm_utils::tlm_quantumkeeper	gvoc_see_qk, gvoc_m_qk		;					///< quantum keeper
	sc_dt::uchar*					gvoc_data					;					///< data pointer
	
	void set_target_idx	(const unsigned int &id)				;					/// defines the target component of the transaction by setting #gvoc_target_idx				
};

#endif /* __INITIATOR_GVOC_H__ */