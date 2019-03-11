/*
	File:	error.cpp


	Created:	03-Aug-98

	See notes in the header file error.h.
*/

#include	<stdio.h>

#ifndef	WIN32
	#include	<stdarg.h>
	#include	<string.h>
	#ifndef JS
		#include	<glib.h>
	#endif
	#include	<errno.h>
	#include	<ctype.h>
	#include	"abb_rr_not_win32.h"
#endif

#include	"abb_rr_error.h"

#include    "abb_rr_diags.h"


#define	F(f)	"("__FILE__")" "  " f


//	Errors "pool".
//
//	Should errors be stored separately for each thread?  Keyed on thread
//	Id?
//
//	How many threads might there be?
//
//	Certainly, not every thread will error at the same time.  Will they?
//
//	Certainly not.  Impossible.
//
//	If not, then one set of error string buffers can be shared between
//	them.
//
/*
#define	MAX_ERR_ITEMS	16
*/
#define	MAX_ERR_ITEMS	64			//	2003-May-14

struct ErrItem
{
	int		Type;

	char	ErrStr [MAX_ERROR_STR_LEN + 1];
};

static ErrItem	EIPool [MAX_ERR_ITEMS];

static int		iEINext = 0;

static int		HW = 0;			//	High water.

static int		EIIdx [MAX_ERR_ITEMS];

static int		bLog = 1;

static int		ToPool ( int AE, const char * Func, const char * Buf1 );

static ErrItem *	GetEI ( int ENo, char * sE, uint SizeE );

static void		Trace (		  int	 Type,		 int	ENo, 
					    const char * Func, const char * Text );

static void		RemoveCrLf ( int & i, char Buf[], uint SzBuf );

static void		Buffer ( const char * Msg, uint nC );

static uint		nE = 0;



//	2004-Aug-05		Buffers.
//
//	When something wants to catch error and notice messages into a buffer.
//
//	See StartBufferErrors(), StopBufferErrors() and Buffer().
//
#define	MAX_BUFFER_THDS		16

static	uint	nBufferThds = 0;

struct	BufferThd
{
	DWORD		ThdId;

	char *		Buffer;

	uint		SzBuffer;

	uint		iB;

};	//	BufferThd

static	BufferThd	BuffThd [MAX_BUFFER_THDS];


/*-----------------------------  ResetE()  --------------------------------*/
/*
	See comments in error.h.
*/
void	ResetE()
{
	iEINext = HW = 0;

}	//	ResetE()


/*----------------------------  LogErrors()  ------------------------------*/
/*
	See comments in error.h.
*/
int		LogErrors ( int OnOff )
{
	int		bLogWas = bLog;

	bLog = OnOff;

	return bLogWas;

}	//	LogErrors()


/*-----------------------------  GetNErrs()  ------------------------------*/
/*
	See comments in error.h.
*/
uint	GetNErrs()
{
	return nE;

}	//	LogErrors()


/*------------------------  StartBufferErrors()  --------------------------*/
/*
	See comments in error.h.
*/
void	StartBufferErrors ( char * Buffer, uint SzBuffer )
{
	if ( nBufferThds >= MAX_BUFFER_THDS )  return;

	BufferThd *	pBT = &BuffThd[nBufferThds++];

	pBT->ThdId = GetCurrentThreadId();

	pBT->Buffer = Buffer;

	pBT->SzBuffer = SzBuffer;

	pBT->iB = 0;

}	//	StartBufferErrors()


/*-------------------------  StopBufferErrors()  --------------------------*/
/*
	See comments in error.h.
*/
void	StopBufferErrors()
{
	uint	i;		DWORD	ThdId = GetCurrentThreadId();

	for ( i = 0; i < nBufferThds; i++ )
	{
		if ( BuffThd[i].ThdId == ThdId )  break;
	}

	if ( i >= nBufferThds )  return;

	memmove ( &BuffThd[i],
			  &BuffThd[i + 1], (--nBufferThds - i) * sizeof(BufferThd) );

	BufferThd *	pBT = &BuffThd[nBufferThds];

	pBT->ThdId = 0;

}	//	StopBufferErrors()


