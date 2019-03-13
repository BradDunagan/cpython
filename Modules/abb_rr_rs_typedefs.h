/*
	File:	rs_typedefs.h

	Simple, basic type definitions possibly used by any part of R & S - app 
	processes, the DS, critical GPs, etc..

	These are basically just abreviations to save space.  For example,

			for ( unsigned int i; ...

		can be replaced with

			for ( uint i; ...

	Also, this hopefully defines basic data types and differences between
	int and long.  On 32 bits systems long is 32 bits.  On 64 bit systems
	a long may be 64 bits.  
	
	With these definitions slng and ulng will always be 64 bits.

	And, for that matter, sint and uint will always be 32 bits.
*/

#ifndef		__RS_TYPEDEFS_H
#define		__RS_TYPEDEFS_H

#include    <stdio.h>

#include	"abb_rr_rs_defs.h"

#include    "abb_rr_basic_typedefs.h"


#ifndef	BOOL
#define	BOOL				int
#endif

#ifndef	TRUE
#define	TRUE				1
#endif

#ifndef	FALSE
#define	FALSE				0
#endif


#define	SIZEOF_SSRT			2
#define	SIZEOF_USRT			2
#define	SIZEOF_SINT			4
#define	SIZEOF_UINT			4
#define	SIZEOF_SLNG			8
#define	SIZEOF_ULNG			8


//	SID
//
//	Small Id.  Smaller than a RID.
//
//	Originated in the MRD stuff (original files versions.h, .cpp) for 
//	identifying versions of DRMs.
//
//	16-Nov-98
//
//	Microsoft uses SID in WINNT.H.
//
//	LID:	Little Id.
//
typedef	uint				LID;
#define	SIZEOF_LID			SIZEOF_UINT

#define	NULL_LID			0x00000000

#define	MIN_LID				0x00000001

//	Reserve flag bits at the top for such things as HS_NOT_IN_DSC, for
//	example.
//
#define	MAX_LID				0x0FFFFFFF		


//	An app Id is made of the process Id and the time the process -
//	the application program - started.
//
typedef ulng	AppId;


//	12-Nov-98
//
//	Originally used by the DRMHist and DHE structures of the record MRD
//	stuff.  See versions.h.
//
/*	1999-Jun-28		Now using LHRTm.  See rs_time.h,cpp.

typedef	unsigned __int32	DHTm;	//	DRM history time.  Seconds.  
									//	Range is roughly 136 years.
*/

//	Cache Index Item.
//
typedef	usrt	CIItem;


//	16-Nov-98
//
//	"Handle" to data.
//
//	This defines the value the app can use to reference DRMs.  The app
//	must never change this value - it is passed to the app when a new
//	DRM is created and the app uses it to reference the DRM when, for
//	example, the DRM is to be loaded from the DB file.
//
typedef	uint	HD;

#define	MAX_HD	0xFFFFFFFF



//	20-Nov-98
//
//	"Handles" which identify a DFI write and read operations.
//
typedef	uint	FWH;

typedef	uint	FRH;



//	08-Feb-1999
//
//	Reconciliation info passed to the application from the DS.
//
struct	RccilInfo
{
	uint	CtxId;				//	Context current server is used in.

	HD		hCD;				//	Client in the context.

	HD		hSvr;				//	Handle of the server.

	void *	pSvrProposed;		//	Other server with same handle.

	uint	CtxIdOfProposed;	//	Context other server used in.

	int		bAccepted;			//	Set non-zero if reconciled.

};	//	RccilInfo


//	08-Feb-1999
//
//	For testing/developing reconciliation.
//
typedef	int (*RccilFunc) ( void *, RccilInfo * );




