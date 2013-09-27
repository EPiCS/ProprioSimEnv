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
///  @brief This contains some useful functions
//
///  @details it contains functions for the calculation of the key parameters of the model,
///		for operation on files and for data conversion (from a numeric type in string and
///		vice-versa
///		
//
//==============================================================================
#ifndef __KEY_FUNCTIONS_H__
#define __KEY_FUNCTIONS_H__

#include <systemc.h>
#include "constants.h"
#include <tlm.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "reporting.h"

#pragma once

namespace key_parameters
{
	unsigned int get_see_gvoc_memsize(const unsigned int& gvoc_dl,								///< const ref to the gvoc datalength
									  const unsigned int& anz_data_perzyklus					///< const ref to the number of gvoc dataset to send in the node per process cycle
									 );

	unsigned int get_sae_memsize( const unsigned int& anz_data_perzyklus						///< const ref to number of sensor dataset per process cycle that each sensor component should send to sae
								, const unsigned int& anz_ic1_initiatoren						///< const ref to the number of sae initiators or rather senenv and othernode components
								, const unsigned int& ic1_initiator_max_dl						///< const ref to the maximal length of a sensor dataset
								);

	unsigned int get_lm_sae_memsize	(const unsigned int& ic1_initiator_max_dl					///< const ref to the maximal length of a sensor dataset
									);

	unsigned int get_lm_rep_memsize	( const unsigned int& lm_rep_dl								///< const ref to the length of the report data of LM
									);

	unsigned int get_see_lm_memsize	( const unsigned int& lm_res_dl								///< const ref to the length of the result data of LM							
									);

	unsigned int get_see_rep_memsize( const unsigned int& see_rep_dl							///< const ref to the length of the report data of SEE
									);

	unsigned int get_monitor_gvoc_memsize	( const unsigned int& gvoc_dl						///< const ref to the length of a gvoc dataset
											, const unsigned int& anz_data_prozyklus			///< const ref to the number of gvoc dataset to send in the node per process cycle
											);

	unsigned int get_actuator_memsize( const unsigned int& see_actions_dl						///< const ref to the length of the see choice data
									,  const unsigned int& anz_data_prozyklus					///< const ref to the number of a see dataset to be sent to the actuator component(s) per process cycle
									);

	unsigned int get_extaction_memsize	( const unsigned int& see_actions_dl					///< const ref to the length of the see choice data
										, const unsigned int& anz_data_prozyklus				///< const ref to the number of a see dataset to be sent to the extaction components per process cycle
										);

	//other key parameters
	//sc_core::sc_time get_global_quantum	( const unsigned int& bus_width
	//									, const unsigned int& sensor_dl
	//									, const unsigned int& anz_data_perzyklus	//Anzahl der Prozessausführungen jedes sensorsenv in einem Zyklus
	//									, const sc_core::sc_time& sae_wl
	//									);

	sc_core::sc_time get_sae_wl	( const sc_core::sc_time& glob_quantum							///< const ref to the global quantum time
								, const unsigned int& sensor_dl									///< const ref to the maximal length of a sensor dataset
								, const unsigned int& bus_width									///< const ref to the bus width #buswidth1
								, const unsigned int& anz_data_perzyklus						///< const ref to the number of sensor dataset to send the SAE per process cycle
								);


	sc_core::sc_time get_sae_rl	( const sc_core::sc_time& sae_wl								///< const ref to the latency for a single write transaction on SAE
								, const unsigned int& anz_ic1_initiatoren						///< const ref to the number of sae initiators or rather senenv and othernode components
								);

	sc_core::sc_time get_see_gvoc_wl( const sc_core::sc_time& glob_quantum						///< const ref to the global quantum time
									, const unsigned int& gvoc_dl								///< const ref to the length of the gvoc data for the SEE component
									, const unsigned int& bus_width								///< const ref to the bus width #buswidth3
									, const unsigned int& anz_data_perzyklus					///< const ref to the number of dataset per process cycle that the gvoc component must send per process cycle to the SEE
									);

	sc_core::sc_time get_monitor_wl	( const sc_core::sc_time& glob_quantum						///< const ref to the global quantum time
									, const unsigned int& gvoc_dl								///< const ref to the length of the gvoc data for the monitor component
									, const unsigned int& bus_width								///< const ref to the bus width #buswidth3
									, const unsigned int& anz_data_perzyklus					///< const ref to the number of dataset per process cycle that the gvoc component must send per process cycle to the monitor
									);

	sc_core::sc_time get_lm_reportmem_rl	( const sc_core::sc_time& glob_quantum				///< const ref to the global quantum time
											, const unsigned int& lm_rep_dl						///< const ref to the length of the LM report data
											, const unsigned int& bus_width						///< const ref to the bus width #buswidth2
											, const unsigned int& anz_data_perzyklus			///< const ref to the number of report dataset per process cycle that the monitor should read from LM
											, const unsigned int& anz_ic1_initiatoren			///< const ref to the number of sae initiators or rather senenv and othernode components
											);

