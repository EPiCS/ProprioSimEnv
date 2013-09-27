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
///  @file key_functions.h
//
///  @brief This contents the functions used to compute the latency times and the 
//		sizes of the memory blocks needed in the model.
//
//
//==============================================================================

#include "key_functions.h"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace key_parameters
{	
	//Memory Sizes
//======================================================================
/// @fn get_see_gvoc_memsize
//
/// @brief determines the see's memory size for g.v.o.c. data
//======================================================================
	unsigned int get_see_gvoc_memsize	( const unsigned int& gvoc_dl
										, const unsigned int& anz_data_perzyklus)
	{
		return gvoc_dl*anz_data_perzyklus;
	}


//======================================================================
/// @fn get_sae_memsize
//
/// @brief determines the sae's memory size
//======================================================================
	unsigned int get_sae_memsize( const unsigned int& anz_data_perzyklus
								, const unsigned int& anz_ic1_initiatoren
								, const unsigned int& ic1_initiator_max_dl
								)
	{
		return anz_data_perzyklus * (anz_ic1_initiatoren) * ic1_initiator_max_dl;
	}


	unsigned int get_lm_sae_memsize(const unsigned int& ic1_initiator_max_dl)
	{
		return ic1_initiator_max_dl;
	}

	unsigned int get_lm_rep_memsize	( const unsigned int& lm_rep_dl )
	{
		return lm_rep_dl;
	}

//======================================================================
/// @fn get_see_lm_memsize
//
/// @brief determines the see's memory size for the lm data
//======================================================================
	unsigned int get_see_lm_memsize	( const unsigned int& lm_res_dl )
	{
		return lm_res_dl;
	}

//======================================================================
/// @fn get_see_rep_memsize
//
/// @brief determines the see's memory size for its report data
//======================================================================
	unsigned int get_see_rep_memsize( const unsigned int& see_rep_dl )
	{
		return see_rep_dl;
	}

//======================================================================
/// @fn get_monitor_gvoc_memsize
//
/// @brief determines the monitor's memory size for the g.v.o.c data
//======================================================================
	unsigned int get_monitor_gvoc_memsize	( const unsigned int& gvoc_dl
											, const unsigned int& anz_data_prozyklus)
	{
		return gvoc_dl*anz_data_prozyklus;
	}

//======================================================================
/// @fn get_actuator_memsize
//
/// @brief determines the actuator's memory size for the see data
//======================================================================
	unsigned int get_actuator_memsize( const unsigned int& see_actions_dl
									,  const unsigned int& anz_data_prozyklus)
	{
		return see_actions_dl*anz_data_prozyklus;
	}

//======================================================================
/// @fn get_extaction_memsize
//
/// @brief determines the extaction's memory size for the see data
//======================================================================
	unsigned int get_extaction_memsize	( const unsigned int& see_actions_dl
										, const unsigned int& anz_data_prozyklus)
	{
		return see_actions_dl*anz_data_prozyklus;
	}

	//other key parameters

	//sc_core::sc_time get_global_quantum	( const unsigned int& bus_width
	//									, const unsigned int& sensor_dl
	//									, const unsigned int& anz_data_perzyklus	//Anzahl der Prozessausführungen jedes sensorsenv in einem Zyklus
	//									, const sc_core::sc_time& sae_wl
	//									)
	//{
	//	double x = ceil((double)sensor_dl/bus_width);
	//	unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
	//	sc_core::sc_time glob_quantum;
	//	unsigned int anz_process_ausf_pro_zyklus = burst_length*anz_data_perzyklus;
	//	glob_quantum = sae_wl*anz_process_ausf_pro_zyklus;
	//	return glob_quantum;
	//}

//======================================================================
/// @fn get_sae_wl
//
/// @brief determines the write latency for a single write transaction on the sae
//======================================================================
	sc_core::sc_time get_sae_wl	( const sc_core::sc_time& glob_quantum
								, const unsigned int& sensor_dl
								, const unsigned int& bus_width
								, const unsigned int& anz_data_perzyklus
								)
	{
		double x = ceil((double)sensor_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus =anz_data_perzyklus*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}

//======================================================================
/// @fn get_sae_rl
//
/// @brief determines the read latency for a single read transaction on the sae
//======================================================================
	sc_core::sc_time get_sae_rl	( const sc_core::sc_time& sae_wl
								, const unsigned int& anz_ic1_initiatoren
								)
	{
		return sae_wl / (anz_ic1_initiatoren);
	}

//======================================================================
/// @fn get_see_gvoc_wl
//
/// @brief determines the write latency for a single write transaction initiated by the gvoc on the see
//======================================================================
	sc_core::sc_time get_see_gvoc_wl( const sc_core::sc_time& glob_quantum
									, const unsigned int& gvoc_dl
									, const unsigned int& bus_width
									, const unsigned int& anz_data_perzyklus
									)
	{
		double x = ceil((double)gvoc_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus =anz_data_perzyklus*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}

//======================================================================
/// @fn get_monitor_wl
//
/// @brief determines the write latency for a single write transaction on the monitor
//======================================================================
	sc_core::sc_time get_monitor_wl	( const sc_core::sc_time& glob_quantum
									, const unsigned int& gvoc_dl
									, const unsigned int& bus_width
									, const unsigned int& anz_data_perzyklus
									)
	{
		double x = ceil((double)gvoc_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus =anz_data_perzyklus*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}

//======================================================================
/// @fn get_lm_reportmem_rl
//
/// @brief determines the read latency for a single read transaction on the LM
//======================================================================
	sc_core::sc_time get_lm_reportmem_rl	( const sc_core::sc_time& glob_quantum
											, const unsigned int& lm_rep_dl
											, const unsigned int& bus_width
											, const unsigned int& anz_data_perzyklus
											, const unsigned int& anz_ic1_initiatoren
											)
	{
		double x = ceil((double)lm_rep_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus =anz_data_perzyklus*anz_ic1_initiatoren*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}

//======================================================================
/// @fn get_see_reportmem_rl
//
/// @brief determines the read latency for a single read transaction on the SEE
//======================================================================
	sc_core::sc_time get_see_reportmem_rl	( const sc_core::sc_time& glob_quantum
											, const unsigned int& see_rep_dl
											, const unsigned int& bus_width
											, const unsigned int& anz_data_perzyklus
											, const unsigned int& anz_ic1_initiatoren
											)
	{
		double x = ceil((double)see_rep_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus =anz_data_perzyklus*anz_ic1_initiatoren*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}

//======================================================================
/// @fn get_act_wl
//
/// @brief determines the write latency for a single write transaction on the actuator
//======================================================================
	sc_core::sc_time get_act_wl	( const sc_core::sc_time& glob_quantum
								, const unsigned int& see_act_dl
								, const unsigned int& bus_width
								, const unsigned int& anz_data_perzyklus
								, const unsigned int& anz_ic1_initiatoren
								)
	{
		double x = ceil((double)see_act_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus =anz_data_perzyklus*anz_ic1_initiatoren*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}


//======================================================================
/// @fn get_extact_wl
//
/// @brief determines the write latency for a single write transaction on the extaction
//======================================================================
	sc_core::sc_time get_extact_wl	( const sc_core::sc_time& glob_quantum
								, const unsigned int& see_act_dl
								, const unsigned int& bus_width
								, const unsigned int& anz_data_perzyklus
								, const unsigned int& anz_ic1_initiatoren
								)
	{
		double x = ceil((double)see_act_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus =anz_data_perzyklus*anz_ic1_initiatoren*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}

//======================================================================
/// @fn get_see_lm_wl
//
/// @brief determines the write latency for a single write transaction initiated by the LM on the see
//======================================================================
	sc_core::sc_time get_see_lm_wl( const sc_core::sc_time& glob_quantum
									, const unsigned int& lm_act_dl
									, const unsigned int& bus_width
									, const unsigned int& anz_data_perzyklus
									, const unsigned int& anz_ic1_initiatoren
									)
	{
		double x = ceil((double)lm_act_dl/bus_width);
		unsigned int burst_length = static_cast<unsigned int>(x + 0.5);
		unsigned int anz_process_ausf_pro_zyklus = anz_data_perzyklus*anz_ic1_initiatoren*burst_length; 
		return glob_quantum / (anz_process_ausf_pro_zyklus);
	}

}

namespace file_ops
{

	std::string build_filename	( std::string &name_prefix					///< prefix for the filename
								, const char* name_suffix					///< suffix for the filename
								)
	{
		std::string name (name_suffix);
		std::string undersc("_");
		//name = name_prefix + undersc + name;
		return name_prefix + undersc + name;
	}
}

