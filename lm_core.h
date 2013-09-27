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
///  @file lm_core.h
//
///  @brief This is the computation core of the lmodel component
///
///  @details It evaluates the sensor data readout from the sae's memory space.
//
//==============================================================================

#ifndef __LM_CORE_H__
#define __LM_CORE_H__

#include "reporting.h"
#include "constants.h"
#include "key_functions.h"
#include <vector>

class lm_core
{
public:
	lm_core	( std::string node_name_ 							///< basename of the node component
			);

	void evaluate_data(unsigned char*, notification::report_actions &status);	
	void send_data(unsigned char*);								// convert result data: from datyp to char

private:
	std::string lmcore_output_file, lmcore_parent_name;
};

#endif