/*--------------------------------  M()  ----------------------------------*/
/*
	See comments in error.h.
*/
int		M ( int AE, const char * Func, const char * Fmt, ... )
{
	const char *	sW = F("M()");

	if ( AE == AE_INT_ERROR )
	{
		bcder ( sW, "Invalid AE." );
	}

	char	Buf1 [MAX_ERROR_STR_LEN + 1];		int	i;
	
	va_list	alist;

	va_start ( alist, Fmt );

	if ( (i = _vsnprintf ( Buf1, MAX_ERROR_STR_LEN, Fmt, alist )) < 0 )
	{
		bcder ( sW, "Buf1 is too small." );

		i = MAX_ERROR_STR_LEN;
	}

	Buf1[MAX_ERROR_STR_LEN] = '\0';

	va_end ( alist );


	//	Take out cr/lf and any punctuation at the end.
	//
	RemoveCrLf ( i, Buf1, sizeof(Buf1) );	


	//	return ToPool ( AE, 0, Buf1 );
	//
	//	11-Jan-1999
	//
	int	ENo = ToPool ( AE, 0, Buf1 );

		Trace ( dtError, ENo, Func, Buf1 );

	Buffer ( Buf1, i );		//	2004-Aug-05

	return ENo;

}	//	M()


/*--------------------------------  E()  ----------------------------------*/
/*
	See comments in error.h.
*/
int		E ( const char * Func, const char * Fmt, ... )
{
	const char *	sW = F("E()");

	char	Buf1 [MAX_ERROR_STR_LEN + 1];		int	i;

	nE++;

	va_list	alist;

	va_start ( alist, Fmt );

	if ( (i = _vsnprintf ( Buf1, MAX_ERROR_STR_LEN, Fmt, alist )) < 0 )
	{
		bcder ( sW, "Buf1 is too small." );

		i = MAX_ERROR_STR_LEN;
	}

	Buf1[MAX_ERROR_STR_LEN] = '\0';

	va_end ( alist );


	//	Take out cr/lf and any punctuation at the end.
	//
	RemoveCrLf ( i, Buf1, sizeof(Buf1) );	


	int	ENo = ToPool ( AE_INT_ERROR, Func, Buf1 );

	//	To diagnostics.
	//
		Trace ( dtError, ENo, Func, Buf1 );

	Buffer ( Buf1, i );		//	2004-Aug-05

	return ENo;
	
}	//	E()


/*-----------------------------  Notice()  ---------------------------------*/
/*
	See comments in error.h.
*/
int		Notice ( const char * Func, const char * Fmt, ... )
{
	const char *	sW = F("Notice()");

	char	Buf1 [MAX_ERROR_STR_LEN + 1];		int	i;

	va_list	alist;

	va_start ( alist, Fmt );

	if ( (i = _vsnprintf ( Buf1, MAX_ERROR_STR_LEN, Fmt, alist )) < 0 )
	{
		bcder ( sW, "Buf1 is too small." );

		i = MAX_ERROR_STR_LEN;
	}

	Buf1[MAX_ERROR_STR_LEN] = '\0';

	va_end ( alist );


	//	Take out cr/lf and any punctuation at the end.
	//
	RemoveCrLf ( i, Buf1, sizeof(Buf1) );	


	//	To diagnostics.
	//
		Trace ( dtNotice, -1, Func, Buf1 );

	Buffer ( Buf1, i );		//	2004-Aug-05

	return 0;
	
}	//	Notice()



/*--------------------------------  T()  ----------------------------------*/
/*
	See comments in error.h.
*/
int		T ( int ENo, const char * Func, const char * Fmt, ... )
{
	const char *	sW = F("T(1)");

	if ( GetEI ( ENo, 0, 0 ) == 0 )
	{
		bcder ( sW, "Bad ENo." );
	}

	char	Buf1 [MAX_ERROR_STR_LEN + 1];		int i;
	
	va_list	alist;

	va_start ( alist, Fmt );

	if ( (i = _vsnprintf ( Buf1, MAX_ERROR_STR_LEN, Fmt, alist )) < 0 )
	{
		bcder ( sW, "Buf1 is too small." );

		i = MAX_ERROR_STR_LEN;
	}

	Buf1[MAX_ERROR_STR_LEN] = '\0';

	va_end ( alist );


	//	Take out cr/lf and any punctuation at the end.
	//
	RemoveCrLf ( i, Buf1, sizeof(Buf1) );	


	//	To diagnostics.
	//
		Trace ( dtError, ENo, Func, Buf1 );

	
	Buffer ( Buf1, i );		//	2004-Aug-05

	return ENo;
	
}	//	T()



