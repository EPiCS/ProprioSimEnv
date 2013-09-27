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

//========================================================================================
///  @file lt_pnode_top.h
//
///  @brief This is the module node, the highest level in the model hierarchy.
//
///  @details It is responsible for the generation of the number of sensors and 
///		actuators specified by the user and for the necessary socket and port bindings between all
///		the TLM components of a proprioceptive node. <br>
///		Before grouping, it first make sure that the choosen values of constructor variables
///		are in the given limits. If this is not the case an error message is thrown and the 
///		simulation is stopped.
//
//========================================================================================

#ifndef __lt_PNODE_TOP_H__
#define __lt_PNODE_TOP_H__

#include <tlm.h>
#include <systemc.h>
#include <tlm_utils\tlm_quantumkeeper.h>

#include "initiator_othernode.h"
#include "initiator_sensorenv.h"
#include "initiator_gvoc.h"
#include "target_sae.h"
#include "target_actuator.h"
#include "target_extaction.h"
#include "interconnect1.h"
#include "interconnect2.h"
#include "interconnect3.h"
#include "interconnect4.h"
#include "bridge_lmodel.h"
#include "bridge_monitor.h"
#include "bridge_see.h"
#include "constants.h"


class lt_pnode_top: public sc_module
{
public:
	lt_pnode_top(	sc_module_name		name_													///< sc module name
				,	unsigned int		ID_														///< Knoten ID
				,	unsigned int		sensorenv_number_										///< number of sensors and environment components
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
				);

public:
	SC_HAS_PROCESS(lt_pnode_top);

	void pn_thread( void )								;
	void print_keyparameters(void)						; 

private:
	unsigned int			node_id						;

	//komponentenanzahl
	unsigned int			sensorenv_number			;
	unsigned int			othernodes_in_number		;
	unsigned int			othernodes_out_number		;
	unsigned int			actuator_number				;
	unsigned int			ic1initiator_number			;
	unsigned int			ic4target_number			;

	//datenlänge im Modell
	unsigned int			ic1initiator_data_length	;
	unsigned int			gvoc_monitor_data_length	;
	unsigned int			gvoc_see_data_length		;
	unsigned int			lm_report_data_length		;
	unsigned int			lm_result_data_length		;
	unsigned int			see_report_data_length		;
	unsigned int			see_actions_data_length		;
	
	//Quantum
	sc_core::sc_time		glob_quantum				;
	sc_core::sc_time		node_delay					;
	tlm_utils::tlm_quantumkeeper	node_qk				;

	//latenzzeiten im Modell
	sc_core::sc_time		sae_write_latency			;
	sc_core::sc_time		sae_read_latency			;
	sc_core::sc_time		monitor_write_latency		;
	sc_core::sc_time		lm_read_latency				;
	sc_core::sc_time		see_lm_write_latency		;
	sc_core::sc_time		see_gvoc_write_latency		;
	sc_core::sc_time		see_report_read_latency		;
	sc_core::sc_time		actuator_write_latency		;
	sc_core::sc_time		extaction_write_latency		;

	//memory sizes
	unsigned int			sae_memory_size				;
	unsigned int			monitor_memory_size			;
	unsigned int			actuator_memory_size		;
	unsigned int			extaction_memory_size		;
	unsigned int			lm_reportmemory_size		;
	unsigned int			see_reportmemory_size		;
	unsigned int			see_lmmemory_size			;
	unsigned int			see_gvocmemory_size			;

	initiator_gvoc			gvoc						;
	sc_core::sc_vector<initiator_sensorenv> sensorsenv	;
	sc_core::sc_vector<initiator_othernode> the_others	;
	sc_core::sc_vector<target_actuator> actuators		;
	sc_core::sc_vector<target_extaction> extactions		;
	target_sae				sae							;
	interconnect1			ic1							;
	interconnect2			ic2							;
	interconnect3			ic3							;
	interconnect4			ic4							;
	bridge_lmodel			lmodel						;
	bridge_monitor			monitor						;
	bridge_see				see							;


public:
	//For Data from sensors and environment
	sc_core::sc_vector< sc_core::sc_fifo< datyp > >									node_infifos		;

	//For Data to other nodes 
	sc_core::sc_vector< sc_core::sc_fifo< sc_dt::uchar> >							node_outfifos		;

	//For Data from other nodes
	sc_core::sc_vector< sc_core::sc_port< sc_core::sc_fifo_in_if<sc_dt::uchar> >  >	othernodes_inports	;

	//======================================================================
	/// @struct create_sensor_obj
	//
	/// @brief creates sensor objects
	//
	//======================================================================
	struct create_sensor_obj
	{
		create_sensor_obj(unsigned int id, unsigned int dl, unsigned int dset_nr, sc_core::sc_time gq, unsigned int fs)
			:cid(id), clength(dl), cgq(gq), cds_nr(dset_nr), cfs(fs){}

		initiator_sensorenv* operator()(const char* name, size_t)
		{
			return new initiator_sensorenv(name, cid, clength, cds_nr, cgq, cfs);
		}
		unsigned int cid, clength, cds_nr, cfs	;
		sc_core::sc_time cgq					;
	};

//======================================================================
/// @struct create_actuator_obj
//
/// @brief creates actuator objects
//
//======================================================================
	struct create_actuator_obj
	{
		create_actuator_obj(unsigned int id, unsigned int dl, unsigned int wd, sc_core::sc_time readlat, sc_core::sc_time writelat, sc_core::sc_time gq)
			:cid(id), csize(dl), cwidth(wd), cwritelat(writelat), creadlat(readlat), cgq(gq){}

		target_actuator* operator()(const char* name, size_t)
		{
			return new target_actuator(name, cid, csize, cwidth, creadlat, cwritelat, cgq);
		}
		unsigned int cid, csize, cwidth				;
		sc_core::sc_time cwritelat, creadlat, cgq	;
	};

//======================================================================
/// @struct create_othernode_obj
//
/// @brief creates othernode objects
//
//======================================================================
	struct create_othernode_obj
	{
		create_othernode_obj(unsigned int id, unsigned int dl, unsigned int dset_nr,  sc_core::sc_time gquantum)
			:cid(id), csize(dl), cquantum(gquantum), cds_nr(dset_nr){}

		initiator_othernode* operator()(const char* name, size_t)
		{
			return new initiator_othernode(name, cid, csize, cds_nr , cquantum);
		}
		unsigned int cid, csize, cds_nr	;
		sc_core::sc_time cquantum		;
	};


//======================================================================
/// @struct create_extaction_obj
//
/// @brief creates extaction objects
//
//======================================================================
	struct create_extaction_obj
	{
		create_extaction_obj(unsigned int id, unsigned int dl, unsigned int wd, sc_core::sc_time readlat, sc_core::sc_time writelat)
			:cid(id), csize(dl), cwidth(wd), cwritelat(writelat), creadlat(readlat){}

		target_extaction* operator()(const char* name, size_t)
		{
			return new target_extaction(name, cid, csize, cwidth, creadlat, cwritelat);
		}
		unsigned int cid, csize, cwidth		;
		sc_core::sc_time cwritelat, creadlat;
	};

//======================================================================
/// @struct create_fifo_obj
//
/// @brief creates fifo objects
//
//======================================================================
	struct create_fifo_obj
	{
		create_fifo_obj(int size)
			:csize(size){}

		sc_fifo<datyp>* operator()(const char* name, size_t)
		{
			return new sc_fifo<datyp>(name, csize);
		}
		int csize;
	};

};

#endif /* __lt_pnode_top_H__ */