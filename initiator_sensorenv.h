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
///  @file initiator_sensorenv.h
//
///  @brief This is the sensor component of a proprioceptive node, either the
///		internal sensor or the 
//
///  @details
///		It writes dataset into the SAE memory by performing blocking transport calls.<br>
///		Its role: Initiator <br>
///		His target: SAE through the interconnect component IC1 <br>
///		Its process: B
//		
//==============================================================================

#ifndef __INITIATOR_SENSORENV_H__
#define __INITIATOR_SENSORENV_H__

#include <systemc.h>
#include "tlm.h"																// TLM headers
#include <tlm_utils\tlm_quantumkeeper.h>
#include <queue>

#include "constants.h"

class initiator_sensorenv:	public sc_module, 
							virtual public tlm::tlm_bw_transport_if<>{

public:	
// Constructor =================================================================
	initiator_sensorenv( 	sc_module_name		module_name							///< sc module name
						, 	const unsigned		int ID								///< component ID
						, 	unsigned int		data_length							///< transaction data length
						,	unsigned int		nr_dataset_cycle					///< number of sensor datasets pro cycle
						,	sc_core::sc_time	glob_quantum						///< time value for the global quantum
						,	unsigned int		fifo_size							///< size of the fifo channel bound to its input port
						);

	/// 
	SC_HAS_PROCESS(initiator_sensorenv);


// Method Declarations =========================================================
	void sensor_thread	( void );
	
	void execute_trans	(	tlm::tlm_generic_payload& tObj						///< ref transaction object
						);
		
	void set_trans_Obj	(	tlm::tlm_generic_payload& tObj,						///< ref transaction object
							const sc_dt::uint64 &adr,							///< const ref to transaction data
							unsigned char* data									///< pointer to transation data
						);
	
// interface methods
	void invalidate_direct_mem_ptr	( 	sc_dt::uint64 start_range, 				///< start address of the memory range
										sc_dt::uint64 end_range					///< end address of the memory range
									);  

/// Required und unused interface method
	tlm::tlm_sync_enum nb_transport_bw 	( 	tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay 			///< ref to time delay
										);
									
	//void prepare_datablock(void);
	
	void read_out_fifo();

// Variable and Object Declarations ============================================
	tlm::tlm_initiator_socket<buswidth1>			s_isocket		;			///< initiator socket for communication with IC1
	sc_core::sc_port< sc_fifo_in_if	<datyp>	>		senv_data_inport;			///< input port bounded to the fifo channel in the parent module termed node
	

private:
	int								s_id					;					///< component id
	std::string						s_name					;					///< component name
	tlm::tlm_generic_payload		s_tObj					;					///< transaction object
	unsigned int					s_data_length, s_dataset_cycle;				///< transaction data llength, number of dataset pro cycle which will be sent to the SAE component
	sc_core::sc_time				s_delay					;					///< transaction time delay
	sc_dt::uint64					s_mem_adr_start			;					///< transaction start address
	tlm_utils::tlm_quantumkeeper	s_qk					;					///< quantum keeper
	sc_dt::uchar					senv_data				;					///< data pointer
	unsigned int 					s_fifo_size				;					///< fifo size
	std::queue< sc_dt::uchar, std::deque<sc_dt::uchar> > senv_queue;				///< data container
	std::string						senv_dataoutput_file	;

};

#endif /* __INITIATOR_SENSORENV_H__ */