/*--------------------------------  T()  ----------------------------------*/
/*
	See comments in error.h.
*/
int		T ( int ENo, const char * Func )
{
	const char *	sW = F("T(2)");

	if ( GetEI ( ENo, 0, 0 ) == 0 )
	{
		bcder ( sW, "Bad ENo." );
	}

	
	char	Buf1 [MAX_ERROR_STR_LEN + 1];		int i;

	i = sprintf ( Buf1, "(trace)" );

	
	//	To diagnostics.
	//
		Trace ( dtError, ENo, Func, Buf1 );

	
	Buffer ( Buf1, i );		//	2004-Aug-05

	return ENo;
	
}	//	T()



/*-------------------------------  bcder()  --------------------------------*/
/*
	See comments in error.h.
*/
int		bcder ( const char * Func, const char * Fmt, ... )
{
	const char *	sW = F("bcder()");

	char	Buf1 [MAX_ERROR_STR_LEN + 1];		int i;
	
	va_list	alist;

	va_start ( alist, Fmt );

	if ( (i = _vsnprintf ( Buf1, MAX_ERROR_STR_LEN, Fmt, alist )) < 0 )
	{
		bcder ( sW, "Buf1 is too small." );

		i = MAX_ERROR_STR_LEN;
	}

	Buf1[MAX_ERROR_STR_LEN] = '\0';

	va_end ( alist );


	//	Take out cr/lf and any punctuation at the end.
	//
	RemoveCrLf ( i, Buf1, sizeof(Buf1) );	


	//	To diagnostics.
	//
		Trace ( dtError, -1, Func, Buf1 );

	Buffer ( Buf1, i );		//	2004-Aug-05

	return -1;

}	//	bcder()



/*------------------------------  syser()  ---------------------------------*/
/*
	See comments in error.h.
*/
int		syser ( const char * Func, const char * Fmt, ... )
{
	const char *	sW = F("syser()");

	char	Buf1 [1024];
	char	Buf2 [1024];
	va_list	alist;
	int		i;

	nE++;

#ifdef	WIN32
	LPVOID	lpMessageBuffer;

	int		Err = GetLastError();

	FormatMessage (   FORMAT_MESSAGE_ALLOCATE_BUFFER 
					| FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					Err,
					MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ), 
					(LPTSTR) &lpMessageBuffer,
					0,
					NULL );

	va_start ( alist, Fmt );

	if ( _vsnprintf ( Buf1, sizeof(Buf1) - 1, Fmt, alist ) < 0 )
	{
		bcder ( sW, "Buf1[] is too small." );
	}

	Buf1[sizeof(Buf1) - 1] = '\0';

	va_end ( alist );

	i = _snprintf ( Buf2, sizeof(Buf2) - 1, 
					"%s  Sys error code: %d  %s", 
					Buf1, Err, lpMessageBuffer );

	if ( i < 0 )
	{
		bcder ( sW, "Buf2[] is too small." );

		i = sizeof(Buf2);
	}

	LocalFree ( lpMessageBuffer );
#else
	#ifndef JS
	char	Buf0 [1024];	char * pB0 = Buf0;

	memset ( Buf0, 0, sizeof(Buf0) );

	int		Err = errno;

	if (    ((_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600))
		 && ! _GNU_SOURCE )
	{
		if ( strerror_r ( Err, Buf0, sizeof(Buf0) ) )
		{
			int	Err2 = errno;
			sprintf ( Buf0, "(strerror_r() failed - errno %d)", Err2 );
		}
	}
	else
		pB0 = strerror_r ( Err, Buf0, sizeof(Buf0) );

	Buf0[sizeof(Buf0) - 1] = '\0';

	va_start ( alist, Fmt );

	if ( _vsnprintf ( Buf1, sizeof(Buf1) - 1, Fmt, alist ) < 0 )
	{
		bcder ( sW, "Buf1[] is too small." );
	}

	Buf1[sizeof(Buf1) - 1] = '\0';

	va_end ( alist );

	i = _snprintf ( Buf2, sizeof(Buf2) - 1, 
			  		"%s  %s", 
					Buf1, pB0 );

	if ( i < 0 )
	{
		bcder ( sW, "Buf2[] is too small." );

		i = sizeof(Buf2);
	}
	#else
		strcpy ( Buf2, "System error." );
	#endif
