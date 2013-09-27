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

#include <iomanip>
#include <systemc.h>
#include <queue>

#include "constants.h"
#include "reporting.h"


namespace trtr
{
//=============================================================================
///	@fn follow_transactions
//
///	@brief It is used  to report a transaction in the node.
//
//=============================================================================
	void follow_transactions( const char *filename					///< reporting file name
							, const std::string &name				///< const sc_module_name &name			///< sc module name of the concerned component
							, const unsigned int &id				///< const ref to concerned component ID
							, const sc_core::sc_time &delay			///< const ref to the transaction time delay
							, const transport_type &t_type			///< const ref to the transaction step 
							, const transport_status &t_status		///< const ref to the transaction response status
							, unsigned int target_id				///< component ID of the transaction target
							, const char *rsp_str					///< const pointer to the error message when the transaction failed
							)
	{
		std::ostringstream   msg;
		msg.str ("");
		msg << "\t " << name << ": " << id ;

		switch(t_type)
		{
		case(T_CALL):
			msg << "\t b_transport call with delay of: " << delay ;
			break;

		case(T_RETURN):
			msg << "\t b_transport returned delay of:  " << delay ;

			switch(t_status)
			{
			case(T_SUCCESS):
				msg << " -> SUCCESSFULL";
				if ( target_id != 0 )
				{
					msg << "\n" << "\t TARGET ID: " << target_id;
				}
				INFO_LOG(filename, __FUNCTION__ , msg.str());
				break;

			case(T_FAILURE):
				msg << " -> FAILED" << endl
					<< "\t " << rsp_str;
				if ( target_id != 0 )
				{
					msg << "\n" << "\t TARGET ID: " << target_id;
				}
				ERROR_LOG(filename, __FUNCTION__ , msg.str());
				break;

			}

			break;
		}

	}


//=============================================================================
///	@fn follow_synch
//
///	@brief It is used  to report a synchronisation operations in the node.
//
//=============================================================================
	void follow_synch		( const char *filename					///< reporting file name
							, const std::string &name				///< const sc_module_name &name			///< sc module name of the concerned component
							, const std::string &process_name		///< process name
							, const unsigned int &id				///< const ref to concerned component ID
							, const sc_core::sc_time &delay			///< const ref to the transaction time delay
							, const synch_type &s_type				///< const ref to the sync_type 
							)
	{
		std::ostringstream   msg;
		msg.str ("");
		msg << "\t Process: "<<  process_name
			<< "\t " << name << ": " << id ;

		switch(s_type)
		{
		case(S_NEED):
			msg << "\t need to synch | delay " << delay;
			break;
		case(S_RETURN):
			msg << "\t has synched	 | delay " << delay;
			break;
		}

		INFO_LOG(filename, __FUNCTION__ , msg.str());
	}

	
//=============================================================================
///	@fn rep_mem_acces
//
///	@brief It is used  to report an operation on a memory object in the node.
//
//=============================================================================
void rep_mem_access			( const char				*filename			///< pointer to the file name
							, const unsigned int		&memory_id			///< const ref to the component ID of the memory owner
							, const unsigned int		&memory_width		///< const ref to the width of the memory which is the same as the corresponding buswidth
							, const unsigned int		&initiator_id		///< const ref to the component ID of the operation initiator
							, tlm::tlm_generic_payload& tObj				///< const ref to the transaction object
							)
	{
		std::ostringstream     msg;
		msg.str("");
		

		tlm::tlm_command	cmd		=	tObj.get_command();
		sc_dt::uint64		adr		=	tObj.get_address();
		unsigned int		length	=	tObj.get_data_length();
		
		unsigned char 		*data	=	new unsigned char[size_t(length)];
		memset(data, 0, size_t(length));
		data						=	tObj.get_data_ptr();
		
		unsigned int	burst_length=	(unsigned int)(ceil((double)length/memory_width));

		msg << "\t MEMORY ID		: "
			<< std::dec << memory_id 
			<< endl

			<< "\t COMMAND		: "	
			<< ((cmd == tlm::TLM_WRITE_COMMAND) ? "WRITE" : "READ")
			<<endl

			<< "\t LENGTH			: "
			<< std::internal	<< std::setw( 2 ) << std::setfill( '0' ) << std::dec << length
			<< endl

			<< "\t ADR			: 0x"	
			<< std::internal	<< std::setw( sizeof(adr) ) << std::setfill( '0' )<< std::uppercase	<< hex << adr
			<<endl
			
			<< "\t DATA			:  ";  

		std::string s_tmp;
		if (length < str_size)
		{
			s_tmp.assign ( (char*)data, length );
			msg << std::left << std::setw(length) << std::setfill(' ') 
				<< s_tmp;
		}
		else
		{
			for(int i = 0; i<length/str_size; i++)
			{
				s_tmp.assign	( (char*)data, str_size );
				msg << std::left << s_tmp << "\t";
				data += str_size;
			}
		}	

		INFO_LOG(filename, __FUNCTION__, msg.str());
	}

}