	sc_core::sc_time get_see_reportmem_rl	( const sc_core::sc_time& glob_quantum				///< const ref to the global quantum time
											, const unsigned int& see_rep_dl					///< const ref to the length of the SEE report data
											, const unsigned int& bus_width						///< const ref to the bus width #buswidth2
											, const unsigned int& anz_data_perzyklus			///< const ref to the number of report dataset per process cycle that the monitor should read from SEE
											, const unsigned int& anz_ic1_initiatoren			///< const ref to the number of sae initiators or rather senenv and othernode components
											);

	sc_core::sc_time get_act_wl	( const sc_core::sc_time& glob_quantum							///< const ref to the global quantum time
								, const unsigned int& see_act_dl								///< const ref to the length of the SEE choice data
								, const unsigned int& bus_width									///< const ref to the bus width #buswidth4
								, const unsigned int& anz_data_perzyklus						///< const ref to the number of choice dataset per process cycle that the SEE will send to the actuator and extaction components
								, const unsigned int& anz_ic1_initiatoren						///< const ref to the number of sae initiators or rather senenv and othernode components
								);

	sc_core::sc_time get_see_lm_wl	( const sc_core::sc_time& glob_quantum						///< const ref to the global quantum time
									, const unsigned int& lm_act_dl								///< const ref to the length of the LM result data
									, const unsigned int& bus_width								///< const ref to the bus width #buswidth2
									, const unsigned int& anz_data_perzyklus					///< const ref to the number of result dataset per process cycle that LM will send to the SEE component
									, const unsigned int& anz_ic1_initiatoren					///< const ref to the number of sae initiators or rather senenv and othernode components
									);
	//template <typename vec_type>
	//bool read_out_file(std::vector<vec_type> *in_vect);
}


namespace file_ops
{

//======================================================================
/// @fn read_out_file
//
/// @brief reads the content of a text of binary file and store it in a vector
//======================================================================
	template<typename vec_type> inline void read_out_file(std::vector<vec_type> *in_vect, char* filename)
	{
		std::ostringstream msg;
		msg.str();

		std::ifstream infile;
		infile.clear();

		std::string strfname = std::string(INPUTDIR) + std::string(filename);

		infile.open (strfname.c_str(), std::ifstream::binary);
        if (infile.is_open())
        {
			if (!infile.good())
				return;

			std::copy( std::istream_iterator<vec_type>( infile ), std::istream_iterator<vec_type>(),  std::back_insert_iterator< std::vector<vec_type> >(*in_vect));  		
        }
        else
        {
			msg << "i couldn't open file!";
			ERROR_LOG("key_functions.cpp", __FUNCTION__, msg.str());
        }
		infile.close(); 
		return;
	}

//======================================================================
/// @fn write_in_file
//
/// @brief writes the content of a vector in a file.
//======================================================================
	template<typename vec_type> inline void write_in_file( const char* outfile_name, std::vector<vec_type> *in_vect )
	{
		std::ostringstream msg;
		msg.str();
		std::string strfname = std::string(OUTPUTDIR) + std::string(outfile_name);

		std::ofstream outfile;
        //outfile.open (outfile_name, std::ifstream::out|std::ifstream::app);
		outfile.open (strfname.c_str(), std::ifstream::out|std::ifstream::app);
        if (outfile.is_open())
        {
			if (!outfile.good())
				return;
			
			std::copy( in_vect->begin() , in_vect->end() , std::ostream_iterator<vec_type>(outfile,"\t")  );
			outfile<< endl ;
            outfile.close();   		
        }
        else
        {
			msg << "i couldn't open the file!";
			ERROR_LOG("key_functions.cpp", __FUNCTION__, msg.str());
        }
		return;
	}

//======================================================================
/// @fn build_filename
//
/// @brief constructs the filename by concatenating the given strings.
//======================================================================

	std::string build_filename	( std::string &name_prefix					///< prefix for the filename
								, const char* name_suffix					///< suffix for the filename
								);

}

 
namespace data_conversion
{

	void test(void);
 
//======================================================================
/// @fn toString
//
/// @brief converts a data from a numeric type in a string.
//======================================================================
	template<class T> std::string toString(const T& t)
	{
		 std::ostringstream stream;
		 stream << std::setw(str_size) << std::setprecision(str_size) << std::fixed << t; // - std::setfill('0') ;
		 return stream.str();
	}
 
//======================================================================
/// @fn toString
//
/// @brief converts a string in a numeric type specified by its template parameter.
//======================================================================
	template<class T> T fromString(const std::string& s)
	{
		 std::istringstream stream (s);
		 T t;
		 stream >> std::setprecision(str_size) >> std::fixed >> t;
		 return t;
	}
};
#endif
