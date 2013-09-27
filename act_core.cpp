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
*=======================================================================
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

#include "act_core.h"
#include "key_functions.h"


//======================================================================
/// @fn act_core
//
/// @brief constructor 
//
//======================================================================
act_core::act_core( std::string node_name_ 										///< basename of the node component
				   )
{

}


//======================================================================
/// @fn execute_see_decision
//
/// @brief uses the received decision information to execute the selected actions
//
/// @details
///		It should be implemented according to the system under investigation
//
//======================================================================
void act_core::execute_see_decision ( memory& mem								///< memory object of the actuator component
									, std::ostringstream&  msg					///< output message for the status change of each thread
									)
{

}