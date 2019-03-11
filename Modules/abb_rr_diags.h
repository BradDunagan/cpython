/*
		 1         2         3         4         5         6         7
1234567890123456789012345678901234567890123456789012345678901234567890123456789

	File:	diags.h

	25-Mar-98

	Based on  D:\GISHA001\Diags\ - hdr\Diags.h  and  src\Diags.cpp.

	Some of the functions used in the GISHA version are removed.  
	
	When GISHA development is continued the functions that have been removed
	or are otherwise needed by GISHA and not here should be implemented with 
	a class derived from this one.

	27-Mar-98

	See hand-written notes of this date, pg 1.

	The program code calls the global functions listed at the bottom of this
	file.


	1999-Sep-28

	Changed name of class from "Diagnostics" to "CDiags".
*/

#ifndef		__DIAGS_H
#define		__DIAGS_H

/*------------------------------- Dependencies  ---------------------------*/


//	End Dependencies


#define PORT


//	Diagnostic levels.
//
#define	D0		0x00000010	//	Most submitted - lowest level, most detail.
#define	D1		0x00000200
#define	D2		0x00003000
#define	D3		0x00040000
#define	D4		0x00500000	//	Least submitted - highest level, least detail.

#define	D_LOWEST	D0
#define	D_HIGHEST	D4

#define	DA			(0x10000000)	//	Always submit.  Can not be disabled.




enum				//	Diagnostic types enabled.
{
	dtBCDError		= 0x00000001,
	dtError			= 0x00000002,
	dtSysError		= 0x00000004,
	dtPrint			= 0x00000008,
	dtCodeSystem	= 0x00000010,
	dtData			= 0x00000020,
	dtExecutive		= 0x00000040,
	dtModeling		= 0x00000080,
	dtCodePieces	= 0x00000100,
	dtBigPicture	= 0x00000200,
	dtNotice        = 0x00000400
};


//	Diagnostics.  Non-errors.  Status messages, progress messages, etc..
//
//	Different "levels" can be turned on/off by the user by way of program
//	options/setup dialogs.  Such configuration values are probably stored
//	in the registry (if Windows) and should be read when the program starts.
//
//->	What if the user wants to change the levels-on/off setting while the
//		program is running?
//
//	When viewing the diagnostics the user should be able to select which
//	levels to view.  
//	
//		What about which functions?  
//
//		Which types?
//
PORT void	Print ( int Level, const char * Func, const char * Fmt, ... );

#endif

//	Diags.h
