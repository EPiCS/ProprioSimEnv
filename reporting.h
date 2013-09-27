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
*========================================================================================
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
///  @file reporting.h
//
///  @brief These are all the reporting functions used in the model.
//
//========================================================================================

#ifndef __REPORTING_H__
#define __REPORTING_H__

#pragma once
#include "tlm.h"                                        ///< TLM headers
#include <sstream>                                      ///< string streams
#include <iomanip>                                      ///< I/O manipulation
#include <stdio.h>                                      ///< standard I/O
#include <stdlib.h>

#define INFO_LOG(filename, function_str, text)\
	{\
		std::ostringstream os;\
		std::string function_str_mod (function_str);\
		int location;\
		if ((location = function_str_mod.find("::")) != -1) \
		{ \
			function_str_mod.erase(0, location + 2); \
		} \
		os << " " << sc_core::sc_time_stamp() << " - " << function_str_mod  << "\n" << text; \
		SC_REPORT_INFO(filename, os.str().c_str());\
	}


#define WARNING_LOG(filename, function_str, text)\
	{\
		std::ostringstream os;\
		std::string function_str_mod (function_str);\
		int location;\
		if ((location = function_str_mod.find("::")) != -1) \
		{ \
			function_str_mod.erase(0, location + 2); \
		} \
		os << " " << sc_core::sc_time_stamp() << " - " << function_str_mod  << "\n" << text;\
		SC_REPORT_WARNING(filename, os.str().c_str());\
	}


#define ERROR_LOG(filename, function_str, text)\
	{\
		std::ostringstream os;\
		std::string function_str_mod (function_str);\
		int location;\
		if ((location = function_str_mod.find("::")) != -1) \
		{ \
			function_str_mod.erase(0, location + 2); \
		} \
		os << " " << sc_core::sc_time_stamp() << " - " << function_str_mod  << "\n" << text;\
		SC_REPORT_ERROR(filename, os.str().c_str());\
	}

#endif

#define BUS_WIDTH(bus_laenge) (bus_laenge/8)


/// comprised all elements and functions neccessary to notify the lmodel about each write operation
/// on the sae memory
namespace notification
{
	///	Each element of this enumeration represents the status of the lmodel and see components actions
	enum report_actions
	{
		READ					=	11,								///< indicates a read operation on the sae memory
		WRITE					=	22,								///< indicates a write operation on the sae memory
		ACTION_FAILED			=	55,								///< Operation failed
		ACTION_NEEDED			=	33,								///< The result of the SAE data processing by LM indicates that actions are needed
		NOTIFY					=	44								///< write operation on the SEE memory failed
	};

}

namespace trtr // transport-transactions
{
	///indicates the current state of a transaction => whether it is about to begin or has already been carried out (return)
	enum transport_type
	{
		T_CALL,														///< transaction call
		T_RETURN													///< transaction return
	};

	///indicates the response status of a transaction
	enum transport_status
	{
		T_SUCCESS,													///< transaction successful
		T_FAILURE,													///< transaction failed
		T_IGNORE		
	};

	///indicates the synchronisation status of a process
	enum synch_type
	{
		S_NEED,														///< process need to synch
		S_RETURN													///< process has synched
	};


	void follow_transactions( const char *filename					///< reporting file name
							, const std::string &name				///< const sc_module_name &name			///< sc module name of the concerned component
							, const unsigned int &id				///< const ref to concerned component ID
							, const sc_core::sc_time &delay			///< const ref to the transaction time delay
							, const transport_type &t_type			///< const ref to the transaction step 
							, const transport_status &t_status		///< const ref to the transaction response status
							, unsigned int target_id = 0			///< component ID of the transaction target
							, const char *rsp_str = ""				///< const pointer to the error message when the transaction failed
							);


	void follow_synch		( const char *filename					///< reporting file name
							, const std::string &name				///< const sc_module_name &name			///< sc module name of the concerned component
							, const std::string &process_name		///< process name
							, const unsigned int &id				///< const ref to concerned component ID
							, const sc_core::sc_time &delay			///< const ref to the transaction time delay
							, const synch_type &s_type				///< const ref to the sync_type 
							);


	void rep_mem_access		( const char				*filename
							, const unsigned int		&memory_id			///< const ref to the component ID of the memory owner
							, const unsigned int		&memory_width		///< const ref to the width of the memory which is the same as the corresponding buswidth
							, const unsigned int		&initiator_id		///< const ref to the component ID of the operation initiator
							, tlm::tlm_generic_payload& tObj
							);
}
