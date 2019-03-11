/*
 	not_win32.cpp
*/
#ifndef JS 
#include	<stdlib.h>
#include	<sys/types.h>
#include	<time.h>
#include	<sys/ipc.h>
#include	<sys/sem.h>
#endif

#ifndef JS_WIN32
#include	<unistd.h>
#endif

#include	"abb_rr_not_win32.h"

#include	"abb_rr_rs_defs.h"


#include	"abb_rr_error.h"


//	A macro for specifiying filename and function name in diagnostics calls.
//
#define	F(f)	"("__FILE__")" "  " f


int		InitializeCriticalSection ( CRITICAL_SECTION * pCS )
{
	pCS->RecursionCount = 0;

#ifndef JS 
	g_static_rec_mutex_init ( &pCS->Mtx );
#endif

	pCS->OwningThread = 0;

	return 0;

}	//	InitializeCriticalSection()	


int		DeleteCriticalSection ( CRITICAL_SECTION * pCS )
{
	if ( pCS == 0 )  return 0;

#ifndef JS 
	g_static_rec_mutex_free ( &pCS->Mtx );
#endif

	return 0;

}	//	DeleteCriticalSection()


int		EnterCriticalSection ( CRITICAL_SECTION * pCS )
{
	_ASSERTE_IF ( pCS )  return -1;

#ifndef JS 
	g_static_rec_mutex_lock ( &pCS->Mtx );
#endif

	pCS->RecursionCount++;

#ifndef JS 
	if ( pCS->RecursionCount == 1 )  pCS->OwningThread = g_thread_self();
#else
	if ( pCS->RecursionCount == 1 )  pCS->OwningThread = 1;
#endif

	return 0;

}	//	EnterCriticalSection()


int		TryEnterCriticalSection ( CRITICAL_SECTION * pCS )
{
	_ASSERTE_IF ( pCS )  return -1;

#ifndef JS 
	if ( ! g_static_rec_mutex_trylock ( &pCS->Mtx ) ) return 0;
#endif

	pCS->RecursionCount++;

#ifndef JS 
	if ( pCS->RecursionCount == 1 )  pCS->OwningThread = g_thread_self();
#else 
	if ( pCS->RecursionCount == 1 )  pCS->OwningThread = 1;
#endif

	return 1;

}	//	TryEnterCriticalSection()


int		LeaveCriticalSection ( CRITICAL_SECTION * pCS ) 
{
	_ASSERTE_IF ( pCS )  return -1;

	_ASSERTE_IF ( pCS->RecursionCount > 0 )  return -1;

#ifndef JS 
	g_static_rec_mutex_unlock ( &pCS->Mtx );
#endif

	pCS->RecursionCount--;

	if ( pCS->RecursionCount > 0 )  return 0;

	pCS->OwningThread = 0;

	return 0;

}	//	LeaveCriticalSection()



DWORD	GetCurrentProcessId()
{
#ifndef JS 	
	return (DWORD)getpid();
#else
	return 1;
#endif

}	//	GetCurrentProcessId()


DWORD	GetCurrentThreadId()
{
#ifndef JS
	return (DWORD)g_thread_self();
#else
	return 1;
#endif

}	//	GetCurrentThreadId()


void	Sleep ( uint MilliSeconds )
{
#ifndef JS
	usleep ( MilliSeconds * 1000 );	
#endif

}	//	Sleep()



DWORD	GetTickCount()
{
#ifndef JS
	const char * sW = F("GetTickCount()");

	struct timespec ts;

	if ( clock_gettime ( CLOCK_MONOTONIC, &ts ) != 0 )
	{
		DelE ( syser ( sW, "clock_gettime() " ) );
		return 0;
	}

	return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
#else
	return 0;
#endif

}	//	GetTickCount()


bool	GetSemaphore ( int sem_id )
{
#ifndef JS
	struct sembuf SB; 	SB.sem_num =  0;
						SB.sem_op  = -1;
						SB.sem_flg =  0;

	if ( semop( sem_id, &SB, 1 ) != -1 )
		return true;

	return false;
#else
	return true;
#endif

}	//	GetSemaphore()


int 	ReleaseSemaphore ( int sem_id )
{
#ifndef JS
	const char * sW = F("ReleaseSemaphore()");

	struct sembuf SB;	SB.sem_num =  0;
						SB.sem_op  =  1;
						SB.sem_flg =  0;

	if ( semop( sem_id, &SB, 1 ) == -1 )
		return syser ( sW, "semop()" );
#endif

	return 0;

}	//	ReleaseSemaphore()


//	not_win32.cpp

