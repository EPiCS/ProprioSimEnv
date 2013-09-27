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
///  @file constants.h
//
///  @brief Group of constants and global variables of the model. The values of 
///		some of these global variables can or must be modified by the user before
///		simulation start.
//
/// @defgroup GLOBS
/// @{
//
//==============================================================================

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <systemc.h>


//Konstanten und globale Variablen

//Konstanten///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//number of communication partners of a component
	const unsigned int			sae_initiator_number	=   2	;							///< number of initiators of the sae component
	const unsigned int			ic2_target_number		=   2	;							///< number of target of the ic2 component
	const unsigned int			ic3_target_number		=   2	;							///< number of target of the ic3 component
	const unsigned int			see_initiator_number	=   3	;							///< number of initiators of the see component
	
	//Identification number of the components
	const unsigned int			gvoc_id_nr				=	100	;							///< GVOC component ID
	const unsigned int			sensor_id_nr			=	200	;							///< Sensor component ID
	const unsigned int			othernodes_id_nr		=	300 ;							///< OtherNode component ID
	const unsigned int			ic1_id_nr				=	400	;							///< Monitor component ID
	const unsigned int			sae_id_nr				=	500	;							///< SAE component ID
	const unsigned int			lmodel_id_nr			=	600	;							///< LModel component ID
	const unsigned int			monitor_id_nr			=	700	;							///< Monitor component ID
	const unsigned int			see_id_nr				=	800	;							///< SEE component ID
	const unsigned int			act_id_nr				=	900	;							///< Actuator component ID
	const unsigned int			extaction_id_nr			=	1000;							///< ExtAction component ID
	
	//BUSWIDTH - for sockets as Template argument and for Beats
	const unsigned int			buswidth1				=	32				;				///< (in Bytes) max data length pro transaction over sockets
	const unsigned int			buswidth2				=	32				;				///< (in Bytes) max data length pro transaction over sockets
	const unsigned int			buswidth3				=	32				;				///< (in Bytes) max data length pro transaction over sockets
	const unsigned int			buswidth4				=	32				;				///< (in Bytes) max data length pro transaction over sockets
	const unsigned int			buswidth5				=	32				;				///< (in Bytes) max data length pro transaction over sockets

	//schould be set before each simulation
	const sc_core::sc_time_unit node_time_unit			=	sc_core::SC_MS	;				///< time unit in the model. It could be changed by the user, if necessary


//Variablen /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//socket index of transaction targets
	extern unsigned int			m_target_idx					;							///< index of the monitor target for a transaction
	extern unsigned int			gvoc_target_idx					;							///< index of the gvoc target for a transaction
	extern unsigned int			see_target_idx					;							///< index of the see target for a transaction
	extern unsigned int			ic1_initiator_idx				;							///< index of the memory subarea of an sae initiator on the sae memory

	//event to trigger processes
	extern	sc_core::sc_vector <sc_core::sc_event>	lmodel_to_monitor_vector;				///< vector of notified events after a transaction between lmodel and sae (C1 to E1) 	
	extern  sc_core::sc_vector <sc_core::sc_event>	lmodel_to_see_vector	;				///< vector of notified events after a transaction between lmodel and see (C2 to D)
	extern	sc_core::sc_vector <sc_core::sc_event>	see_to_monitor_vector	;				///< vector of notified events after a transaction between see and actuator or extaction (D to E2)
	extern	sc_core::sc_vector <sc_core::sc_event>	monitor_ev_vector		;				///< vector of notified events after a transaction between lmodel and (C1 to C2 or E1 to C2) 


//Konstanten /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NUMB_OF_NODES	1																	///< number of node in the node in the system under unvestigation
#define INPUTDIR	"C:/***/input/"															///< storage location for input files
#define OUTPUTDIR	"C:/***/output/"														///< storage location for output files

	typedef sc_dt::uchar			datyp						;							///< data type in the model
	#define str_size				1														///< string size when converting data from type datyp to string and vice-versa

	//LM report data - example
	const sc_dt::uchar lm_pos_result[] = "A"					;							///< report of the LM component  when the evaluated sensor information is in the acceptable limits
	const sc_dt::uchar lm_neg_result[] = "F"					;							///< report of the LM component  when the evaluated sensor information is not in the acceptable limits



/// @}
#endif