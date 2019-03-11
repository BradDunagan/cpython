/*
		 1         2         3         4         5         6         7		  
123456789012345678901234567890123456789012345678901234567890123456789012345678

	File:	libarg.cpp

*/

/*------------------------------ Dependencies  -----------------------------*/

#include	"abb_rr_libarg.h"

	
//	End Dependencies



//	A macro for specifiying filename and function name in diagnostics
//	call.
//
#define	F(f)	"("__FILE__")" "  " f




/*-----------------------------  PECB::PECB()  -----------------------------*/
/*d
*/
PECB::PECB  ( int		PE_Id, 
			  PECBFunc  CallBack, void * pPE, PECallback cbPE, CWnd * pWnd  )
{
	this->PE_Id    = PE_Id;

	this->CallBack = CallBack;

	this->pPE      = pPE;

	this->cbPE	   = cbPE;

	this->pWnd	   = pWnd;

	this->p        = 0;

	Flags = 0;

	ChgFlags = 0;

}	//	PECB::PECB()


/*-----------------------------  PECB::PECB()  -----------------------------*/
/*d
*/
PECB::PECB  ( int		PE_Id,   
			  PECBFunc  CallBack, void * pPE, PECallback cbPE, CWnd * pWnd,
			  uint		ChgFlags  )
{
	this->PE_Id    = PE_Id;

	this->CallBack = CallBack;

	this->pPE      = pPE;

	this->cbPE	   = cbPE;

	this->pWnd	   = pWnd;

	this->p        = 0;

	Flags = 0;

	this->ChgFlags = ChgFlags;

}	//	PECB::PECB()


