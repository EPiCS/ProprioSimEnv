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

#include "memory.h"
#include "reporting.h"
#include "constants.h"

static const char *filename = "memory.cpp";


//======================================================================
/// @fn memory
//
/// @brief constructor
//
//======================================================================
memory::memory    
(
  const unsigned int ID										// Target ID
, sc_core::sc_time   read_delay								// read delay
, sc_core::sc_time   write_delay							// write delay
, sc_dt::uint64      memory_size							// memory size (bytes)
, unsigned int       memory_width							// memory width (bytes)
)
: memory_ID         (ID				)
, i_ID				(0				)
, m_read_delay      (read_delay		)
, m_write_delay     (write_delay	)
, m_memory_size     (memory_size	)
, m_memory_width    (memory_width	)
{ 
///verify the size of the memory	

/// Allocate and initalize an array for the target's memory
	m_memory = new unsigned char[size_t(m_memory_size)];

/// clear memory - size_t(m_memory_size) -> specify the maximum number of Bytes memset has to affect
	memset(m_memory, 0, size_t(m_memory_size)); 

} // end Constructor


//==============================================================================
///  @fn memory::operation
//  
///  @brief performs read's and write's on the memory and give feedback.
// 
///  @details 
///		It checks the attributes of the transaction object to determine if the 
///		required operation can or cannot be performed. If possible the required
///		operation is executed. If not it just gives an error message as response. 
///		<br> At the end, the memory access is reported.
//
///	@see memory::check_address(tlm::tlm_generic_payload& tObj)
/// @see trtr::rep_mem_access()
//
//==============================================================================
bool memory::operation	( unsigned int id,							///< initiator ID
						  tlm::tlm_generic_payload& tObj,			///< ref to transaction object 
						  sc_core::sc_time& delay					///< transaction delay 
						)
{
	//Initiator id
	i_ID = id;

	// Access the required attributes from the payload
	sc_dt::uint64    address	=	tObj.get_address();			// memory address
	tlm::tlm_command command	=	tObj.get_command();			// memory command
	unsigned char    *data		=	tObj.get_data_ptr();		// data pointer
	unsigned  int    length		=	tObj.get_data_length();		// data length
	unsigned int	burst_length=	(unsigned int)(ceil((double)length/m_memory_width));
	
	tlm::tlm_response_status response_status = check_address(tObj);

	if (tObj.get_byte_enable_ptr())
	{
		tObj.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		delay = sc_core::SC_ZERO_TIME;
		return false;
	}
	else if (tObj.get_streaming_width() != tObj.get_data_length()) 
	{
		tObj.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		delay = sc_core::SC_ZERO_TIME;
		return false;
	}

	switch (command)
	{
	default:
	{
		tObj.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		break;
	}
    
    // Setup a TLM_WRITE_COMMAND Informational Message and Write the Data from
    // the Generic Payload Data pointer to Memory
	case tlm::TLM_WRITE_COMMAND:
	{
		if (response_status == tlm::TLM_OK_RESPONSE)
		{
			memcpy( (m_memory+address), data, size_t(length) );
		}
		delay = delay + m_write_delay*burst_length;
		break;
	}

	// Setup a TLM_READ_COMMAND Informational Message and read/copy the Data from
	// the Memory to the Generic Payload Data pointer 
	case tlm::TLM_READ_COMMAND:
	{
		if (response_status == tlm::TLM_OK_RESPONSE)
		{

			memcpy(data, m_memory+address, size_t(length));
		}
		delay = delay + m_read_delay*burst_length;
		break;
	}

	} // end switch

	tObj.set_response_status(response_status);
  
	///report memory acces
	trtr::rep_mem_access(filename, memory_ID, m_memory_width, i_ID, tObj);

	return true;
}// end memory_operation


