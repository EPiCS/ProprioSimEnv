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
///  @file initiator_lmodel.h
//
///  @brief This is the part of the private and public self-aware node which 
///		processes the information collected from the sensors and also the 
///		Model(s) component of the proprioceptive Node - abbr.: LM
///
///  @details It readouts dataset from the SAE, evaluates them and send some data 
///		to the SEE component as result to the data evaluation or input for the 
///		decision making. <br>
///		Its Role: Brigde <br>
///		Its Targets: SAE and SEE <br>
///		Its Initiator: GVOC <br>
///		Its Processes: C1 and C2
//
//==============================================================================

#ifndef __BRIDGE_LMODEL_H__
#define __BRIDGE_LMODEL_H__

#include <systemc.h>
#include "tlm.h"                                    // TLM headers
#include <tlm_utils\tlm_quantumkeeper.h>

#include "memory.h"
#include "constants.h"
#include "lm_core.h"


class bridge_lmodel:	public sc_module, 
						virtual public tlm::tlm_bw_transport_if<>,
						virtual public tlm::tlm_fw_transport_if<>
{

public:
// Constructor ================================================================= 
	bridge_lmodel(	  sc_module_name module_name								///< sc module name
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
					);
	
	SC_HAS_PROCESS(bridge_lmodel);


// Method Declarations =========================================================
	
	void lmodel_sae_thread(void);

	void lmodel_see_thread(void);

	void set_trans_Obj	( 	tlm::tlm_generic_payload&, 							///< ref to transaction object
							tlm::tlm_command cmd, 								///< transaction command (read or write)
							const unsigned int &target_id,						///< const ref to target component ID
							unsigned char* data,								///< transaction data
							sc_dt::uint64 adr									///< transaction start address
						);

	void execute_trans	( 	tlm::tlm_generic_payload&, 							///< ref to transaction object 
							const unsigned int &target_id						///< const ref to target component ID
						);

	void do_report(void);
	  
// interface Methods - forward path
	void b_transport		(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								sc_core::sc_time& delay		 					///< ref to time delay
							);

	bool get_direct_mem_ptr	(	tlm::tlm_generic_payload& tObj, 				///< ref to transaction object
								tlm::tlm_dmi& dmi_data							///< ref to dmi descriptor
							);

	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_fw	(	tlm::tlm_generic_payload& tObj,		///< ref to transaction object 
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay 			///< ref to time delay
										);

	unsigned int transport_dbg		(	tlm::tlm_generic_payload& tObj			///< ref to transaction object 
									);

// interface Methods - backward path
	void invalidate_direct_mem_ptr	( 	sc_dt::uint64 start_range, 				///< start address of the memory range
										sc_dt::uint64 end_range					///< end address of the memory range
									);  

	/// Required and unused interface method
	tlm::tlm_sync_enum nb_transport_bw	( 	tlm::tlm_generic_payload& tObj, 	///< ref to transaction object
											tlm::tlm_phase& phase, 				///< ref to transaction phase
											sc_core::sc_time& delay 			///< ref to time delay
										);
	


// Variable and Object Declarations ============================================
	tlm::tlm_initiator_socket<buswidth5>	lm_see_isocket;						///< initiator socket for communication with SEE
	tlm::tlm_initiator_socket<buswidth1>	lm_sae_isocket;						///< initiator socket for communication with SAE
	tlm::tlm_target_socket<buswidth2>		lm_m_tsocket;						///< target socket for communication with monitor

private:
	memory							lm_repmem;
	unsigned int					lm_id;										///< component id
	std::string 					lm_name;									///< module name
	tlm::tlm_generic_payload		lm_sae_tObj, lm_see_tObj;					///< transaction objects
	unsigned int					lm_sae_dl, lm_res_dl, lm_rep_dl;			///< data lengths						
	sc_core::sc_time				lm_sae_delay, lm_see_delay;					///< time delay temporal decoupling							
	sc_dt::uint64					sae_adr_start, see_adr_start;				///< transaction start adresses
	unsigned int					lm_target_id;								///< transaction target
	tlm_utils::tlm_quantumkeeper	lm_qk1, lm_qk2;								///< quantum keepers
	sc_dt::uchar					*lm_res_data, *lm_sae_data;					///< pointers
	unsigned int					m_cycle_nr_trigger;							///< user specification with which the monitor component

	void set_target_id				( const unsigned int &id ); 				///   defines the target component of the transaction by setting #lm_target_id.
	lm_core							*lm_core_obj;								

	unsigned int					nodenbr;
	std::string						lm_notifcount_file;
};

#endif /* __INITIATOR_lmodel_H__ */