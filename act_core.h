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
///  @file act_core.h
//
///  @brief This is the computation core of the actuator
///
///  @details It evaluates the received data from the SEE and executes the
///		the corresponding action(s)
//
//==============================================================================

#ifndef __ACT_CORE_H__
#define __ACT_CORE_H__

#include "node_thread.h"
#include "memory.h"
#include "constants.h"
#include "key_functions.h"

#include <iostream>

class act_core
{
public:
	act_core	(	std::string node_name_									///< basename of the node component
				);

	void execute_see_decision	( memory& mem								///< memory object of the actuator component
								, std::ostringstream&  msg					///< output message for the status change of each thread
								);

};

#endif