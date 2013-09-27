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
///  @file see_core.h
//
///  @brief This is the computation core of the see component
///
///  @details It evaluates the received data from the LModel component and
///  	and generates the decision vection of as a result to the data evaluation.
//
//
//==============================================================================

#ifndef __SEE_CORE_H__
#define __SEE_CORE_H__

#include <vector>
#include "key_functions.h"
#include "constants.h"
#include "memory.h"

class see_core {
public:
	see_core(	std::string node_name_
			);
	void eval_and_decide(memory &lm_mem, memory &gvoc_mem, unsigned char* see_dec_data);

private:
	std::string			seecore_output_filename, see_parent_name;
}; 
#endif