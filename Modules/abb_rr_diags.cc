/*
		 1         2         3         4         5         6         7
12345678901234567890123456789012345678901234567890123456789012345678901234567

	File:	diags.cpp

	25-Mar-98

	See comments in diags.h.
*/

/*------------------------------- Dependencies  ---------------------------*/

#ifndef JS
	#ifdef	WIN32
	#include	<direct.h>
	#include	<io.h>
	#include	<Tchar.h>
	#else
	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<unistd.h>
	#include	<errno.h>
	#endif
#else
	#include <ctype.h>
#endif

#include	<stdio.h>
#include	<stdarg.h>
#include	<string.h>
#include	<stdlib.h>

#include	"abb_rr_diags.h"

//	End Dependencies



#define PORT


//	A macro for specifiying filename and function name in diagnostics
//	calls.
//
#define	F(f)	"(" __FILE__ ")" "  " f



/*--------------------------------  Print()  -------------------------------*/
/*
*/
PORT void	Print ( int Level, const char * Func, const char * Fmt, ... )
{
//	const char *	sW = F("Print()");

	char	Buf [1024 * 4];		int	n, L = 0;

	va_list	alist;

	va_start ( alist, Fmt );

	if ( (n =  vsnprintf ( &Buf[L], sizeof(Buf) - (L + 1),  
//						   Fmt, (char *)alist ))
						   Fmt,         alist ))
		 < 0 )
	{
		L = sizeof(Buf) - 1;
	}
	else
		L += n;
		
	va_end ( alist );

	Buf[L] = '\0';

    printf ( "%d %s %s\n", Level, Func, Buf );

}	//	Print()