//==============================================================================
///  @fn memory::check_address
//  
///  @brief It checks if the transaction address is in the address range of this 
///		memory
// 
///  @details
///    	This routine is used to check for errors in address space
//
//==============================================================================
tlm::tlm_response_status  memory::check_address  ( tlm::tlm_generic_payload& tObj			///< ref to transaction object
												 )
{
	unsigned int	start_address		=	tObj.get_address();			// memory address
	unsigned int	length				=	tObj.get_data_length();		// data length

	if ( start_address >= m_memory_size )
	{
		return tlm::TLM_ADDRESS_ERROR_RESPONSE;					// operation response
	}
	else
	{
		if ( (start_address + length) > m_memory_size )   
		{
			return tlm::TLM_ADDRESS_ERROR_RESPONSE;				// operation response
		}
		return tlm::TLM_OK_RESPONSE;							// operation response
	}
}

//==============================================================================
///  @fn memory::check_address
//  
///  @brief It checks if the given start address as well as the end address
///		of the operation is in the address range of this memory.
// 
///  @details
///    	This routine is used to check for errors in address space
//   
//==============================================================================
tlm::tlm_response_status  memory::check_address ( const unsigned int &address				///< ref to start address of the write operation
												, const unsigned int &length				///< ref to length of data to be written
												)
{
	if ( address >= m_memory_size )
	{
		return tlm::TLM_ADDRESS_ERROR_RESPONSE;				// operation response
	}
	else
	{
		if ( (address + length-1) > m_memory_size )   
		{
			return tlm::TLM_ADDRESS_ERROR_RESPONSE;			// operation response
		}	
		return tlm::TLM_OK_RESPONSE;						// operation response
	}
	
}


//==============================================================================
///  @fn memory::get_mem_ptr
//  
///  @brief returns pointer to the correspondant memory range
//   
//==============================================================================
unsigned char* memory::get_mem_ptr(void)
{
	return m_memory;
}


//==============================================================================
///  @fn memory::self_write
//  
///  @brief It performs write operations of a component on his own memory after 
///		checking that the required write can be performed.
//
///	 @see check_address(const unsigned int &address, const unsigned int &length) 
//
//==============================================================================
void memory::self_write	( const sc_dt::uchar &adr											///< const ref to start address			
						, unsigned char *source_array										///< pointer to the data source array	
						, const unsigned int &data_length									///< const ref to data length
						)
{
	std::ostringstream msg;
	
	tlm::tlm_response_status response_status = check_address(adr, data_length);
	if ( response_status == tlm::TLM_OK_RESPONSE)
	{
		sc_dt::uchar address = adr;
		for (int i = 0 ; i < data_length; i++)
		{
			m_memory[address++] = source_array[i];
		}
	}
	else
	{
		msg.str("");
		msg << "\t Memory: " << memory_ID 
			<< "\t ADDRESS ERROR: WRITE ACCESS TO THE MEMORY WASN'T POSSIBLE! ";
		ERROR_LOG( filename, __FUNCTION__, msg.str().c_str() );
	}
}


//==============================================================================
///  @fn memory::self_read
//  
///  @brief It performs read operations of a component on his own memory after 
///		checking that the required read can be performed, in other words, that the
///		given address range is between the memory boundaries.
//
///	 @see check_address(const unsigned int &address, const unsigned int &length) 
//
//==============================================================================
	void memory::self_read	( sc_dt::uint64 &adr												///< const ref to start address				
							, unsigned char *target_array										///< pointer to the target array for the data
							, const unsigned int &data_length									///< pointer to the data length
							)
	{
		std::ostringstream msg;	
		tlm::tlm_response_status response_status = check_address(adr, data_length);
		if ( response_status == tlm::TLM_OK_RESPONSE)
		{
			memcpy( target_array, (m_memory+adr), size_t(data_length) );
		}
		else
		{
			msg.str("");
			msg << "\t Memory: " << memory_ID 
				<< "\t ADDRESS ERROR: READ ACCESS TO THE MEMORY WASN'T POSSIBLE! ";
			ERROR_LOG( filename, __FUNCTION__, msg.str().c_str() );
		}
	}