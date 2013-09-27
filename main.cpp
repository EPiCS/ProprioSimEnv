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

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <iomanip>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <vector>

#include "lt_top_one.h"
#include "stimulus.h"
#include "key_functions.h"


using namespace std;

static const char *filename = "main.cpp"; ///< filename for reporting

//======================================================================
/// @fn main
//
/// @brief This is the entry point of the model. 
//
///	@details It should contain the nodes instantiation and bindings operations
///		among them. We recommend to do these in an extra module and 
///		just instantiate this object here as shown in the commented code
//
//======================================================================
int sc_main(int argc, char *argv[]){

// current system time
	std::time_t t; 
	time(&t);
	std::cout<< "CURRENT DATE AND TIME: " << ctime(&t) << endl;

//variable for the report messsage
	std::ostringstream  msg;

//simulation_start
	msg.str(""); 
	msg << __FUNCTION__ << endl
		<< setfill('*') << setw(80) << " " << endl
		<< setfill(' ') << setw(31) << "SIMULATION: START "<<endl
		<< setfill('*') << setw(80) << " ";
	SC_REPORT_INFO( filename, msg.str().c_str() );

	//****** Programm Start: Example *****///
	// sc_core::sc_time global_quantum	= sc_core::sc_time(2, node_time_unit);
	// sc_core::sc_time simulation_time	= sc_core::sc_time(14000, node_time_unit);
	//Sim TOP("SYSTEM", global_quantum, simulation_time);
	//sc_core::sc_start();
	//****** INSTANTIATION*****///

	msg.str("");
	msg << __FUNCTION__ << endl
		<< setfill('*') << setw(80) << " " << endl
		<< setfill(' ') << setw(32) << "SIMULATION: END "<<endl
		<< setfill('*') << setw(80) << " ";
	SC_REPORT_INFO(filename, msg.str().c_str());


	//delta cycles
	std::cout << "NUMBER OF DELTA CYCLES:	"<< sc_core::sc_delta_count() << endl;
	// current system time
	time(&t);
	std::cout<< "CURRENT DATE AND TIME:	" << ctime(&t);


	return 0;
}