//	NameEtc Structure
//
//	1999-Jul-05		See other comments of this date.
//
//	The NameEtc structure data is an optional part of the MRD.  If it exists
//	it is stored in shared memory (with a reserved LID) when the MRD is 
//	loaded so that applications can get at it without DS calls.
//
//	Name and possibly other information (etc) is created only if the app 
//	requests it.  This is so because not all records need it and it consumes 
//	significant memory.
//
//	On the NEF_NAME_CHANGED Flag -
//
//		If the NameEtc info for a record exists and the MRD is loaded then
//		the name etc info will - should - be gotten from the MRD - because
//		there it will be most up to date (because of possible changes
//		made by the app).  If the MRD is not loaded then the name should
//		be gotten from the Id index (II).  
//
//	->	The important thing is that if the NEF_CHANGED flag is set then the 
//		II should be updated when the MRD is unloaded.
//
//	->	How?  The II is indexed by type and name.  It will have to be updated
//		* when * the change occurs?  The app will have to make a DS call to
//		change the name?
//
//	->	Unless the original name (the name when the record is loaded) is
//		maintained.  More space taken (64 bytes! - unless a MMS allocation).
//
//	Do This -
//
//		The application context that * first * changes the name must remember
//		the previous name.  That way the previous name is stored in one place
//		and only by the single context that first changes the name.  All 
//		other contexts simply have MMS iP of iP_NULL.
//
//	->	Then the important thing is that the context that has stored the 
//		previous name must do a DS call to update the II when that context
//		is going out of scope.  It that is done and the previous name is
//		lost there will be no way to update the II because the II is sorted
//		by type and name.
//
//
//	1999-Oct-11
//
//	Added "update criteria" words.   These specify the criteria the DS
//	uses in choosing DRMs over others when UpdateContext() is called.
//
//
//	1999-Oct-13
//
//	Word(s) added for implementing synchronization with Windows' 
//	Interlocked...() functions.
//
//	->	Now, to implement this synchronization, every record must have this
//		name, etc. structure in shared memory.
//
struct	NameEtc
{
	CIItem	CI;				//	2 bytes.

	BYTE	Spare[2];

//	uint	Flags;			//	See NEF_ definitions in rs_defs.h.
	uint	NEFlags;		//	See NEF_ definitions in rs_defs.h.

	char	Name [MAX_NAME_LEN + 1];

	uint	UCFlags;		//	See UC_ definitions in rs_defs.h.

#ifdef	WIN32
	LONG	I_a;			//	For Windows' Interlocked...() functions.
#else
	int		SemId;			//	Simply use a semaphore.  For now.
#endif

	long	I_b;			//	Semaphore count.

	DWORD	ThdId;			//	Thread that has the record locked.

	uint	ThdCnt;			//	Number of times the same thread has locked
							//	the record.

	LID		Lock_hC;		//	2010-Jul-08		One context.

	//	2004-Jul-24		Offsets in shared memory.
	//
	//	Invalid if 0, else subtract 1.
	//
	uint	CUIO;			//	CtxUpdtInfo.

//	uint	RSsO;			//	RecServers.
	//
	//	2004-Aug-23, 2004-Aug-30, 2004-Sep-01
	//
	//	Making a RS in each DRM.
	//
	//	Therefore, a DRM may change size at any time (actually, when a 
	//	server is added or removed - i.e., when the DRM is duped).  And 
	//	when that happens the DRM's pB, CSA, Id should be provided
	//	back to the caller - likewise when other contexts update themselves.


	//	2004-Aug-23, 2004-Oct-18
	//
	//	For records where NEF_ALLOW_DRM_DUP and NEF_ONE_DRM are set a context
	//	may reallocate the DRM and thus change its location in shared
	//	memory.  All contexts therefore, indirectly get a pointer to the
	//	DRM via this.  This is set when such a reallocation happens.
	//
	uint	nORB;			//	Number of bytes (including hdr and DSs).
	uint	oORB;			//	Offset of One Record Base DRM.


