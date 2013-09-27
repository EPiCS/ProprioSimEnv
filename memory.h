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
///  @file memory.h
//
///  @brief This is for isolating operations on a component memory from the rest
//
//==============================================================================

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <systemc.h>
#include <tlm.h>

class memory
{

// Member Methods  ====================================================
public:

	memory	( const unsigned int ID					///< component ID
			, sc_core::sc_time   read_delay         ///< delay for reads
			, sc_core::sc_time   write_delay        ///< delay for writes
			, sc_dt::uint64      memory_size        ///< memory size (bytes)
			, unsigned int       memory_width       ///< memory width (bytes)
			);					


	unsigned char* get_mem_ptr(void);



	bool operation	(	unsigned int id,												///< intiator component ID
						tlm::tlm_generic_payload& tObj,									///< ref to transaction object 
						sc_core::sc_time& delay_time									///< ref to time delay
					);	

	void self_write	( const sc_dt::uchar &adr											///< const ref to start address			
					, unsigned char *source_array										///< pointer to the data source array	
					, const unsigned int &data_length									///< const ref to data length
					);

	void self_read	( sc_dt::uint64 &adr												///< const ref to start address				
					, unsigned char *target_array										///< pointer to the target array for the data
					, const unsigned int &data_length									///< pointer to the data length
					);

	tlm::tlm_response_status check_address 	( tlm::tlm_generic_payload& tObj			///< ref to transaction object
											);
											
	tlm::tlm_response_status  memory::check_address ( const unsigned int &address		///< ref to start address of the write operation
													, const unsigned int &length		///< ref to length of data to be written
													);

	sc_dt::uint64         m_memory_size;           ///< memory size (bytes)
	unsigned int          m_memory_width;          ///< memory width (bytes)

// Member Variables/Objects  ===================================================
private:   
   unsigned int          memory_ID;               ///< target ID
   unsigned int          i_ID;                    ///< initiator ID
   sc_core::sc_time      m_read_delay;            ///< read delay
   sc_core::sc_time      m_write_delay;           ///< write delay
   unsigned char         *m_memory;               ///< memory
};
#endif /*__MEMORY_H__*/