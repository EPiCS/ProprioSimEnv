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
*=====================================================================================
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
///  @file SENSENV_FIFO_INPUT.h
//
///  @brief This is the module which has the method process responsible for filling up the fifo
///		channels of a node bounded to the input ports of the senenv components in each process
///		cycle before the transactions start.
//
///	@details To be implemented according to the needs
//
//========================================================================================

#ifndef __SENSORENV_FIFO_INPUT_H__
#define __SENSORENV_FIFO_INPUT_H__

#include <systemc.h>
#include "constants.h"
#include "key_functions.h"

class senenv_fifo_input:public sc_module
{

public:
	senenv_fifo_input( sc_module_name	module_name							///< sc module name
			, unsigned int		out_port_nr									///< number of output ports
			, sc_core::sc_time	glob_quantum_								///< time value for the global quantum
			, char*				filename_									///< input file name
			);

	SC_HAS_PROCESS( senenv_fifo_input );
	void senenv_fifo_input_thread( void );
	void read_data	( char* file_name										///< pointer to input file name
					);

	sc_core::sc_vector< sc_port< sc_fifo_out_if<datyp> > > out_ports;		///< vector of output port with access right to fifo channel
	
private:
	int					s_out_port_nr, s_counter;
	sc_core::sc_time	glob_quantum;
	char*				inputfile;
	std::vector<datyp>	data_vector;

};
#endif