#endif

	Buf2[sizeof(Buf2) - 1] = '\0';

	
	//	Take out cr/lf and any punctuation at the end.
	//
	RemoveCrLf ( i, Buf2, sizeof(Buf2) );	


	return E ( Func, Buf2 );

}	//	syser()


/*----------------------------  sysnotice()  -------------------------------*/
/*
	See comments in error.h.
*/
int		sysnotice ( const char * Func, const char * Fmt, ... )
{
	const char *	sW = F("sysnotice()");

	char	Buf1 [1024];
	char	Buf2 [1024];
	va_list	alist;
	int		i = 0;

#ifdef	WIN32
	LPVOID	lpMessageBuffer;

	int		Err = GetLastError();

	FormatMessage (   FORMAT_MESSAGE_ALLOCATE_BUFFER 
					| FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					Err,
					MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ), 
					(LPTSTR) &lpMessageBuffer,
					0,
					NULL );

	va_start ( alist, Fmt );

	if ( _vsnprintf ( Buf1, sizeof(Buf1) - 1, Fmt, alist ) < 0 )
	{
		bcder ( sW, "Buf1[] is too small." );
	}

	Buf1[sizeof(Buf1) - 1] = '\0';

	va_end ( alist );

	i = _snprintf ( Buf2, sizeof(Buf2) - 1, 
			  		"%s  %s", 
					Buf1, lpMessageBuffer );

	LocalFree ( lpMessageBuffer );
#else
	#ifndef JS
		char	Buf0 [1024];
		int		Err = errno;

		if ( (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE )
		{
			if ( strerror_r ( Err, Buf0, sizeof(Buf0) ) )
			{
				int	Err2 = errno;
				sprintf ( Buf0, "(strerror_r() failed - errno %d)", Err2 );
			}
		}
		else
			strerror_r ( Err, Buf0, sizeof(Buf0) );

		Buf0[sizeof(Buf0) - 1] = '\0';

		va_start ( alist, Fmt );

		if ( _vsnprintf ( Buf1, sizeof(Buf1) - 1, Fmt, alist ) < 0 )
		{
			bcder ( sW, "Buf1[] is too small." );
		}

		Buf1[sizeof(Buf1) - 1] = '\0';

		va_end ( alist );

		i = _snprintf ( Buf2, sizeof(Buf2) - 1, 
				  		"%s  %s", 
						Buf1, Buf0 );
	#else
		strcpy ( Buf2, "System error." );
	#endif
#endif

	if ( i < 0 )
	{
		bcder ( sW, "Buf2[] is too small." );

		i = sizeof(Buf2);
	}

	Buf2[sizeof(Buf2) - 1] = '\0';

	
	//	Take out cr/lf and any punctuation at the end.
	//
	RemoveCrLf ( i, Buf2, sizeof(Buf2) );	


	//	To diagnostics.
	//
		Trace ( dtNotice, -1, Func, Buf1 );

	Buffer ( Buf1, i );		//	2004-Aug-05

	return 0;

}	//	sysnotice()


/*-------------------------------  GetE()  --------------------------------*/
/*
	See comments in error.h.
*/
int		GetE ( int & ENo, int & AE, char * sE, uint SizeE )
{
	const char *	sW = F("GetE()");

	int	R = 0;

	AE = AE_NO_ERROR;

	ErrItem *	pEI = GetEI ( ENo, 0, 0 );
	
	if ( pEI == 0 )  
	{
		AE = AE_ERROR;			//	Bad ENo.

		ENo = E ( sW, "Bad ENo." );

		R = -1;
	}

	if ( sE == 0 )
	{
		bcder ( sW, "Bad sE." );

		R = -1;
	}

	if ( SizeE < 1 )
	{
		bcder ( sW, "Bad SizeE." );

		R = -1;
	}

	if ( R < 0 )
		return R;


	AE = pEI->Type;

	uint	nESBytes = strlen ( pEI->ErrStr ) + 1;

	if ( nESBytes > SizeE )
	{
		bcder ( sW, "SizeE is too small." );

		int	nEC = SizeE - 1;

		strncpy ( sE, pEI->ErrStr, nEC );	sE[nEC] = '\0';

		R = nESBytes;
	}
	else
		strcpy ( sE, pEI->ErrStr );

	_ASSERTE_IF ( iEINext > 0 )
	{
		bcder ( sW, "iEINext <= 0." );

		return -1;
	}

	EIIdx [--iEINext] = ENo - 1;

	return R;

}	//	GetE()


