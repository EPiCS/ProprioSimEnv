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

#include "constants.h"

sc_core::sc_vector <sc_core::sc_event> monitor_ev_vector		("monitor_event_"		, NUMB_OF_NODES	)	;
sc_core::sc_vector <sc_core::sc_event> see_to_monitor_vector	("see_to_monitor_event_", NUMB_OF_NODES )	;
sc_core::sc_vector <sc_core::sc_event> lmodel_to_see_vector		("lmodel_to_see_event_"	, NUMB_OF_NODES	)	;
sc_core::sc_vector <sc_core::sc_event> lmodel_to_monitor_vector	("lmodel_to_monitor_"	, NUMB_OF_NODES	)	;