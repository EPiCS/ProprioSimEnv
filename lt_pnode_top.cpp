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

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <iomanip>
#include <systemc.h>
#include <cmath>

#include "lt_pnode_top.h"
#include "constants.h"
#include "reporting.h"
#include "key_functions.h"


static const char	*filename = "lt_pnode_top.cpp";		///< filename for reporting

//======================================================================
/// @fn lt_pnode_top
//
/// @brief constructor
//
///	@details It instantiates objects in his initiialisation list. Each 
///		object respresents a component of a proprioceptive node. <br>
///		The instantiated objects are then connected with each other using  
///		socket bindings to build a proprioceptive node.
//======================================================================
lt_pnode_top::lt_pnode_top(	sc_module_name		name_													///< sc module name
						,	unsigned int		ID_														///< Knoten ID
						,	unsigned int		sensorenv_number_										///< number of sensorsenv
						,	unsigned int		othernodes_in_number_									///< number of connected nodes for data input
						,	unsigned int		othernodes_out_number_									///< number of connected nodes for data output
						,	unsigned int		actuator_number_										///< number of actuators
						,	unsigned int		extaction_number_										///< number of actuators
						,	unsigned int		ic1initiator_data_length_								///< max length of sensor data
						,	unsigned int		gvoc_monitor_data_length_								///< max length of gvoc data for the monitor
						,	unsigned int		gvoc_see_data_length_									///< max length of gvoc data for the see
						,	unsigned int		lm_report_data_length_									///< max length for lm result data
						,	unsigned int		lm_result_data_length_									///< max length for lm result data
						,	unsigned int		see_report_data_length_									///< max length for see report data
						,	unsigned int		see_actions_data_length_								///< max length of see actions data 
						,	unsigned int		ic1initiator_nr_dataset_cycle_							///< number of sensor dataset per process cycle
						,	unsigned int		lmresult_nr_dataset_cycle_								///< number of LM result-dataset per process cycle
						,	unsigned int		seeresult_nr_dataset_cycle_								///< number of see actions-dataset per process cycle
						,	unsigned int		gvocm_nr_dataset_cycle_									///< number of gvoc monitor-dataset per process cycle
						,	unsigned int		gvocsee_nr_dataset_cycle_								///< number of gvoc see-dataset per process cycle
						,	sc_core::sc_time	globales_quantum_										///< time value of the global quantum for all processes
						,	unsigned int		cycle_trigger_nr_										///< process cycles interval to trigger the monitor
						,	unsigned int		infifos_size_											///< size of the fifo channel for incoming sensor data from the files
						,	unsigned int		outfifos_size_											///< size of the fifo channel for outgoing actuator data
						,	unsigned int		nodenbr_												///< Position index for the vectoors of events
						):
	  node_id
		( ID_
		)
	, sensorsenv
		( "SENSORENV")
	, sensorenv_number
		( sensorenv_number_
		)
	, othernodes_in_number
		( othernodes_in_number_
		)
	, othernodes_out_number
		( othernodes_out_number_
		)
	, actuators
		( "ACTUATOR"	)
	, extactions
		( "EXT_ACTION"	)
	, the_others
		( "OTHER_NODE"	)
	, node_infifos
		("IN_FIFO")
	, node_outfifos
		("OUT_FIFO")
	, actuator_number
		( actuator_number_ )
	, ic1initiator_number
		( othernodes_in_number + sensorenv_number)
	, ic4target_number
		( othernodes_out_number + actuator_number )
	, ic1initiator_data_length
		( ic1initiator_data_length_ )
	, gvoc_monitor_data_length
		( gvoc_monitor_data_length_ )
	, gvoc_see_data_length
		( gvoc_see_data_length_ 	)
	, lm_report_data_length
		( lm_report_data_length_ 	)
	, lm_result_data_length
		( lm_result_data_length_ 	)
	, see_report_data_length
		( see_report_data_length_ 	)
	, see_actions_data_length
		( see_actions_data_length_ 	)
	, glob_quantum
		( globales_quantum_ 		)
	, sae_write_latency
		( key_parameters::get_sae_wl(glob_quantum, ic1initiator_data_length_, buswidth1/8, ic1initiator_nr_dataset_cycle_) 
		)
	, sae_read_latency
		( key_parameters::get_sae_rl(sae_write_latency, ic1initiator_number)
		)
	, monitor_write_latency
		( key_parameters::get_monitor_wl(glob_quantum, gvoc_monitor_data_length_, buswidth3/8, gvocm_nr_dataset_cycle_)
		)
	, lm_read_latency
		( key_parameters::get_lm_reportmem_rl(glob_quantum, lm_report_data_length_, buswidth2/8, ic1initiator_nr_dataset_cycle_, ic1initiator_number)
		)
	, see_lm_write_latency
		( key_parameters::get_see_lm_wl(glob_quantum, lm_result_data_length_ , buswidth5/8, ic1initiator_nr_dataset_cycle_, ic1initiator_number)
		)
	, see_gvoc_write_latency
		( key_parameters::get_see_gvoc_wl(glob_quantum, gvoc_see_data_length_, buswidth3/8, gvocsee_nr_dataset_cycle_)
		)
	, see_report_read_latency
		( key_parameters::get_see_reportmem_rl(glob_quantum, see_report_data_length_, buswidth2/8, ic1initiator_nr_dataset_cycle_, ic1initiator_number)
		)
	, actuator_write_latency
		( key_parameters::get_act_wl(glob_quantum, see_actions_data_length_, buswidth4/8, ic1initiator_nr_dataset_cycle_, ic1initiator_number)
		)
	, extaction_write_latency
		( key_parameters::get_act_wl(glob_quantum, see_actions_data_length_, buswidth4/8, ic1initiator_nr_dataset_cycle_, ic1initiator_number)
		)
	, sae_memory_size
		( key_parameters::get_sae_memsize(ic1initiator_nr_dataset_cycle_, ic1initiator_number, ic1initiator_data_length)
		)
	, monitor_memory_size
		( key_parameters::get_monitor_gvoc_memsize(gvoc_monitor_data_length_, gvocm_nr_dataset_cycle_)
		)
	, actuator_memory_size
		( key_parameters::get_actuator_memsize(see_actions_data_length_, seeresult_nr_dataset_cycle_)
		)
	, extaction_memory_size
		( key_parameters::get_actuator_memsize(see_actions_data_length_, seeresult_nr_dataset_cycle_)
		)
	, lm_reportmemory_size
		( key_parameters::get_lm_rep_memsize(lm_report_data_length_)
		)
	, see_reportmemory_size
		( key_parameters::get_see_rep_memsize(see_report_data_length_)
		)
	, see_lmmemory_size
		( key_parameters::get_see_lm_memsize(lm_result_data_length_)
		)
	, see_gvocmemory_size
		( key_parameters::get_see_gvoc_memsize(gvoc_see_data_length_, gvocsee_nr_dataset_cycle_)
		)
	, ic1
		( "IC1"
		, ic1initiator_number				
		, sae_memory_size
		)
	, ic2
		( "IC2"
		)
	, ic3
		( "IC3"
		)
	, ic4
		( "IC4"
		, ic4target_number
		)
	, gvoc
		( "GVOC"
		, gvoc_id_nr
		, gvoc_see_data_length_
		, gvoc_monitor_data_length_
		, gvocsee_nr_dataset_cycle_
		, gvocm_nr_dataset_cycle_
		, glob_quantum
		)
	, lmodel
		( "LMODEL"
		, lmodel_id_nr
		, lm_reportmemory_size
		, (buswidth2/8)										
		, lm_result_data_length_								
		, lm_report_data_length_
		, ic1initiator_data_length
		, lm_read_latency
		, lm_read_latency
		, sae_read_latency
		, see_lm_write_latency
		, cycle_trigger_nr_
		, glob_quantum
		, 0
		)
	, sae
		("SAE"
		, sae_id_nr
		, sae_memory_size
		, (buswidth1/8)
		, sae_read_latency
		, sae_write_latency
		)
	, monitor
		( "MONITOR"
		, monitor_id_nr
		, monitor_memory_size
		, (buswidth3/8)
		, lm_report_data_length_
		, see_report_data_length_
		, monitor_write_latency
		, monitor_write_latency
		, lm_read_latency
		, see_report_read_latency
		, glob_quantum
		, nodenbr_
		)
	, see
		( "SEE"
		, see_id_nr
		, see_lmmemory_size
		, see_gvocmemory_size
		, see_reportmemory_size
		, see_report_data_length_
		, see_actions_data_length_
		, (buswidth5/8)
		, (buswidth3/8)
		, (buswidth2/8)
		, see_report_read_latency
		, see_gvoc_write_latency
		, see_lm_write_latency
		, cycle_trigger_nr_
		, glob_quantum
		, 0
		)
{
	//create sensorsenv
	sensorsenv.init	(sensorenv_number, create_sensor_obj(sensor_id_nr, ic1initiator_data_length, ic1initiator_nr_dataset_cycle_, glob_quantum, infifos_size_)); 
	
	//create actuators
	actuators.init	(actuator_number, create_actuator_obj(act_id_nr, actuator_memory_size, (buswidth4/8), actuator_write_latency, actuator_write_latency, globales_quantum_));

	//create othernodes
	the_others.init	(othernodes_in_number, create_othernode_obj(othernodes_id_nr, ic1initiator_data_length, ic1initiator_nr_dataset_cycle_, glob_quantum) );	

	//create othernodes
	extactions.init	(othernodes_out_number, create_extaction_obj(extaction_id_nr, extaction_memory_size, (buswidth4/8), extaction_write_latency, extaction_write_latency) );	


	//multiports for data input from sensors and env
	node_infifos.init(sensorenv_number, create_fifo_obj(infifos_size_));
	othernodes_inports.init(othernodes_in_number);
	node_outfifos.init(othernodes_out_number);

	SC_THREAD(pn_thread);

	//bindings-operations
	//binds initiator socket of each sensor to the target socket of a ic1
	for (int i= 0; i<sensorenv_number; i++)
	{
		sensorsenv[i].s_isocket.bind( ic1.ic1_tsocket_array[i] );
	}

	for (int i= 0; i<othernodes_in_number; i++)
	{
		the_others[i].on_isocket.bind( ic1.ic1_tsocket_array[i+sensorenv_number] );
	}

	//binds initiator socket of ic1 to the target socket of t1
	ic1.ic1_isocket.bind	( sae.sae_tsocket_array[0]	);

	//binds initiator socket of gvoc to the target socket of monitor
	gvoc.gvoc_isocket.bind	( ic3.ic3_tsocket	);

	//binds initiator sockets of ic3 to the target socket of monitor and see
	ic3.ic3_isocket_array[0].bind	( monitor.m_tsocket		);
	ic3.ic3_isocket_array[1].bind	( see.see_gvoc_tsocket	);

	//binds initiator socket of monitor to the target socket of ic2
	monitor.m_isocket.bind	( ic2.ic2_tsocket	);

	//binds initiator sockets of ic2 to the target socket of sae and see
	ic2.ic2_isocket_array[0].bind	( lmodel.lm_m_tsocket		);
	ic2.ic2_isocket_array[1].bind	( see.see_monitor_tsocket	); 

	//binds initiator socket of lmodel to the target socket of sae and see
	lmodel.lm_sae_isocket.bind	( sae.sae_tsocket_array[1]);
	lmodel.lm_see_isocket.bind	( see.see_lmodel_tsocket); 

	//binds initiator socket of see to the target socket of ic4
	see.see_isocket.bind		( ic4.ic4_tsocket	);

	//binds initiator socket of ic4 to the target socket of act und ext act
	for (int i= 0; i<actuator_number; i++)
	{
		ic4.ic4_isocket_array[i].bind	( actuators[i].act_tsocket );
	}
	for (int i= 0; i<othernodes_out_number; i++)
	{
		ic4.ic4_isocket_array[i+actuator_number].bind	( extactions[i].extact_tsocket );
	}

	//Port bindings
	for (int i = 0; i < sensorenv_number; i++)
	{
		sensorsenv[i].senv_data_inport(node_infifos[i]);
	}
	for (int i = 0; i < othernodes_in_number; i++)
	{
		the_others[i].on_data_inport(othernodes_inports[i]);
	}
	for (int i = 0; i < othernodes_out_number; i++)
	{
		extactions[i].extact_outport(node_outfifos[i]);
	}

	print_keyparameters();

	node_qk.set_global_quantum (glob_quantum);	
	node_qk.reset();
	
}


