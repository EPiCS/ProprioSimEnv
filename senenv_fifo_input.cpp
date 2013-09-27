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
*======================================================================================
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

#include "senenv_fifo_input.h"
#include "reporting.h"

static const char *filename = "stimulus.cpp";				///< filename for reporting

//========================================================================================
///  @fn stimulus
//
///  @brief constructor
//
///	@details It registers the method process of the module to the kernel.
////	<br> It initializes the vector of output ports with the number of fifo channels  
///		to which they ports should later be bounded to and specified during instantiation.
///		<br> It finally reads out the input file for sensor data.
//
//========================================================================================
senenv_fifo_input::senenv_fifo_input	( sc_module_name	module_name				///< sc module name
					, unsigned int		out_port_nr									///< number of output ports
					, sc_core::sc_time	glob_quantum_								///< time value for the global quantum
					, char*				filename_									///< input file name
					): 
					   s_out_port_nr(out_port_nr)
					,  out_ports("Stimulus_nr")
					,  glob_quantum(glob_quantum_)
					,  s_counter ( 0 )
					,  inputfile( filename_)
{
	SC_METHOD(senenv_fifo_input_thread);
	out_ports.init(s_out_port_nr);
	read_data(inputfile);
}


//========================================================================================
///  @fn stimulus_thread
//
///  @brief method process. It is triggered after every quantum time and writes the sensor 
///		data into the fifo channel(s) before the transactions start. 
//
///	 @details Before every write operation, it makes sure that there is unoccupied slots in 
///		the fifo available.<br>
///		This can be modified to better suit the user needs
//========================================================================================
void senenv_fifo_input::senenv_fifo_input_thread(void)
{
	std::ostringstream msg;
	msg.str("");
	msg<< "\t FIFO NR: " << s_out_port_nr;

	if (out_ports.at(0)->num_free() > 0)
	{
		if(s_counter < data_vector.size())
		{
			out_ports[0] -> write(data_vector.at(s_counter));
		}
		else
		{
			msg<< " NO DATA AVAILABLE!" << s_counter;
			sc_core::sc_stop();
		}
	}
	else
	{
		msg<< "NO FREE SLOTS IN THE FIFO CHANNEL!";
		ERROR_LOG( filename, __FUNCTION__, msg.str() );
	}
	
	sc_core::next_trigger(glob_quantum);
}


//========================================================================================
///  @fn read_data
//
///  @brief It readouts the input file and stores the data in a vector.
//
///	 @details to be implemented according to the needs
//
//========================================================================================
void senenv_fifo_input::read_data(char* filename)
{
	file_ops::read_out_file(&data_vector , filename);
}