/*
 	not_win32.h

	To mimmic win32 critical section functionality.
*/

#ifndef	__NOT_WIN32_H
#define	__NOT_WIN32_H

#ifndef JS
#include	<glib.h>
#endif

#include	"abb_rr_rs_typedefs.h"

#define     PORT


struct	CRITICAL_SECTION
{
#ifndef JS 
	GStaticRecMutex	Mtx;

	GThread *	OwningThread;
#else
	int 		OwningThread;
#endif
	
	int			RecursionCount;

};	//	CRITICAL_SECTION


#ifndef	JS_WIN32
extern int		InitializeCriticalSection ( CRITICAL_SECTION * pCS );

extern int		DeleteCriticalSection ( CRITICAL_SECTION * pCS );

extern int		EnterCriticalSection ( CRITICAL_SECTION * pCS );

extern int		TryEnterCriticalSection ( CRITICAL_SECTION * pCS );

extern int		LeaveCriticalSection ( CRITICAL_SECTION * pCS ); 



extern DWORD	GetCurrentProcessId();

extern DWORD	GetCurrentThreadId();

extern void		Sleep ( uint MilliSeconds );


extern DWORD	GetTickCount();


extern bool		GetSemaphore ( int sem_id );

extern int 		ReleaseSemaphore ( int sem_id );
#else
PORT int		InitializeCriticalSection ( CRITICAL_SECTION * pCS );

PORT int		DeleteCriticalSection ( CRITICAL_SECTION * pCS );

PORT int		EnterCriticalSection ( CRITICAL_SECTION * pCS );

PORT int		TryEnterCriticalSection ( CRITICAL_SECTION * pCS );

PORT int		LeaveCriticalSection ( CRITICAL_SECTION * pCS ); 



PORT DWORD		GetCurrentProcessId();

PORT DWORD		GetCurrentThreadId();

PORT void		Sleep ( uint MilliSeconds );


PORT DWORD		GetTickCount();


PORT bool		GetSemaphore ( int sem_id );

PORT int 		ReleaseSemaphore ( int sem_id );
#endif

#endif

//	not_win32.h