/*-------------------------------  GetAE()  -------------------------------*/
/*
	See comments in error.h.
*/
int		GetAE ( int ENo )
{
	ErrItem *	pEI = GetEI ( ENo, 0, 0 );
	
	if ( pEI == 0 )  return AE_ERROR;

	return pEI->Type;

}	//	GetAE()


/*-------------------------------  DelE()  --------------------------------*/
/*
	See comments in error.h.
*/
void	DelE ( int ENo )
{
	const char *	sW = F("DelE()");

	ENo--;

//	if ( (ENo < 0) || (ENo >= iEINext) || (ENo >= MAX_ERR_ITEMS) )
	if ( (ENo < 0) ||                     (ENo >= MAX_ERR_ITEMS) )
	{
		bcder ( sW, "Bad ENo." );

		return;
	}

	_ASSERTE_IF ( iEINext > 0 )
	{
		bcder ( sW, "iEINext <= 0." );

		return;
	}

	EIIdx [--iEINext] = ENo;

}	//	DelE()





//	Private.


/*------------------------------  ToPool()  --------------------------------*/
/*
*/
static int	ToPool ( int AE, const char * Func, const char * Buf1 )
{
	const char *	sW = F("ToPool()");

	//	Find an ErrItem slot.
	//
	ErrItem *	pEI;	int	iEI, nC;

	if ( iEINext < 0 )
	{
		bcder ( sW, "iEINext < 0." );

		return -1;
	}

	if ( iEINext >= MAX_ERR_ITEMS )
	{
		bcder ( sW, "iEINext (%d) >= MAX_ERR_ITEMS (%d)", 
					iEINext, MAX_ERR_ITEMS );

		return -1;
	}

	if ( iEINext == HW )
	{
		pEI = &EIPool [iEI = iEINext++];

		HW  = iEINext;
	}
	else
	{
		pEI = &EIPool [iEI = EIIdx [iEINext++]];
	}


	if ( Func )
	{
#ifdef	JS_WIN32
		nC = _snprintf_s ( pEI->ErrStr, MAX_ERROR_STR_LEN, _TRUNCATE,
						   "%s  %s", Func, Buf1  );
#else
		nC = _snprintf ( pEI->ErrStr, MAX_ERROR_STR_LEN, 
						 "%s  %s", Func, Buf1  );
#endif
	}
	else
#ifdef	JS_WIN32
		nC = _snprintf_s ( pEI->ErrStr, MAX_ERROR_STR_LEN, _TRUNCATE, 
						   "%s", Buf1  );
#else
		nC = _snprintf ( pEI->ErrStr, MAX_ERROR_STR_LEN, "%s", Buf1  );
#endif

	if ( nC < 0 )
	{
		bcder ( sW, "ErrStr is too small." );
	}

	pEI->ErrStr[MAX_ERROR_STR_LEN] = '\0';

	pEI->Type = AE;

	return iEI + 1;

}	//	ToPool()


/*-------------------------------  GetEI()  --------------------------------*/
/*
*/
static ErrItem *	GetEI ( int ENo, char * sE, uint SizeE )
{
	const char *	sW = F("GetEI()");

	ENo--;

//	if ( (ENo < 0) || (ENo >= iEINext) || (ENo >= MAX_ERR_ITEMS) )
	if ( (ENo < 0) ||                     (ENo >= MAX_ERR_ITEMS) )
	{
		if ( sE )
		{
			const char *	ES = "Bad ENo.";

			if ( strlen ( ES ) > (SizeE - 1) )
			{
				bcder ( sW, "%s  SizeE is too small.", ES );
			}
			else
			{
				bcder ( sW, "%s", ES );

				strcpy ( sE, ES );
			}
		}

		return 0;
	}

	return &EIPool [ENo];

}	//	GetEI()
	