//======================================================================
/// @fn pn_thread
//
/// @brief Thread process F of a node.
//
/// @details It is the synchronization process of a node. It is the last one
///		to be run in each process cycle. It doesn't generate any transactions 
///		and always syncrhonizes immediately after it is resumed.
//======================================================================
void lt_pnode_top::pn_thread(void)
{
	std::ostringstream  msg;
	wait ( sc_core::sc_time(0.5, node_time_unit) );
	while(true)
	{
		node_delay = node_qk.get_local_time();

		msg.str ("");
		msg << "\t" << name() << "\t-\t PROZESS NODE:";

		node_delay = glob_quantum;

		node_qk.set( node_delay );
 		wait(sc_core::SC_ZERO_TIME);

		if ( node_qk.need_sync() ) 					// Check local time against quantum and sync if neccessary
		{
			trtr::follow_synch(filename, name(), "NODE", node_id, node_delay, trtr::S_NEED);
			node_qk.sync();
			trtr::follow_synch(filename, name(), "NODE", node_id, node_delay, trtr::S_RETURN);
		}
		else
		{
			msg << "\t delay:" << node_delay;
			INFO_LOG(filename, __FUNCTION__ , msg.str());
		}	
	}
}


//======================================================================
/// @fn print_keyparameters
//
/// @brief utilities function.
//
/// @details It prints the key parameters of the node calculated during elaboration
///		before the simualtion start.
//======================================================================
void lt_pnode_top::print_keyparameters(void)
{
	std::ostringstream  msg;

	msg.str("");
	msg << "\t Node Name					:	" << name()<< endl
		<< "\t GLOBAL QUANTUM				=	" << glob_quantum				<< endl << endl

		<< "\t MONITOR-GVOC WRITE LATENCY	=	" << monitor_write_latency		<< endl
		<< "\t SEE-GVOC WRITE LATENCY		=	" << see_gvoc_write_latency		<< endl
		<< "\t SAE WRITE LATENCY			=	" << sae_write_latency			<< endl
		<< "\t SAE READ LATENCY			=	" << sae_read_latency			<< endl
		<< "\t LM READ LATENCY			=	" << lm_read_latency			<< endl
		<< "\t SEE-LM WRITE LATENCY		=	" << see_lm_write_latency		<< endl
		<< "\t SEE READ LATENCY			=	" << see_report_read_latency	<< endl
		<< "\t ACTUATOR WRITE LATENCY		=	" << actuator_write_latency		<< endl
		<< "\t EXTACTION WRITE LATENCY	=	" << extaction_write_latency	<< endl <<endl

		<< "\t SAE MEMORY SIZE			=	" << sae_memory_size			<< endl
		<< "\t MONITOR MEMORY SIZE		=	" << monitor_memory_size		<< endl
		<< "\t SEE GVOC-MEMORY SIZE		=	" << see_gvocmemory_size		<< endl
		<< "\t ACT MEMORY SIZE			=	" << actuator_memory_size		<< endl
		<< "\t EXTA MEMORY SIZE			=	" << extaction_memory_size		<< endl
		<< "\t LM REPORT-MEMORY SIZE		=	" << lm_reportmemory_size		<< endl
		<< "\t SEE REPORT-MEMORY SIZE		=	" << see_reportmemory_size		<< endl
		<< "\t SEE LM-MEMORY SIZE			=	" << see_lmmemory_size
		
		;
	INFO_LOG(filename, __FUNCTION__ , msg.str());
}