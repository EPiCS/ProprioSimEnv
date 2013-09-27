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

#include "lm_core.h"
#include <algorithm>
#include <math.h>


//======================================================================
/// @fn lm_core
//
/// @brief constructor
//
//======================================================================
lm_core::lm_core( std::string node_name_ 										///< basename of the node component
				 ): lmcore_parent_name(node_name_)
{
}


//======================================================================
/// @fn evaluate_data
//
/// @brief evaluates the sensor data
//
/// @details To be implemented according to the system under investigation
//======================================================================
void lm_core::evaluate_data(unsigned char* data, notification::report_actions &status)
{
}

//======================================================================
/// @fn send_data
//
/// @brief receives data from the LModel Component
//
/// @details
///		To be implemented according to the system under investigation
//======================================================================
void lm_core::send_data(unsigned char* data)
{
}