/*------------------------------  Trace()  --------------------------------*/
/*
*/
static void	Trace ( int Type, int ENo, const char * Func, const char * Text )
{
	//	2004-Jun-19		Based on CDiags::Print().

	char	Buffer1 [1024];		int	iC;
	char	Buffer2 [1024];

	//	As in CDiags::Print(), take the file out of the Func.
	//
	//	The file path is expected to be in () and then 2 spaces and then
	//	the function name.  We want the function name.
	//
	iC = 0;

	if ( Func[0] == '(' )
	{
		while ( Func[iC] && (Func[iC] != ')') )  iC++;

		if (    (Func[iC + 0] == ')')
			 && (Func[iC + 1] == ' ')
			 && (Func[iC + 2] == ' ') )
		{
			iC += 3;
		}
	}

#ifdef JS_WIN32
	_snprintf_s ( Buffer1, sizeof(Buffer1), _TRUNCATE, &Func[iC] );
#else
	_snprintf ( Buffer1, sizeof(Buffer1) - 1, &Func[iC] );
#endif

	Buffer1[sizeof(Buffer1) - 1] = '\0';


	const char *	sType = "";

	switch ( Type )
	{
		case dtBCDError:	sType = "BCDError";		break;
		case dtError:		sType = "Error";		break;
		case dtSysError:	sType = "SysError";		break;
		case dtPrint:		sType = "Diag";			break;
		case dtCodeSystem:	sType = "CodeSystem";	break;
		case dtData:		sType = "Data";			break;
		case dtExecutive:	sType = "Executive";	break;
		case dtModeling:	sType = "Modeling";		break;
		case dtCodePieces:	sType = "CodePieces";	break;
		case dtBigPicture:	sType = "BigPicture";	break;
		case dtNotice:		sType = "Notice";		break;
		default:

			sType = "?";
	}

	if ( ENo < 0 )
	{
#ifdef JS_WIN32
		_snprintf_s ( Buffer2, sizeof(Buffer2), _TRUNCATE,
					  " %-11s  %s  %s", 
					  sType, Buffer1, Text );
#else
		_snprintf ( Buffer2, sizeof(Buffer2) - 1, 
					" %-11s  %s  %s", 
					sType, Buffer1, Text );
#endif
	}
	else
#ifdef JS_WIN32
		_snprintf_s ( Buffer2, sizeof(Buffer2), _TRUNCATE,
					  " %-11s  %-3d  %s  %s", 
					  sType, ENo, Buffer1, Text );
#else
		_snprintf ( Buffer2, sizeof(Buffer2) - 1, 
					" %-11s  %-3d  %s  %s", 
					sType, ENo, Buffer1, Text );
#endif

	Buffer2[sizeof(Buffer2) - 1] = '\0';

#ifdef	_GC
	printf ( "%s\n", Buffer2 );
#elif	!WIN32
	printf ( "%s\n", Buffer2 );
#else
	#ifdef	_DEBUG
		_RPT1 ( _CRT_WARN, "%s\n", Buffer2 );
	#endif
#endif

}	//	Trace()


/*----------------------------  RemoveCrLf()  ------------------------------*/
/*
*/
static void	RemoveCrLf ( int & i, char Buf[], uint SzBuf )
{
	if ( i < (int)SzBuf )
	{
		while ( (i > 0) && ! isprint ( Buf[i - 1] ) )
		{
			Buf[i - 1] = '\0';

			i--;
		}

	//	if ( Buf[i - 1] == '.' )  Buf[i - 1] = '\0';
	}

}	//	RemoveCrLf()


/*-----------------------------  Buffer()  --------------------------------*/
/*
*/
static void	Buffer ( const char * Msg, uint nC )
{
	if ( nBufferThds == 0 )  return;

	uint	i;		DWORD	ThdId = GetCurrentThreadId();

	for ( i = 0; i < nBufferThds; i++ )
	{
		if ( BuffThd[i].ThdId == ThdId )  break;
	}

	if ( i >= nBufferThds )  return;


	BufferThd *	pBT = &BuffThd[i];

	uint nExtraC = 3;				//	2 \n and a \0.

	if ( pBT->iB + nC + nExtraC > pBT->SzBuffer )  
	{
		if ( pBT->iB + nExtraC > pBT->SzBuffer )  return;

		nC = pBT->SzBuffer - (pBT->iB + nExtraC);
	}


	strncpy ( &pBT->Buffer[pBT->iB], Msg, nC );

	pBT->Buffer[pBT->iB += nC] = '\n';

	pBT->Buffer[pBT->iB +=  1] = '\n';

	pBT->Buffer[pBT->iB +=  1] = '\0';

}	//	Buffer()


//	error.cpp