	//	For now.  Try to keep this thing at 128 bytes.
	//
#ifdef	WIN32
	BYTE	Etc [128 - (  sizeof(CIItem)		//	CI
						+ sizeof(BYTE[2])		//	Spare
						+ sizeof(uint)			//	Flags
		                + MAX_NAME_LEN + 1		//	Name[]
						+ sizeof(uint)			//	UCFlags
						+ sizeof(PVOID)			//	I_a
						+ sizeof(long)			//	I_b
						+ sizeof(DWORD)			//	ThdId
						+ sizeof(uint)			//	ThdCnt
						+ sizeof(LID)			//	hC
						+ sizeof(uint)			//	CtxUpdtInfo offset.
					//	+ sizeof(uint) )];		//	RecServers offset.
					//
					//	2004-Aug-23, 2004-Aug-30, 2004-Sep-01

						+ sizeof(uint)			//	nORB.
						+ sizeof(uint)			//	oORB.

						)];
#else
	BYTE	Etc [128 - (  sizeof(CIItem)		//	CI
						+ sizeof(BYTE[2])		//	Spare
						+ sizeof(uint)			//	Flags
		                + MAX_NAME_LEN + 1		//	Name[]
						+ sizeof(uint)			//	UCFlags
						+ sizeof(int)			//	SemId	
						+ sizeof(long)			//	I_b
						+ sizeof(DWORD)			//	ThdId
						+ sizeof(uint)			//	ThdCnt
						+ sizeof(LID)			//	hC
						+ sizeof(uint)			//	CtxUpdtInfo offset.
					//	+ sizeof(uint) )];		//	RecServers offset.
					//
					//	2004-Aug-23, 2004-Aug-30, 2004-Sep-01

						+ sizeof(uint)			//	nORB.
						+ sizeof(uint)			//	oORB.

						)];
#endif
	
};	//	NameEtc



//	Applications use these to identify write/read control structures when
//	writing/reading to/from DRMs.  See data.h, .cpp.
//
typedef	uint	DHW;

typedef	uint	DHR;



//	Shared Memory Offset.   Offset relative to the begining of shared memory.
//
//	32 bits will be enough as long as shared memory is not greater than 4GB.
//
//	64 bit machines?  RAM < $0.10 per MB?
//
//	These are used to specify the locations of things in shared memory
//	instead of pointers.  Pointers can not easily be shared across process
//	boundaries.
//
//typedef	uint	SMO;



//	2001-Nov-14
//
//	Import Create call-back function.
//
//	Applications provide this function for the import call  in order
//	to create application defined class objects.
//
//		sIsA		Identifies the class.  All CData derived classes should
//					implement IsA() and a CId.  sIsA should match a CId of
//					a class the caller of the import should know about.
//
//		TypeId		The type Id of the record to be created.
//
//		sName		Name of the record to be created.
//
//		pData		The caller provides this pointer to the constructed 
//					object on return.
//
//	Returns ENo.
//
struct	RID;

class	CData;

typedef int		(* ImpCreateCB)( const char *		sIsA,
								 const RID &		TypeId,
								 const char *		sName,
									   CData * &	pData );


//	2006-May-22		Called after a record is imported.
//
//	Created so that CDUser::SaveData() can be called on the imported data to 
//	establish a context that can be selected.
//
//	See the end of CDEntity::Import2() for example.
//
//	Returns ENo.
//
typedef int		(* PostImpCB)( const CData * pData, void * pImporter );



//	2012-Nov-14, 17
//
//	To fully disambiguate the identity of a system you need -
//
//		The system name.
//
//		The name of the user/developer that created it (because Robot
//		Records allows the dev to create his own systems with a name that
//		may already be used by another dev).
//
//		The name of the user/developer that it is pulled from (because
//		multiple devs may have posted the system to share).
//
struct	SysSpec
{
	void	operator = ( const SysSpec & It )
	{
		int i;
		for ( i = 0; (i < SYS_MAX_NAME_PART_LEN) && It.SysNamePart[i]; i++ )
			SysNamePart[i] = It.SysNamePart[i];
		SysNamePart[i] = 0;

		for ( i = 0; (i < MAX_USER_NAME_LEN) && It.CreatedUserName[i]; i++ )
			CreatedUserName[i] = It.CreatedUserName[i];
		CreatedUserName[i] = 0;

		for ( i = 0; (i < MAX_USER_NAME_LEN) && It.PulledFromUserName[i]; i++ )
			PulledFromUserName[i] = It.PulledFromUserName[i];
		PulledFromUserName[i] = 0;
	}

	char	SysNamePart [SYS_MAX_NAME_PART_LEN + 1];

	char	CreatedUserName [MAX_USER_NAME_LEN + 1];

	char	PulledFromUserName [MAX_USER_NAME_LEN + 1];

};	//	SysSpec


#ifndef	WIN32
#define FILETIME	struct stat

#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDCLOSE				8
#define IDHELP				9
#define IDTRYAGAIN		   10
#define IDCONTINUE		   11
#define IDTIMEOUT		32000
#endif

#endif

//	rs_typedefs.h

