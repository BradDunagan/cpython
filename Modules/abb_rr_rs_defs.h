/*
	File:	rs_defs.h

	Simple, basic macro definitions possibly used by any part of R & S - app 
	processes, the DS, critical GPs, etc..
*/

#ifndef		__RS_DEFS_H
#define		__RS_DEFS_H

#include	<assert.h>


//	2003-Oct-16		_ASSERTE_IF
//
//	If _ASSERTE is defined then this is a debug build - do the _ASSERTE and
//	also, if the condintion fails, do the statement under the _ASSERTE_IF.
//
#ifndef	WIN32
#ifndef	NDEBUG
#define	_ASSERTE(cond)			\
		assert(cond)
#endif	//	NDEBUG
#endif	//	WIN32

#ifndef _ASSERTE
#define	_ASSERTE(cond)			\
		assert(cond)
#endif

#ifdef	_ASSERTE
#define	_ASSERTE_IF(cond)		\
		_ASSERTE ( cond );		\
		if ( ! (cond) )
#else
#define	_ASSERTE_IF(cond)		\
		if ( ! (cond) )
#endif

#define	MAX_COMPUTER_NAME_LEN	255

//	For specifying the address when connecting to another node with the
//	Remote2 (websockets) stuff
//
#define	MAX_DNS_NAME_LEN		255


//	2015-Mar-02
//
#define	MAX_APP_NAME_LEN		63


//	2012-Jan-24		User Name
//
//	This is the user name as specified by the customer/client in the Robot
//	Records web site.
//
#define	MAX_USER_NAME_LEN		31


//	2012-Jan-20		System Names
//
//	A system name is made of one or more "name parts".
//
//	Since the system name is also the directory where the data files are 
//	stored locally, each system name is postfixed with a \.  For example -
//
//		Barney\SandboxA\                      (not a line continuation)
//
//	The user, when specifying a system name does need to specify the \.
//
//	The \ is not included in SYS_MAX_NAME_PART_LEN.
//
#define	SYS_MAX_NAME_PART_LEN	31			//	Not including the \.

#define	SYS_MAX_DIR_NAME_PARTS	5

#define	SYS_MAX_NAME_LEN	(  (SYS_MAX_NAME_PART_LEN + 1)				\
							 * SYS_MAX_DIR_NAME_PARTS)

//	2012-Nov-15		More on System Names
//
//	Need user info to disambiguate when specifing systems.  So each system 
//	name part becomes -
//
//		<SysName>-<CreatedByUserName>-<PulledFromUserName>
//
//	For example, "Hello-Jack-Bill".
//
//	This is called a System Name Specification.  The user names must be 
//	included in each system name part.
//
#define	SYS_MAX_NAME_PART_SPEC_LEN	(  SYS_MAX_NAME_PART_LEN  + 1		\
									 + MAX_USER_NAME_LEN      + 1		\
									 + MAX_USER_NAME_LEN)

#define	SYS_MAX_NAME_SPEC_LEN	(  (SYS_MAX_NAME_PART_SPEC_LEN + 1)		\
								 * SYS_MAX_DIR_NAME_PARTS)



//	2012-Mar-07
//
//	Stored in the data file.  Not to be a book.  
//
#define	MAX_SYS_DESC_LEN		(1024 - 1)


//	2012-Dec-17
//
//	Separate workspace for each node a system runs on.
//
#define MAX_NODE_TYPE_LEN		15
#define	MAX_NODE_NAME_LEN		31
#define	MAX_NODE_DESC_LEN 		255


//	2012-Dec-17
//
//	The access part of "Attribution and Access" implementation.
//
//	So far, just "PUBLIC" is defined.  And possibly "LICENSE".
//
//	Those may be the only two.  If access is not PUBLIC then it is LICENSE.
//	LICENSE will, for now, indicate the user (the poster of the system or
//	record) requires the puller to obtain permission.
//
#define	MAX_ACCESS_LEN			7


//	2011-Aug-01		Application Names
/*
#define	APP_NAME_RR				"Robot Records"

#define	APP_NAME_RR_REPORT		"Robot Records Report"

#define	APP_NAME_RR_ONE_PE		"Robot Records One PE"
*/
//	2015-Mar-02		These are also used as directory names.
//
//	Under C:\Users\brad\AppData\Local, for example.
//
//	So, to make things easier, don't use spaces.
//
#define	APP_NAME_RR				"RobotRecords"

#define	APP_NAME_RR_ONE_PE		"RobotRecordsOnePE"

#define	APP_NAME_RR_REPORT		"RobotRecordsReport"


//	2010-Jun-10		REQUIRES_DIMLOCK
//
//	The DIM functions must be used by one thread at a time in an app
//	because each app has just one channel connected to the DS.
//
//	So, DIM functions use a critical secition object.
//
//	Sometimes a record must be locked when calling the DS.
//
//	To prevent deadlock the policy is that the DIM must be locked before 
//	any record is locked by the same thread.  I think locking the DIM is 
//	more cleanly and consistently done from the app because, generally,
//	the app controls record locks.
//
//	So this REQUIRES_DIMLOCK is placed at the top of functions that might
//	call the DIM to check that the app has locked the DIM.
//	
//	Note -
//
//		The DIM lock is effectively, for the app, a DS lock.
//
//		It is critical - to prevent deadlock - that if the app is going
//		to the DS that the app thread lock the DIM before locking any
//		record.
//
//		The reason for this is that the DS may unlock and lock other
//		records (one record locked at a time).  Another app thread
//		must not lock and hold a lock on one of those records while
//		waiting for the DIM lock.  App threads must lock the DIM first.
//
#ifdef	_ASSERTE
#define	REQUIRES_DIMLOCK											\
		_ASSERTE_IF ( (ENo = DIM_Locked ( sW )) == 0 )  return ENo;
#else
#define	REQUIRES_DIMLOCK									\
		if ( (ENo = DIM_Locked ( sW )) != 0 )  return ENo;
#endif


//	2011-Jul-01		Going public.
//
//	Using the Repository means -
//
//		Copies of records are stored "globally" in a single Repository
//		accessable through the internet.  
//
//		Records of multiple Sytems are maintained in the Repository.
//
//		Robot Records users may go to the botrcds.com website, register,
//		download the apps and connect with a System.
//
//		A user may be connect with just one System at any time.
//
//		The System ID is set in word 3 of every RID allocated.  This 
//		allows for maybe "checking out" a record from a System other 
//		than the one the user is connect to.  That is, records from any
//		System may be maintained locally in the user's DS.
//
//		Each user has a local DS.  It is not much different from that
//		where a Repository is not being used.  Except -
//
//			RIDs are allocated by the Repository.
//
//			Records are sent to the Repository when the users save them
//			in the user's context.
//
#ifndef JS
#define	USE_REPOSITORY		1
#endif

//	2011-Aug-11		On multiple GPs.
//
#define	RR_DISTRIBUTED		1



//	For displaying internal error messages.
//
#define	INTERNAL_ERROR		"Robot Records internal error.\n" \
							"\n" \
							"Please notify Dunagan Engineering."


//  This is the format string.  Takes one argument - error detail string.
//	See DoArchiveException() for an example of how this is used in a
//	call to Msg().
//
#define	INTERNAL_S_ERROR	"Robot Records internal error.\n" \
							"\n"   \
							"%s\n" \
							"\n"   \
							"Please notify Dunagan Engineering."


//	For displaying internal errors when parsing expressions.
//
#define INTERNAL_MACRO_ERROR	"Internal error.  " \
								"Please notify Dunagan Engineering."


#define INTERNAL_EXP_ERROR	"Internal error.  " \
							"Please notify Dunagan Engineering."


#define INTERNAL_IREC_ERROR	"Internal error.  " \
							"Please notify Dunagan Engineering."


//	Used in RSP hdr_chkr.cpp, and possibly elsewhere.
//
#define	INTERNAL_CHKR_ERROR	"Internal error.\n"						\
							"Please notify Dunagan Engineering."


//	Maximum number of threads serving local R & S application processes and
//	connections from DSs on other GPs.
//
//	Note that all of each app's threads communicate with the DS through one
//	channel.  So, this is equivalent to the max number of apps the DS can
//	serve - not the max sum of all app's threads.
//
//	Note also the actual number of apps the DS is able to server is always
//	one less than this because one of these threads always exists waiting
//	for a connection from an app (when an app connects another thread
//	is created).  And, at times, it may be two or three less than this due
//	to timing (the DS freeing up resources of a terminated app, etc.).
//
/*
#define	MAX_DS_THDS			3	//	Set low for testing errors and recovery.
*/
#define	MAX_DS_THDS			16	


/*
#define	MAX_THDS_PER_APP	32
*/
//	2008-Oct-01		Make this a prime number -
//
//	because thread Ids % MAX_THDS_PER_APP are used as indices for various
//	purposes.
//					
#define	MAX_THDS_PER_APP	31



//	Try to align things in shared memory so that structures work.
//
#define	DS_MEM_ALIGNMENT		8

#define	DS_MEM_ALIGN_REMAINDER(nB)	((nB % DS_MEM_ALIGNMENT)		\
									 ? DS_MEM_ALIGNMENT				\
									   - (nB % DS_MEM_ALIGNMENT)	\
									 : 0)			


//	Names of records, user indexes, etc..
//
//	Max length of any name - except, possibly, the name of a GP.
//
#define	MAX_NAME_LEN			63

//	Originally (first) used by RWO components.
//
#define	MAX_SHORT_NAME_LEN		31


//	General description.  First used in CRType to describe a record type.
//
#define	MAX_DESCRIPTION_LEN		255


//	Maximum number of R & S application processes that may run on the local 
//	GP simultaneously.
//
#define	MAX_APPS_PER_GP			15


#define	MAX_ERROR_STR_LEN		511



//	1999-May-14
//
//	Deals with the "temporary context" issue.  Instead of having thread Id
//	as part of context, one rule:
//
//		1	There may be just one instance of data of the same record in the 
//			same context in existence at any time in the same thread.
//
//	This means that if a thread wants multiple instances of the same record 
//	then it must instantiate the record in different contexts.
//
//	ContextId, in versions2.h .cpp, counts threads.  If a thread attempts to 
//	discharge the record in a particular context and there other threads 
//	using the record in the same context then the record will not be 
//	discharged.
//
/*
#define	MAX_THREADS_PER_CONTEXT	16
*/
//
//	2004-Jan-25		Disregards apps.
//
//	See DRM and DRM_Client structures in mrd.h.  This is max threads 
//	regardless of which application the thread is of.
//
//	Therefore, increased from 16 to ...
//
//	Keep it low for now.  See DRM_Client.  When exceeded implement a more
//	efficient thread/ref count in DRM_Client.
//
/*
#define	MAX_THREADS_PER_CLIENT	3
*/
//	2007-Jan-07		Torture par 20.  Temp increase.
//
#define	MAX_THREADS_PER_CLIENT	30


//	Flags implemented in the first word of every DRM.
//
//	1999-Jun-28
//
//	See HWN of this date.  Adding 3 additional words to make a 4 word
//	"header" for each DRM.
//
//		Word 0:		Flags word.
//		Word 1:		Change count.
//		Word 2:		Offset, in bytes, to first MCS relative to Flags word.
//		Word 3:		Context Id that has set the DF_ONE_CONTEXT flag bit.
//
//	2000-May-09
//
//		Word 3:		Offer-submit discrimination.  See below.
//
//	2002-Oct-10
//
//		Word 2:		MCS stuff not used. This word available?
//
#define	DF_NUM_HDR_WORDS	4

#define	DF_NUM_HDR_BYTES	(DF_NUM_HDR_WORDS * sizeof(uint))

#define	DF_SET1(w,f)		((w) |=  f)	//	Sets the flag.

#define	DF_SET0(w,f)		((w) &= ~f)	//	Unsets the flag.

#define	DF_IS_SET(w,f)		((w) &   f)	//	Non-zero if the flag is set.

/*
#define	DF_CHANGED		0x80000000		
*/
#define	DF_CHANGED		0x84000000		
//
//	Indicates data has changed and DRM should be written to the DB.  Set 0
//	by the DS when the DRM is loaded from the DB file.  App must set this to
//	1 so that the DS will write changes to the DB file when the DRM is 
//	deleted from the cache.
//
//	2011-Dec-19	
//
//	Added bit 0x04000000 for a separate Share flag.  This bit is reset when
//	the DRM is shared.  See DF_SHARE_CHANGE.
//
//	Bit 0x80000000 is reset when the DRM is written locally.  See 
//	DF_CHANGE_WRITTEN.

#define	DF_OBSOLETE		0x40000000		
//
//	Indicates the DRM is obsolete and that there is a newer version.  When
//	the DRM is loaded the DS sets this flag to 0 if the DRM is the only or
//	newest version, otherwise the DS sets this to 1.  An app context sets
//	this to 1 when it creates a new version of the DRM.  Other contexts check
//	this bit to see if the DRM they are using is the latest.  Apps must not
//	set this bit to 0.  When set it remains set.

#define	DF_ONE_CONTEXT	0x20000000			//	Obsolete
//
//	A context sets this flag when it does not want any other context to
//	use the DRM.  For example, if a DRM has been duplicated and many critical
//	changes are about to be made then the context may set this flag until
//	all changes have been made.
//
//	The context Id that has set this flag is maintained in header word 3.
//
//	Obsolete (at least for ODef DRMs) -
//
//		The DF_ONE_CONTEXT flag is unnecessary.  The ODef text is 
//		buffered by the editor.  When the CheckNtrp "publishes" 
//		the text * then * the DRM is dupped and the new DRM contains 
//		the changed text.  Ntrps must then update their context to 
//		use the text of the new DRM.
//
//	Othere records may still use it?  Seems plausible.

#define	DF_HISTORY		0x10000000		
//
//	Indicates history on the DRM is maintained.  That is, when ever the
//	DRM is released by a context something in its history is added or
//	updated.
//
//	Originally provided for RSP source code text DRMs.  History is used to
//	detect newer such DRMs.

#define	DF_DNU			0x08000000
//
//	2000-May-09
//
//	See comments of this date below.


#define	DF_WRITE_CHANGE	0X80000000
//
//	* Used when DRM is written. *
//	
//	To reset the 0x80000000 bit to zero when the DRM is written.

#define	DF_SHARE_CHANGE	0x04000000
//
//	* Used when DRM is shared. *
//	
//	To reset the 0x04000000 bit to zero when the DRM is shared.
//	
//	Another change bit that can be set after the DRM is posted to Share.
//	(Don't want to reset all the DF_CHANGED bits after posting to Share because
//	we want to stil save the DRM locally.)
//
//	This flag is set when a change occurs.
//
//	This bit is reset (to 0) only when the DRM is posted to Share.
//
//	The DF_CHANGE_WRITTEN bit is reset only when the DRM is written locally.
//
//	Also, note that since ChgCnt is only for sharing, ChgCnt is incremented 
//	only when this bit is set during a post to Share.


#define	DF_UR_OFFER		0x00000003		//	Two bits.

#define	DF_UR_ATTEMPT	0x0000000C		//	Two bits.

#define	DF_SET_UR_OFFER(w,v)	((w) = (((w) & ~DF_UR_OFFER)   \
										| ((v << 0) & DF_UR_OFFER)))
#define	DF_SET_UR_ATTEMPT(w,v)	((w) = (((w) & ~DF_UR_ATTEMPT) \
										| ((v << 2) & DF_UR_ATTEMPT)))
#define	DF_GET_UR_OFFER(w)		(((w) & DF_UR_OFFER)   >> 0)
#define	DF_GET_UR_ATTEMPT(w)	(((w) & DF_UR_ATTEMPT) >> 2)
//
//	These four "UR" (Unresolved Reconciliation) bits indicate there is
//	a new DRM offered and the context of older DRMs have attempted 
//	reconciliation - but possibly not with the newest DRM.  See HWN 
//	1999-Oct-12, 1999-Oct-14 and other comments of those dates (esp 14th).
//
//	-	When a new DRM is offered the UR bits of other DRMs (with the same
//		handles as the new) is checked.  If an ATTEMPT bit is set then
//		the opposite OFFER bit is set.  (If no ATTEMPT bit is set then any
//		one of the OFFER bits is set.)
//
//	-	When a context accesses a DRM it checks and compares OFFER and 
//		ATTEMPT bits.  Any difference in their values indicates their is a 
//		new offer and reconciliation is (should be) attempted.  If their is
//		reconciliation then the context adopts the new DRM.  If their are
//		no other contexts using the old DRM then the old DRM will be 
//		destroyed automatically by the DS.  If their is no reconciliation
//		then the bit pattern of ATTEMPT is set to match that of OFFER so
//		that reconciliation is not attempted again until their is a newer
//		offer.
//
//	Generally -
//
//		Zero OFFER indicates no offer - no new DRM is available.
//
//		Both bits of either OFFER or ATTEMPT should never be set at the
//		same time.
//
//		If an ATTEMPT bit is set then one of the OFFER bits better be
//		set.
//
//	2000-May-09		See HWN of this date.
//
//	DRM header word 3.  For offer-submit discrimination.  I.e., Should 
//	the application do an Offer() call to the DS or are conditions such 
//	that the previous Offer() call is still sufficient?
//	
//	See HWNs.  The purpose of these is so that the app can tell whether
//	or not it needs to call Offer() after it has changed a DRM.
//	The RSP values DRM will, of course, change very frequently - whenever
//	an assignment is made to a RSP variable.  But other contexts do not 
//	need to update their DRMs if they are using the same DRM (da) and, 
//	likewise, the app does not have to call Offer() after each change if 
//	it knows that all other DRMs still have not updated themselves because 
//	of the previous Offer() call.
//
//	Since I don't want to add words to the DRM header I packed things of
//	the HWNs into one word.  Instead of indicating the total number of DRMs
//	with the same handle there is a single flag bit that indicates more 
//	than one DRM with the same handle.  The number of offer calls is not
//	maintained in this word (word 1 is a "change count" - that might do).
//	And, since the max number of DRMs a record may have can be represented 
//	with 11 bits (see MAX_REC_DRMS) the flag and the two DNU values can 
//	fit in one word - with some bits to spare.
//
//	Put the flag bit in word 0 so that the DRM structure's Flags word can
//	be used when the DRM is not loaded.
//
//	Word 3 bit allocation:	76543210 76543210 76543210 76543210 
//							-------- -------- -------- --------
//												   OOO OOOOOOOO		ODNU
//                                     DDDDDD DDDDD					DNU
//							SSSSSSSS SS								spare
//
//	ODNU:	11 bits.	Number of original DRMs not updated.
//
//	DNU:	11 bits.	Number of DRMs not updated.
//
//	Word 3.
//
#define	DW_SET_ODNU(w,v)	((w) = ((w) & 0xFFFFF800) | ((v) <<  0))
#define	DW_GET_ODNU(w)		(((w) & 0x000007FF) >>  0)

#define	DW_SET_DNU(w,v)		((w) = ((w) & 0xFFFC07FF) | ((v) << 11))
#define	DW_GET_DNU(w)		(((w) & 0x0003F800) >> 11)

//	Word 0.
//
#define	DW_SET_DNUF(w,v)	((w) = ((w) & ~DF_DNU) | ((v) << 27))
#define	DW_GET_DNUF(w)		(((w) & DF_DNU) >> 27)



//	2004-Aug-09		A second flags word for DRMs.
//
//	This flags word is in the DRM structure - not normally available to
//	the app unless gotten via the SDRMInfo structure.
//
//	Need a bArchive.  Instead of just a bArchive word, create Flags2 and
//	use a bit in it.  Then we have other bits for other purposes.
//
//	The DF2_ (DRM Flags2) definitions -
/*
//	If this bit is set then the DRM is marked to be archived.  Actually,
//	this more acurately indicates there are DRM_Clients of this DRM in
//	the DRM's CAR - and as long at that is true then this bit should be
//	set - and _only_ as long as that is true - if the last of the clients
//	is taken out of the CAR then this bit should be unset.
//
//	When this bit is set the DRM will not be destroyed when the last
//	_loaded_ client discharges the DRM - instead, the DRM will be archived
//	in the same record as that of the DRM's CAR.
//
#define	DF2_CTX_ARCHIVED			0x00000001

	On second thought, use nArcClis in the DRM structure instead of just
	a flag bit.  nArcClis tells not just that the DRM structure should
	be archived (when > 0) but also, when == 0, that because there are no
	contexts archived the DRM can be destroyed.

	But keep the Flags2 word in the DRM structure for possible use later.
*/
//	2004-Oct-19		A Flags2 bit in the DRM.
//
//	Note that the Flags2 word is in the DRM structure - not normally 
//	available to the app unless gotten via the SDRMInfo structure.
//
//	This bit indicates the DRM is a record base DRM.
//
//	This bit being set is (was) the same as checking pD->hD == pMRD->hRBD
//	(in the DS).
//
#define	DF2_IS_RBD					0x00000002


//	Bit set if the DRM data is source text.  For example, the source text of
//	the odef.
//
#define	DF2_IS_SRC_TEXT				0x00000004

//	2012-Dec-14
//
//	Something other than HomeGP to indicate checked out.
//
//	That is, a record whose home is another GP may be pulled from Share so 
//	that the home GP's status of the record is that it is not checked out.
//
#define	DF2_IS_CHECKED_OUT			0x00000008



//	Got to limit the client trail length.
//
//	UT:	If the CAR (client archive record) is implemented with a UT and the
//		client trail is the index key (entries sorted by client trails) then
//		the length of the trail must be limited - a key's size is limited to
//		256 bytes.  If only the record Id of each client in the trail is
//		used in the key then a client trail of length 16 is 256 bytes (if
//		each RID is 4 words - worse case, yes).
//
//		However, a variable number of RIDs in the key will require some type
//		of count info at the front of the key (how many RIDs).  Therefore
//		with a key size limit of 256 bytes, each RID taking 4 words (16 
//		bytes), and room at the front for RID count, then a max of 15 RIDs
//		should be allowed.
//
//		This is based on the UT node size being 1024.  I suppose if the node
//		size is increased to 2048 then the max key size would be 512, etc..
//
//		Or, the max RID size could be reduced from 4 words to 3, 2 or 1.
//		In many industrial systems RIDs may not go beyound 1 word.  And, on
//		the other hand, context depth may never come close to 15.
//
//		Note that increasing this value affects UTFT_RIDA.  Watch out for
//		the number of bits used to indicate number of RIDs in the field or
//		key.
//
#define	MAX_CLIENT_TRAIL_LEN		15



//	What were DSS_ macros from the TData days.
//
//	Bits  0 -  7:
//
//	These bits are primarilly used to indicate the result of a function
//	call.  Functions may return D_OK or D_ERROR, for example, but that
//	return value is not the same as the status of the object.
//
//	Before the return, the object's status value is bit combined with 
//	the return value.
//
#define	D_RETURN_MASK		0x800000FF
//
//	Bits  8 - 15:
//
//	These generally indicate more slowly changing status aspects.  For
//	example, if the TData constructor completed ok then the 
//	D_CONSTRUCTEDOK bit will be set.  There will be a public TData 
//	function returning boolean for each of these status bits.
//	For when the user must choose an object the D_CANCELED status
//	bit is provided.  The bit is set if the user cancels the choose dialog.
//
//
//	Benefits
//
//	If TData implements Status correctly a caller can determine things
//	such as whether or not the object was constructed correctly, its a
//	a new record, etc., after the object has been used a while (even 
//	though an exception might have occured if not constructed correctly
//	there will be that bit set indicating so).
//
enum	DStatus
{
	D_UNDEFINED						= (0x00000000),

	D_OK							= (0x00000001),

	D_ERROR	 						= (0x00000002),

	D_CANCEL						= (0x00000003),

	D_NOTFOUND						= (0x00000004),

	D_OUTOFMEMORY					= (0x00000005),

	D_CLIENT						= (0x00000006),

	D_SENTRY						= (0x00000007),

//	14-Aug-97	Used only by RPL stuff.  Implemented a rplUSES value in that
//				library.
//	D_USES							= (0x00000007),

//	D_BOI							= (0x00000008),		//	Begining Of Index

	D_CONSTRUCTEDOK					= (0x00000100),

	D_NEWRECORD						= (0x00000200),

	D_CANCELED						= (0x00000400),

	D_NORECORD						= (0x00000800),

	D_BUSY							= (0x00001000),

	D_NAME_NOT_UNIQUE				= (0x00002000),

	D_NEW_DSLOCKED					= (0x00004000),

	//	2006-Mar-20		
	//
	//	Flag indicates the data instance is in the notifyee log.
	//
	D_IN_NOTIFYEE_LOG				= (0x00008000),

	//	2008-Nov-18
	//
	//	Flag indicates the data instance is in the notifyer log.
	//
	D_IN_NOTIFYER_LOG				= (0x00010000),

	//	2009-Jul-16
	//
	//	To unregister the lock on the record in ReleaseIDIItem().
	//
	D_ON_RELEASE_UNREGISTER_LOCK	= (0x00020000),

	//	2009-Jul-20
	//
	//	To remove the client during the DS trip to release.
	//
	D_ON_RELEASE_RMV_CLIENT			= (0x00040000),

	//	2009-Jul-20
	//
	//	With D_ON_RELEASE_RMV_CLIENT, to unlock the record when removing the
	//	client.
	//
	D_ON_RELEASE_UNLOCK 			= (0x00080000),

	//	2006-Apr-10		Temporary work around.
	//
	//	For direct use of CData (no derivation).  For example, pHVFData in
	//	CHdrViewFrame.
	//
	D_BEING_DISCHARGED_EXEMPT		= (0x00100000),

	//	2010-Jan-08, 10		Implementing cmd rec type paths.
	//
	D_TYPE_NOT_NEW					= (0x00200000),

	D_NAME_TOO_LONG					= (0x00400000),

	D_NULL_REF						= (0x00800000),

	//	2012-Dec-07		To override putting an instance in a cache.
	//
	//	Used in CDType.
	//
	D_NO_ICACHE						= (0x01000000),

	//	2013-Jun-16, 28		Indicates a new record is under the lock set
	//						by the DS.
	//
	D_NEWRECORD_LOCKED				= (0x02000000),

};


//	Reserved LIDs.
//
//	These are LIDs reserved so that non-DRMs can be stored in a cache index
//	allocation.  For example, a record's name would be nice to store in 
//	shared memory so that all contexts on a GP can easily get at it without 
//	making DS calls.
//
#define	RL_NAME_ETC			1		//	See MRD in mrd.h.

#define	RL_CTX_UPDT_INFO	2		//	See CtxUpdtInfo in did.h.

#define	RL_REC_SERVER		3		//	See RecServers in rs_typedefs.h.

//	When a reference is stored in a UT the UT must become a client of
//	the data of that reference so that the data is not destroyed if all
//	other contexts discharge it.  When a DRM_Client of that data is made 
//	representing the UT, instead of 0 for the svr Id (that is, the 
//	DRM_Server in the client - which in the UT - there is none because
//	it is not managed by the MRD stuff) specify RL_UT_SVR.  This can/should
//	be recognized in routines that check for errors, etc..
//
#define	RL_UT_SVR_ID		4		//	See RecMRD::AddUT_Client().

//	UT records are not managed by MRD stuff.  Yet they are clients of 
//	regular records and therefore must specify a DID2 for themselves. 
//	Instead of 0 for the DRM_Id of UT DID2s, specify RL_UT_DRM_ID.  
//	This can/should be recognized in routines that check for errors, etc..
//
#define	RL_UT_DRM_ID		5		//	See RecMRD::AddUT_Client().


//	2008-Dec-02		For shared memory used by the UT and applications to
//					pass long fields (LF).
//
//	Only shared memory is allocated - not a record or DRM.  But yet a
//	DRM Id is probably needed for the cache entry.
//
#define	RL_UT_LF_DRM_ID		6


//	If its known that the record Id is that of a type record then the
//	DRM_Id of a DID2 should be this.
//
#define	RL_TYPE_FLAG		7		//	See GetClientStr() in ver_ctx.cpp.


//	For the RID pool.
//
#define	RL_RID_POOL			8


//	2011-Jul-13		Data file "header".
//
//	Fixing the read/write of the data file header in ds_main.cpp.
//
//	PrepWriteDFI(), etc., require non-null keys.  So this is the SID of the
//	header DFIKey.
//
#define	RL_HDR				9


//	2012-Mar-07		System description.
//
#define	RL_SYS_DESC			10


#define	RL_NON_RESERVED		100		//	First allocatable LID.



//	NameEtc Flags
//
//	Flags for the NameEtc info of a MRD.  See versions2.h.
//
#define	NEF_NAME_CHANGED		0x80000000	//	Should indicate that the II
											//	needs updating.

#define	NEF_CREATING_LOCKED		0x40000000	//	Set when record is created
											//	and never set again after 
											//	dim func UnlockNewRecord().

//	To prevent deadlock just one record may be locked (ORL) by a thread at a 
//	time.  This bit is set by a thread when it unlocks a record but does
//	not want  * any *  changes made to the record while it locks another
//	record.
//
//	If a thread detects this bit after locking a record that thread must
//	immediately unlock the record, unlock the DIM if it is locked, wait,
//	lock the DIM and record again and check the bit again.  Repeate if 
//	necessary.
//
#define	NEF_DO_NOT_MUCK			0x20000000	

//	This bit is set when the record has been checked out from another
//	GP.  Used by CDType so as to not remove the client if the its the first
//	client as is the case when the record is checked out.
//
#define	NEF_CHECKED_OUT			0x10000000

//	This bit is set if it is suspected that a record aleady exists when
//	the DS is called to create the record.  Happens for types when using
//	the repository (Share).
//
#define	NEF_MAY_EXIST			0x08000000


#define	NEF_NONVOLATILE_BITS	0x00FFFFFF	//	2002-May-11.

//	2004-Aug-16		Going to dup-less DRMs.
//
//	See 2004-Aug-16 entry in DS2_DRMs.doc with heading "Eliminating Dup 
//	(of a DRM)?".
//
//	Unless this bit is set, duping of a DRM is not allowed.
//
#define	NEF_ALLOW_DRM_DUP		0x00000001

//	If this bit is set (default) then the record uses the RS collection.
//
/*	2004-Sep-01

	DRMs may be duped and have svrs.  ODef, for example, must be duped, 
	and it has hdr svrs.

	All records use a server collection - now a DataSvrs collection - in
	each DRM.

#define	NEF_USE_RS				0x00000002
*/
//	2004-Sep-08
//
//	If NEF_ALLOW_DRM_DUP is set _and_ this is set it means the DRM can be
//	reallocated - which is what the DS function RecMRD::DupDRM() does if 
//	the DRM has just one context.
//
//	This flag specifies that no matter what the context situation is, there
//	is always just one DRM.  Used, for example, by CDCheckNtrp::~CDCheckNtrp
//	when writing the data of a changed command record - it may have shrunk
//	or growen. 
//
//	This flag says that the data can be duped (reallocated) - but there is 
//	always just one version.
//
#define	NEF_ONE_DRM				0x00000002


//	2006-Feb-02		Sometimes, turn off context sensitivity.
//
//	Indicate that server changes (enlisting and discharging servers) should 
//	be applied to all clients - that is, in effect, turn off context 
//	sensitivity.
//
//	Originally implemented on this date (2006-Feb-02) for CDEntity.  Entities
//	can have children and (for now) one parent - which are server entities.
//	Setting up those relationships in a cmd record and having them made
//	non-volatile is complicated.  That is, with context sensitivity, a child
//	for example is linked to a parent in a context - say that of a CDEntity01
//	which is an entity record that can be manipulated as a record in a 
//	cmd record.  If CDEntity01 record is a local variable then the parent-
//	child relationship is lost when function returns - unless other contexts 
//	are updated to that of the CDEntity01 record.  But which contexts should
//	be updated?  When?  It seems that any client of the entities should see
//	the parent and children - so, for entities, it seems, all contexts should
//	see the servers - with possible exceptions.
//
//	So, this flag, when set in the NEFlags word of NameEtc, means that server
//	enlistment and discharging is not context sensitive unless requested.
//
//	2010-May-13
//
//	See  NEF_NO_CTX_SNSTVTY  below.
//
#define	NEF_REQUEST_CTX_SNSTVTY	0x00000004

//	2006-Feb-06
//
//	App provides hC with this value (otherwise 0) when calling AddRecord().
//	This tells the DS to create two DRM_Clients for the new data - the first
//	is the null client but its ServerDRM_Id is set to the hC of a second
//	special DRM_Client to which the servers of the data are linked.
//
//	So, the null client and other DRM_Clients do not link with the servers.
//	Instead they reference - by their ServerDRM_Id - a special client - which
//	the app never sees the hC of - that has a link with all the servers.
//
//	The servers, therefore, always have just the one - special - client.
/*
#define	HC_NO_CTX_SNSTVTY		0x10000000
*/
//	2013-Mar-02
//
#define	HC_DELAYED_DISCHARGE	0x10000000

//	2013-Mar-26		When RS_SS_SVR_IS_ROOT and a new LID is gotten for
//					the RootIdC.
//
//	This bit will indicate the hC is not that of a ClientInst.
//
#define	HC_NOT_A_REAL_CLIENT	0x20000000


//	2006-Feb-06
//
//	Flag bit set in a DRM_Client's ServerDRM_Id member when the DRM_Client
//	does not link with servers - instead, ServerDRM_Id (that is, its lower
//	bits) is the hC of the special DRM_Client that does.
//
#define	DIF_HC					0x10000000


//	2010-Jan-27		Trying to disable context sensitivity again.
//
//	Optional.
//
//	For now, instead of removing  * all *  the NEF_REQUEST_CTX_SNSTVTY
//	code, adding a new flag.
//
//	(Note that currently NEF_REQUEST_CTX_SNSTVTY is not being used.)
//
#define	NEF_NO_CTX_SNSTVTY		0x00000010

//	2010-Jan-27, 30
//
//	Another way of saying NEF_NO_CTX_SNSTVTY.  But the NEF_NO_CTX_SNSTVTY
//	macro might be deprecated.
//
//	Optional.  Probably, initiallly, selectively on cmd records.
//
//	Ignored, irrelevant if NO_CTX_SNS_ENABLED is 0.
//
#define	NEF_NO_CTX_SNS_REFS		0x00000020


//	2010-Feb-07
//
//	Root in the sense of a server tree.
//
//	Few records are root records.  One example would be ntrp records.
//
//	Ignored, irrelevant if, in a svr record, NO_CTX_SNS_ENABLED is 0.
//
//	It is an error if there is more than one record with NEF_ROOT_RECORD set
//	in a client trail.  For example, Ntrp and User records are probably both
//	root records.  So, it will be an error for an Ntrp record to be a svr of 
//	a User record.
//
/*	2010-Mar-22, 25

	NEF_ROOT_RECORD is not needed.

	The record at the head of all client trails is a root record.

#define	NEF_ROOT_RECORD			0x00000040
*/

//	2010-Aug-05		Don't dup on server reference change.
//
//	Since the context sensitivity was achieved with the indirectness of
//	hS why was a dup necessary on reference change?
//
//	The value of hS should be set in the DRM (probably with a call to
//	IDIIItem::CreateRefVal() - see CDEntity02::CDEntity02(1) and
//	CDWorld::CDWorld(1) for examples) when the DRM is created.  That 
//	value never changes.
//
//	Anyway, this flag says to not dup when changing a reference.  For 
//	example, when discharging a server.
//
#define	NEF_NO_DUP_ON_REF_CHG	0x00000080


//	2004-Aug-23		DS2
//	2004-Oct-05
//
//	While a RBD may be dupped, and there may be multiple RBDs (the
//	NEF_ONE_DRM flag is not set in the NEFlags) the record may have a
//	* child *  DRM of which multiple versions of  * it *  are not 
//	wanted.  So, as kind of a cludge for now, the app may set this 
//	flag bit in the bCopy word of the DupDRM() call to specify that 
//	another DRM should not be created.
//
#define	DUPF_ONE_DRM			0x00000002


//	2004-Aug-23, 2004-Oct-13
//
//	hS (handle of a DRM_Server - not the Id of such) flag bit that 
//	indicates the svr is not in the DSs collection.
//
//	CmdRecs maintain hSs themselves - there may be many svrs in a CmdRec
//	and having to maintain them in the DS would waist space.  This also
//	relieves the problem of MAX_DATA_SVRS (max svrs in a DSs collection).
//
/*	2005-Dec-11		Generallizing.

#define	HS_CMDREC_SVR			0x80000000
*/
//	Other record types (not just command records) maintain their servers 
//	outside the DS collection (DataServers Collection).
//
#define	HS_NOT_IN_DSC			0x80000000


//	2013-Mar-18		Indicates reference is that of a function's local var.
//
//	That is, a function's local variable is referfencing a server.
//
//	Such svrs should not be adopted.
//
#define	HS_LOCAL				0x40000000

#define	HS_HANDLE_BITS			0x0FFFFFFF

#define	HS_FLAG_BITS			0xF0000000


//	2004-Aug-23		DS2
//
//	In CDVals (CmdRec), a special value for hS that indicates the svr is 
//	the same record as the client (eg, a record has a member that is a ptr
//	and it points to another member in the same record).
//
//#define	hS_THIS					(HS_CMDREC_SVR | 0x00000001)
//
//	2004-Dec-06		Every cmd rec ref's hS is set and not changed
//
//	When a ptr is pointing to the same record that it is declared in then
//	the DRM_Server's ServerDId will be set to the same data as the client.
 


//	1999-Oct-11		More NameEtc stuff.
//
//	Update Criteria.  These are used to describe the criteria used when
//	a context is updated to a different version of DRM.
//
//	These bits are set (not set) in the UCFlags word of NameEtc.
//
//	See RecMRD::UpdateContext()  and  RecMRD::DominantByHistory() for how 
//	these bits are used.
//
#define	UC_HIST_CHECKED		0x00000001		//	Use DRM with latest DHE
											//	indicating "checked".

#define	UC_HIST_CHECKED_RQD	0x00000002		//	Current DRM required to have
											//	"checked" DHE to update.

//	1999-Oct-13		Yet more NameEtc stuff.
//
//	Values for the interlocking words.
//
#define	NE_RECORD_LOCKED	0x00000001

#define	NE_RECORD_UNLOCKED	0x00000000

#define	NE_RECORD_WAITING	0x00000002



//	Some stuff defined for working with unresolved reconciliations.
//
//	See HWN of 1999-Oct-14, 1999-Oct-18 and comments of those dates in files.
//
//	Reconciliation involves multiple steps and the process might continue
//	or stop as a result of each step depending on new DRM availability,
//	client situations, etc..
//
#define	UR_CONTINUE			0

#define	UR_NO_OFFER			1		//	Reconciliation process can not 
									//	continue now because there is no
									//	new DRM to reconcile with (UR bits
									//	not set).

#define	UR_NO_NEW_OFFER		2		//	UR bits set but there is no * new *
									//	offer.


//	2000-May-24
//
//	Data change notification types.  Used with WM_DATA_CHANGED message.
//
#define	DCHG_ANY				0x00000001		//	Catch all.

#define	DCHG_POS				0x00000002		//	Position.

#define	DCHG_COLOR				0x00000004		//	Color.

#define	DCHG_ORG				0x00000008		//	Coordinate frame.

#define	DCHG_MODEL				0x00000010		//	Entity model change.

#define	DCHG_COMPONENT			0x00000020		//	Model component change.

#define	DCHG_COMP_NO_UPDATE		0x00000040		//	Model component change,
												//	no update necessary.

#define	DCHG_NAME				0x00000080		//	Record name change.

#define	DCHG_DEPRECATED			0x00000100		//	2002-May-11
												//	Record is deprecated.

#define	DCHG_OBJ_VALUE			0x00000200		//	CDVals value change.

#define	DCHG_ROBOT				0x00000400		//	Any robot change.

#define	DCHG_REC_ADD			0x00000800		//	2002-May-17
												//	Record added (created).

#define	DCHG_REC_DEL			0x00001000		//	2002-May-17
												//	Record deleted 
												//	(deprecated).

#define	DCHG_ENT_LNK			0x00002000		//	2002-May-17
												//	Entity linked or
												//	unlinked.

#define	DCHG_MODEL_DOC			0x00004000		//	2003-Feb-23

#define	DCHG_ENT_VALS			0x00008000		//	2008-Feb-11
												//	Change in entity vals
												//	record.

#define	DCHG_ENT_ADD			0x00010000		//	2008-Dec-05
												//	Added entity to world.

#define	DCHG_WORLD				0x00020000		//	2009-Jan-12
												//	WCHG_ specs details.

#define	DCHG_VIEWPOINT			0x00040000		//	2009-Jul-10
												//	Viewpoint in extra data.


//	2009-Dec-21
//
//	Data change notification flags.
//
#define	DCHGF_SYNC				0x00000001		//	Notify only when the PE's
												//	are synchronized.



//	2000-Jul-24
//
//	Maximum number of "indentations" in an export file.
//
//	An indentation is made when an entity exports children.  Therefore, this
//	limits the height of the entity tree exported.
//
#define	EXP_MAX_INDENTS			32


#define	EXP_TYPE_DESC			"Data Type"


//	When exporting long text this is the maximum number of characters per
//	line.  This does not include the number at the begining of the line that
//	indicates the number of characters on the line or any other information
//	characters included in the export.  I.e., an exported line will have this
//	format in the export file -
//
//		www...wwwnnnwccc...ccc
//
//	Where w indiates white space characters, nnn is a zero-filled three
//	digit number indiating the number of characters to import, and c is the
//	characters to import.  This macro specifies the maximum number of c 
//	characters.
//	
#define	EXP_LONG_TEXT_LINE_MAX_CHARS	80


//	2000-Dec-26
//
//	R & S applicaton objects (CVPFrame, CDEntity, CDModel, etc.) expect
//	database records of certain contents.  
//
//	The first NUM_RELEASE_VERSION_BYTES bytes of such records will specify
//	a version number so that new versions of R & S application software
//	can handle a user's application objects in old databases.
//
//	->	This has nothing to do with DRM versioning in the data server.
//
//	See GetDataVersion() in data.h, .cpp.
//
#define	NUM_RELEASE_VERSION_BYTES	16



//	2001-May-03
//
//	For implementing a user interface when the user needs to be prompted
//	for action direction.
//
#define	RS_ANSR_UNKNOWN				0x00000000

#define	RS_ANSR_YES					0x00000001

#define	RS_ANSR_NO					0x00000002

#define	RS_ANSR_CANCEL				0x00000004



//	2001-Oct-23
//
//	Maximum number of elements another element may contain.  See other
//	comments of this date.
//
//	Originally implemented to control closing and opening the GUIness of
//	the elements - open/close containers first/last.
//
#define	SE_MAX_GUI_CTNEES			16


//	2001-Nov-30
//
//	For more control over when to reopen system elements and their 
//	containees.
//
#define	SE_WAS_OPEN_PREV_SESSION	0x00000001

#define	SE_WAS_OPEN_CTNEE			0x00000002

//	2002-Jul-23
//
#define	SE_WAS_MAXIMIZED			0x00000010

#define	SE_WAS_MINIMIZED			0x00000020


//	2002-May-11		Record Flags.
//
//	Bits in the flags word of the Id Index item

//	RF_DEPRECATED		Should prevent additional clients of records.
//
//	Originally for "deleting" RWO model records.  When this bit is set
//	on a record then no GUI elements will list the record so the user
//	will not select it to be a model of an element, for example.
//
//	The idea is that eventually all clients will discharge the record and
//	then it can be destroyed.
//
#define	RF_DEPRECATED			0x00000001	






//	2002-Oct-10		DRM Change Reasons
//
//	These are specified in the Resn member of DRMChgResn when a DRM is
//	dupped.  The DRMChgResn structure is defined in did.h.
//
//	DCR_NONE
//
#define	DCR_NONE				0x00000000


//	DCR_CODE
//
//	Text of code is changed usually by the user with an editor.
//
//	Applies to ODef records, and Vals when the vals are dupped due to changes
//	in variables.
//
#define	DCR_CODE				0x00000001		


//	DCR_PTR
//
//	A code pointer has changed.  When ever a pointer points to a different
//	record or if is pointing to a record and it is set to NULL the DRM in
//	the pointer is declared in must be dupped.
//
#define	DCR_PTR					0x00000002


//	DCR_DTYPE
//
//	CDType dups its DRM for various reasons - mostly to create a larger DRM
//	when subtypes are added, etc..
//
//	More specific info is put in the MiscInfo of DRMChgResn.
//
#define	DCR_DTYPE				0x00000003


//	DCR_DENTITY
//
//	CDEntity dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DENTITY				0x00000004


//	DCR_DMODEL
//
//	CDModel dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DMODEL				0x00000005


//	DCR_DBAG
//
//	CDBag dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DBAG				0x00000006


//	DCR_DGENROBOT1
//
//	CDGenRobot01 dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DGENROBOT1			0x00000007


//	DCR_DCHECKNTRP
//
//	Probably the ntrp (its state, etc.).
//
//	CDCheckNtrp dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DCHECKNTRP			0x00000008


//	DCR_DCOLORS
//
//	CDColors dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DCOLORS				0x00000009


//	DCR_DNTRP
//
//	Probably the ntrp (its state, etc.).
//
//	CDNtrp dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DNTRP				0x0000000A


//	DCR_DUDUI
//
//	CDUDUI dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DUDUI				0x0000000B


//	DCR_DODEF
//
//	CDODef dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DODEF				0x0000000C


//	DCR_DOHDR
//
//	CDOHdr dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DOHDR				0x0000000D


//	DCR_DVALS
//
//	CDVals dups.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_DVALS				0x0000000E


//	DCR_CHECKING
//
//	CheckNtrp dups the object (that is, a CDVals) being checked 
//	before checking.
//
#define	DCR_CHECKING			0x0000000F


//	DCR_CHECKED_OK
//
//	CheckNtrp offers checked object (that is, a CDVals).
//
#define	DCR_CHECKED_OK			0x00000010


//	DCR_CHECK_PARAMS
//
//	CheckNtrp unbuffering parameters proxy.
//
#define	DCR_CHECK_PARAMS		0x00000011


//	DCR_POP_CHECK_GLBLS
//
//	CheckNtrp unbuffering parameters proxy.
//
#define	DCR_POP_CHECK_GLBLS		0x00000012


//	DCR_DSCRG_B4_UPDATE
//
//	Submitted in the process of a CDVals updating itself when its current
//	DRM is dupped to discharge its servers - they are discharged in case
//	they are no longer servers in the version the CDVals is updating to.
//
#define	DCR_DSCRG_B4_UPDATE		0x00000013


//	DCR_ADOPT_ODEF
//
//	CDVals DRM being dupped because a new ODef version is being adopted.
//	Specified when CDVals::AdoptNewODef() is called.
//
#define	DCR_ADOPT_ODEF			0x00000014


//	DCR_USEDVALS
//
//	Updating used vals.
//
#define	DCR_USEDVALS			0x00000015


//	DCR_MODEL_VERSION_BYTES
//
//	Adding dimension to documentation record.
//
#define	DCR_MODEL_VERSION_BYTES	0x00000016


//	DCR_DOC_ADD_DIMENSION
//
//	Adding dimension to documentation record.
//
#define	DCR_DOC_ADD_DIMENSION	0x00000017


//	DCR_DOC_DEL_DIMENSION
//
//	Removing dimension from documentation record.
//
#define	DCR_DOC_DEL_DIMENSION	0x00000018


//	DCR_DDATABYTES
//
//	Server is being discharged in CDataBytes base class.
/*
#define	DCR_DDATABYTES			0x00000019
*/


//	DCR_SVR_ID_CHANGE
//
//	The RID (as maintained in the record) of a server changed.
//
#define	DCR_SVR_ID_CHANGE		0x0000001A


//	DCR_CLIENT_UPDATE_SVR
//
//	A record is updating a server record.
//
#define	DCR_CLIENT_UPDATE_SVR	0x0000001B


//	DCR_SERVER_ENLISTMENT
//
//	A reference to other data is being added.
//
//	Used by the DS in RecMRD::EnlistServer().
//
#define	DCR_SERVER_ENLISTMENT	0x0000001C


//	DCR_SERVER_DISCHARGE
//
//	A reference to other data is being removed.
//
//	Used by the DS in RecMRD::DischargeServer().
//
#define	DCR_SERVER_DISCHARGE	0x0000001D


//	DCR_TEXTURE
//
//	CDTexture base DRM is dupped when a client changes the texture or its
//	notes.  More info in MiscInfo of DRMChgResn.
//
#define	DCR_TEXTURE				0x0000001E


//	DCR_SET_ADD_INDEX
//
//	CDVals set DRM is dupped to add an index.
//
#define	DCR_SET_ADD_INDEX		0x0000001F


//	DCR_DIMGPROC
//
//	CDImgProc
//
#define	DCR_DIMGPROC			0x00000020



/*--------------------------------------------------------------------------*/
/*                                                                          */
/*	2003-Jan-21               Clipboard Formats                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/

//	Used by a UDUI view to copy/paste controls.
//
#define	RS_CF_UDUI_CONTROLS		(CF_PRIVATEFIRST + 0)

#define	RS_CF_UDUI_CONTROL_POS	(CF_PRIVATEFIRST + 1)

#define	RS_CF_UDUI_CONTROL_SIZE	(CF_PRIVATEFIRST + 2)

#define	RS_CF_UDUI_CONTROL_BOTH	(CF_PRIVATEFIRST + 3)		//	Pos and Size.


//	Used by model dialogs to copy/paste model components.
//
#define	RS_CF_EMC_COMPONENTS	(CF_PRIVATEFIRST + 4)

/*
//	2003-Aug-19		Parameter sent in WM_GEN_UPDATE message.
//
enum	GenUpdtReason
{
	gurUndefined		= 0x00000000,

	gurNewEntity		= 0x00000001,

};	//	GenUpdtReason

	2008-Jan-18
*/

//	2004-Nov-13		DS function LoadServer() flags.
//
//	This bit is set if hC[0] was not previously linked to the svr (was not 
//	a client).
//
#define	RS_LS_LINKED				0x00000001


//	This bit is set if the server Id in the DS' DRM_Server structure is
//	null - indicates a cmd record null did or null ptr.
//
#define	RS_LS_NULL_REF				0x00000002


//	2004-Dec-06		hS is not set to hS_THIS.
//
//	This bit is set if the server data is the same as the client.  For
//	example, a cmd rec ptr variable is pointing to a member in the same
//	record.
//
#define	RS_LS_THIS_REF				0x00000004



//	2004-Nov-13		DS function SetServer() flags.
//
//	This bit is set when the reference to the server is being set for a
//	cmd rec variable value of type did.  The point being that the variable 
//	is only the Id of the server and it does not do anything with the 
//	server but reference it.
//
#define	RS_SS_NO_LOAD				0x00000008

//	This bit is probably also set when RS_SS_NO_LOAD is set.  This bit
//	keeps the DS from removing the DRM_Server structure from the client 
//	data when the current server data is discharged (in the process of 
//	setting the server the current server (if any) is first discharged).
//
//	This bit will probably also be set when a cmd rec variable of type ptr 
//	is being set null - the idea being that we still what a DRM_Server 
//	whose data Id is null to indicate the ptr is null.
//
#define	RS_SS_KEEP_STRUCT			0x00000010

//	The DS uses reference counts to keep track of which data is being 
//	actively used by the app.  When calling SetServer() due to changes
//	in cmd rec did variables this bit should be set because server data 
//	is not being * actively * referenced.
//
#define	RS_SS_NO_ADJ_REFS			0x00000020

//	2012-Mar-13
//
//	Override no context sensitivity flag in NEFlags.  That is, ignore the
//	the NEF_NO_CTX_SNS_REFS bit in a record's NEFlags.
//
#define	RS_SS_OVERRIDE_NO_CTX_SNS	0x00000040




//	2004-Nov-13		DS function DischargeServer() flags.
//
//	->	The same Flags word is used by SetServer() - watch the bit 
//		designations.
//
//	This bit is set when the application wants the DRM_Server whose
//	ServerDId is null to be destroyed.
//
//	2010-Jun		More accurately -
//
//	The CILSvr is removed from the specified client.  If that is the last
//	CILSvr linked to the DRM_Server then the DRM_Server is removed also.
//
#define	RS_DS_DESTROY_STRUCT		0x00000080

//	This bit is set by the app when the DS should unlock the record being
//	discharged.
//
#define	RS_DS_UNLOCK_SERVER			0x00000100

//	2010-Jul-07
//
//	Override no context sensitivity flag in NEFlags.  That is, ignore the
//	the NEF_NO_CTX_SNS_REFS bit in a record's NEFlags.
//
//	Purpose -	ODef records have their NEF_NO_CTX_SNS_REFS bit set so
//				that changes made in one context are seen by others.  But
//				when a variable discharges a def the def should not be 
//				discharged by other clients at the same time because those
//				other clients might still be active.
//
#define	RS_DS_OVERRIDE_NO_CTX_SNS	0x00000200


//	2004-Nov-13		DS function AddServer() - EnlistServer() flags.
//
//	->	The same Flags word is used by SetServer() and DischargeServer() -
//		watch the bit designations.
//
//	The server is being added to a record that was pulled from Share.
//
#define	RS_AS_PULLING_FROM_SHARE	0x00000400


//	2012-Jul-16		Do not post svr.
//
//	E.g., a ntrp's edit session is not posted to Share.  So we don't want a 
//	ntrp to post its checkntrp svr to Share.
//
#define	RS_AS_DO_NOT_SHARE			0x00000800


//	2012-Jul-31		Root should not have to be that of a null context.
//
//	For snapshots (see CDlgSnapshot) the root must be the CDSnapshot record
//	which is maintained as a svr of the ntrp.  That is, not with a null
//	client.  The reason for this is that if the snapshot had the same root
//	client as the rest of the svrs of the ntrp then ops that normally occur
//	durning the running of the ntrp whould affect the snapshot.  That is bad
//	because we want the snapshot to remain as it was when it was created.
//	
#define	RS_SS_SVR_IS_ROOT			0x00001000


//	2012-Sep-10		Load Record flag.
//
//	Client root should not be unique.  Rather, set RootIdC to that of the
//	client the svrs are set from.  That is, we want to use the same svr
//	links as that from an already existing root.
//
//	Used by CDUser to keep the svrs persistent by having them set for all
//	clients so that when the PE thread's null client discharges the record
//	the svrs will still be set in the null client that remains.
//
#define	RS_LR_ADOPT_ROOT			0X00002000


//	2013-Jan-09		Load Record flag
//
//	Force using an existing hC if available.  See RecMRD::LoadBaseDRM().
//
//	Used by BILib function GetRecord() when getting the IdC of a native
//	record.  Don't want to create an additional null context.
//
#define	RS_LR_EXISTING_CONTEXT		0X00004000


//	2013-Apr-11, 12	Load Record flag
//
//	Don't adopt any svrs.
//
//	Used by DS_PullRecord2() when the DRM already exists here.
//
#define	RS_LR_FORCE_NEW_LINKS		0x00008000


//	2011-Mar-10		RCI  Reduced ClientInsts
//
//	The number of ClientInsts of a data is reduced when clients of the
//	same root context share the links of a data's svrs.  That is, the svrs,
//	under a given root context, then have just one ClientInst for each hS 
//	of the client data.
//
#define	RS_RCI_ENABLED			1



/*--------------------------------------------------------------------------*/
/*                                                                          */
/*	                    Named Pipe and Shared Memory                        */
/*                                                                          */
/*	App-DS  and  DS-DS interfaces.                                          */
/*--------------------------------------------------------------------------*/

//	This is something to use for allocating buffer space that will accomodate
//	messages with "typical" data sizes.
//
/*	2002-Dec-04

#define	NPM_TYP_DATA_SIZE	( 4 * 1024)
*/
//	2002-Dec-04
//
//	Increasing this size to match that of SMM_TYP_DATA_SIZE (see below).
//
/*
#define	NPM_TYP_DATA_SIZE	( 8 * 1024)
*/
#define	NPM_TYP_DATA_SIZE	(64 * 1024)


//	4G - 1 is the maximum message data size.  In other words, processes
//	should be able to point to memory and tell the pipe to send whatever
//	number of bytes from that memory.  On the receiving end the process
//	should point to memory and tell the pipe to put it there.
//
#define	NPM_MAX_DATA_SIZE	((4 * 1024 * 1024 * 1024) - 1)


//	This is something to use for allocating buffer space that will accomodate
//	messages with "typical" data sizes.
//
/*	2002-Aug-01
#define	SMM_TYP_DATA_SIZE	(   4 * 1024)
*/
//	Increase this for SMMT_Create_Table_UT.
//
/*	2010-Feb-13
#define	SMM_TYP_DATA_SIZE	(   8 * 1024)
*/
//	Increasing this for MRDInfo.
/*
#define	SMM_TYP_DATA_SIZE	(  64 * 1024)
*/
#define	SMM_TYP_DATA_SIZE	(1024 * 1024)



/*--------------------------------------------------------------------------*/
/*                                                                          */
/*	1999-Jun-07                Windows Messages                             */
/*                                                                          */
/*	Some of these message are used only in the DS or R & S Apps.  They are  */
/*	all placed here so there are no conflicts with their Ids (if that would */
/*	matter).                                                                */
/*--------------------------------------------------------------------------*/

//	wParam		0
//
//	lParam		EndPoint *
//
#define WM_CHANA_CALL			(WM_USER + 1)

//	wParam		0
//
//	lParam		EndPoint *
//
#define	WM_CHANA_MSG_IN_BUFFER	(WM_USER + 2)


//	Used by R & S App Thead windows to get the pointer to the thread object.
//
//	R & S App Thead windows send the message to the thread, with the window's
//	handle in the wParam.
//
//	The thread object responds by sending it back with the CRSAppThd * in 
//	the lParam.
//
//	wParam		HWND
//
//	lParam		CRSAppThd *
//
#define	WM_AT_PTR				(WM_USER + 3)


//	This message is sent to the main DS thread to start another service
//	thread.
//
//		wParam			0
//
//		lParam			0
//
#define WM_START_DS_THD			(WM_USER + 4)


//	This message is sent to the main DS thread to indicate that a diagnostic
//	has been added to the diagnostics file.  The purpose being that the
//	main thread may update a window displaying diagnostics.
//
//		wParam			0
//
//		lParam			Number of diagnostics in the file.
//
#define	WM_VIEW_NEW_DIAG		(WM_USER + 5)


//	For the RSP editor/viewer.
//
//	Set execution status button.
//
//		wParam			PE Id.			If 0 then set the main button.  Else
//										set a sub btn identified by this Id.
//
//		lParam			Code pointer.
//
#define	WM_SET_BTN				(WM_USER + 6)


//	For the RSP editor/viewer.
//
//	Highlight code piece.
//
//		wParam			PE Id.			
//
//	   (lParam & 0xFFFFFFFC) >> 2		Code pointer (CPtr).
//
//		lParam-bit0		Bool - scroll to see.
//
//		lParam-bit1		Bool - show.
//
#define	WM_HIGHLIGHT_CODEPIECE	(WM_USER + 7)


//	This message is sent to the main DS thread to display a "report" on a 
//	record (contexts, DRMs, etc.).
//
//	->	lParam (a pointer) is deleted when the message is processed.
//
//		wParam			0
//
//		lParam			Pointer to RecRpt.
//
#define	WM_UPDATE_DS_VIEW		(WM_USER + 8)


//	Posted when a record changes.  App must call CData::Notify().
//
//	Posted to the window's thread.
//
//		wParam			hWnd	The window to receive the message.
//
//		lParam			RegId	The Id supplied to the window when it
//								requests being notified.  See 
//								CData::AddNotifyee().
//	
//	2008-Nov-18		Adding hC info. 
//
//	Reciever must call GetNotifyerFromLog() to get the hC.
//
//	It is possible for the notifyer to have been deinstantiated by the
//	time the receiver processes the message - in which case hC will be 0.
//
//	wParam is no longer hWnd.  The message is always sent to a window
//	so that hWnd as a param is silly.
//
//		wParam			LogId	What is passed to GetNotifyerFromLog().
//
//		lParam			RegId	The Id supplied to the window when it
//								requests being notified.  See 
//								CData::AddNotifyee().
//	
#define	WM_DATA_CHANGED			(WM_USER + 9)


//	2000-Jul-25
//
//	Posted by entity show/hide dialog when an entity is clicked on in
//	in the entities list.  Only way I can figure to notify the dialog
//	* after * the check box image is updated.
//
//	2002-Sep-19		Use of lParam changed.  See DlgVPEntityShowHide.
//
//		wParam			hItem	Handle of the tree-list item.
//
//		lParam			2002-Sep-19		Upper 16 bits indicates which tree 
//						control was clicked in and possibly other things, 
//						lower 16 bits indicate things like keyboard status.
//
#define	WM_MY_TREE_NOTIFY		(WM_USER + 10)


//	2000-Jul-25
//
//	Posted by entity show/hide dialog when the dialog is being initialized.
//	For some reason I don't appreciate, CTreeControl::SetCheck() does not
//	seem to work in the CEntityShowHide::OnInitDialog() function.
//
//	By posting this message the tree-list items are checked after the
//	init dialog function (and the initialization of the controls I assume) 
//	is finished.
//
//	2002-Sep-19		Use of lParam changed.  See DlgVPEntityShowHide.
//
//		wParam			0
//
//		lParam			2002-Sep-19		Upper 16 bits indicates which tree 
//						control to the set the check in and possibly other 
//						things, 
//
#define	WM_MY_TREE_SET_CHECK	(WM_USER + 11)


//	2001-Nov-27
//
//	Messages sent by import mapping functions.
//
//	Originally created as a means of controlling things from the main IM
//	view (CIMBkGnd) when, for example, buttons are clicked in one of the
//	IM step dialogs.

//	Display import mapping help.
//
//		wParam			Help page/topic identifier.
//
//		lParam			0
//
#define	WM_IM_HELP				(WM_USER + 12)


//	Go back to the previous step.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_BACK_STEP			(WM_USER + 13)


//	Go to the next step.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_NEXT_STEP			(WM_USER + 14)


//	Cancel the import process.  Should cause the main IM GUI to close.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_CANCEL			(WM_USER + 15)


//	Ok
//
//		wParam			0
//
//		lParam			0
//
/*
#define	WM_IM_OK				(WM_USER + 16)
*/

//	Execute step 1.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_STEP1				(WM_USER + 17)


//	Execute step 2.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_STEP2				(WM_USER + 18)


//	Execute step 3.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_STEP3				(WM_USER + 19)


//	Set native type Id to which the currently selected imported type
//	is mapped to.  The native type Id is that of the selected native
//	type.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_MAP_TYPE			(WM_USER + 20)


//	Create the currently selected imported type and make the new type
//	that which is mapped to.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_IM_CREATE_TYPE		(WM_USER + 21)


//	Sent by UDUI main window on its WM_CLOSE event to the BPW.
//
//	To notify the BPW's UIEle of the close.
//
//		wParam		EleId	System element Id.
//
//		lParam		0
//
#define	WM_UDUI_CLOSING			(WM_USER + 22)


//	Sent by UDUI main window in its PostNcDestroy event to the BPW.
//
//	To notify the BPW's UIEle of the final destruction.
//
//		wParam		EleId	System element Id.
//
//		lParam		0
//
#define	WM_UDUI_DESTROYED		(WM_USER + 23)


//	2002-Aug-20		Same purpose as WM_DATA_CHANGED.  See above.
//
//	New and improved.  More efficient.
//
//	Instead of posting several notifications from one thread to the same
//	window of another thread they are instead grouped together and posted
//	in one message.
//
//		wParam		hWnd	The window to receive the message.
//
//		lParam		iP		The iP of a mms_a memory chunk.  The first
//							int in the chunk is the number of RegIds
//							following in the chunk.
//	
//	2008-Nov-19
//
//		lParam		iP		The iP of a mms_a memory chunk.  The first
//							uint in the chunk is the number of DChg2
//							structs following in the chunk.
//	
#define	WM_DATA_CHANGED2		(WM_USER + 24)


//	2002-Sep-22		To do things immediately after a dialog is displayed.
//	
//	Things that would otherwise be done in OnInitDialog() but that requre
//	the dialog being displayed to show status messages, etc.
//
//	In other words, ...
//
//	Dialog's OnInitDialg() posts this to do dialog initialization after 
//	the dialog is displayed.  In order to, for example, display status
//	messages while the dialog is being initalized - instead having nothing
//	nothing being displayed when the user expects to see a dialog.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_POST_INIT_DIALOG		(WM_USER + 25)


//	2002-Nov-11		To notify a thread that a RSP message is ready to be
//					received or sent.
//
//	This allows a mechanism so the receiver/sender is not looping in
//	in its RSP internals waiting for the other end to send/receive.
//
//	With this we use the Windows message as kind of an "interrupt" for
//	the receiving/sending thread to know that the sender/receiver has done
//	its thing.
//
//		wParam		hWnd	May be 0.  If not 0 then the window that the 
//							message is posted to.
//
//		lParam		EvtId	May be 0.  If not 0 then the VPE-to-PE event Id.
//							See CVPEle::Send().
//
#define	WM_RSP_MSG				(WM_USER + 26)


//	2003-Jan-02		To get the main app thread to destroy the startup
//					progress window.
//
//		wParam		0
//
//		lParam		0
//
#define	WM_CLOSE_SP				(WM_USER + 27)


//	2003-Jan-03		To get the main app thread to show the startup progress
//					window - but in the context of shutting down.
//
//					That is, we use the same progress window for startup
//					and shutdown.
//
//		wParam		0
//
//		lParam		0
//
#define	WM_SHUTDOWN_SP			(WM_USER + 28)


//	2003-Jan-06		After the app appears to be initialized the app's main
//					thread posts this message to the BPW thread to try to 
//					set the focus there.
//
//		wParam		0
//
//		lParam		0
//
#define	WM_FOCUS_SP				(WM_USER + 29)


//	2003-Jan-06		Posted on the main threads queue after the startup
//					or shutdown dialog window is destroyed.  Hopefully
//					this will help setting the focus to the proper window
//					at startup.
//
//		wParam		0
//
//		lParam		0
//
#define	WM_POST_NC_DESTROY_SP	(WM_USER + 30)


//	2003-Jan-06		Still having focus problems.  And in some cases when
//					shutting down, the SP window will not close unless
//					the mouse is moved over it - like the main thread's
//					message queue is messed up.
//
//					With this all threads must post to the main thread
//					in order to show a status message.
//
//		wParam		0
//
//		lParam		char * of the message.  The memory is deleted
//					when this message is received and processed.
//
#define	WM_SP_STATUS_MSG		(WM_USER + 31)


//	2003-Jan-06
//
//		wParam		0
//
//		lParam		0
//
#define	WM_FOCUS_AFTER_SP		(WM_USER + 32)


//	2003-Jan-06
//
//		wParam			0
//
//		lParam			0
//
#define	WM_MINIMIZE_MAIN_THD	(WM_USER + 33)


//	2003-Jan-10		The main thread posts this to the BPW thread when the
//					BPW thread still exists when the main thread attempts
//					to close.
//
//					The BPW thread must be closed before the main thread.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_MAIN_CLOSE			(WM_USER + 34)


//	2003-Jun-17		The file dialog posts this message when the user hits
//					the dialog's OK button.
//
//					The message is sent to the window that opened the dialog.
//
//		wParam			0
//
//		lParam			Pointer to a structure that indicates the file 
//						dialog's results.  The recipient of this message
//						is expected to delete this structure.
//
#define	WM_FILE_DLG_OK			(WM_USER + 35)


//	2003-Jun-17		The file dialog posts this message when the user hits
//					the dialog's Cancel button - or when the user closes
//					dialog without hitting the Ok button.
//
//					The message is sent to the window that opened the dialog.
//
//		wParam			0
//
//		lParam			Pointer to a structure that indicates the file 
//						dialog's results.  The recipient of this message
//						is expected to delete this structure.
//
#define	WM_FILE_DLG_CANCEL		(WM_USER + 36)


//	2003-Jul-10		Used to select a list control item.
//
//		wParam			Index of item to be selected.
//
//		lParam			Unused.
//
#define	WM_SELECT_LSTCTL_ITEM	(WM_USER + 37)


//	2003-Jul-10		Used to begin editing a list control item.
//
//		wParam			Index of item to be edited.
//
//		lParam			Unused.
//
#define	WM_EDIT_LSTCTL_ITEM		(WM_USER + 38)


//	2003-Jul-10		My list control (CMyListCtrl) sends this message to a 
//					window (probably the parent of the control) when it
//					gets a WM_CHAR.
//
//	Use SendMessage() to send this message.  If the result is 0 then
//	CMyListCtrl ignores the character (does not let the base class process
//	it).
//
//		wParam			nChar
//
//		lParam			Unused.
//
#define	WM_MYLISTCTRL_WM_CHAR	(WM_USER + 39)


//	2003-Jul-27		A parent PE posts this message to itself after enabling
//					a sub process to trace.
//
//		wParam			bByOV2		TRUE if the trace command is invoked 
//									from OV2.
//
//		lParam			Unused.
//
#define	WM_TRACE_PE				(WM_USER + 40)


//	2003-Aug-01		A child PE posts this message to itself after receiving
//					a message from the parent PE to run.
//
//		wParam			Unused.
//
//		lParam			Unused.
//
#define	WM_RUN_PE				(WM_USER + 41)


//	2003-Aug-14		Indicates YellowInfo window is destroyed.
//
//	The YellowInfo window sends this to its owner when it is destroyed.
//
//	The owner then deletes the window.
//
//		wParam			Unused.
//
//		lParam			Pointer of window being destroyed.
//
#define	WM_YIW_DESTROYED		(WM_USER + 42)


//	2003-Aug-19		Other threads post this to the BPW to get it to possibly
//					update dialogs, etc..
//
//	For example, a PE may create a new entity.  If the Entity Dialog is open
//	then its entity list should be updated.
//
//		wParam			Id of system element that posts the message.
//
//		lParam			Pointer to DCHG structure defined in RSAppThd.h.
//						The receiver of the message (the BPW thread) is
//						expected to delete this.
//
#define	WM_GEN_UPDATE			(WM_USER + 43)


//	2003-Sep-08		The IDI posts this message when a notifyee is not 
//					associated with a window.
//
//	Not being associated with a window means the message is posted to the
//	thread.
//
//		wParam			CData *.  The pointer to the notifyee as specified
//						by the notifyee.
//
//	2006-Mar-20
//
//		wParam			Notifyee log Id.  Allows the notifyee to be 
//						deinstantiated before the notification is serverd.
//
//
//		lParam			RegId.  That specified by the notifyee when it 
//						registered to be notified.
//
#define	WM_DATA_CHANGED_pD		(WM_USER + 44)


//	2003-Sep-08		The IDI posts this message when a notifyee is not 
//					associated with a window.
//
//	Not being associated with a window means the message is posted to the
//	thread.
//
//		wParam			CData *.  The pointer to the notifyee as specified
//						by the notifyee.
//
//	2006-Mar-20
//
//		wParam			Notifyee log Id.  Allows the notifyee to be 
//						deinstantiated before the notification is serverd.
//
//
//		lParam			iP		The iP of a mms_a memory chunk.  The first
//								int in the chunk is the number of RegIds
//								following in the chunk.
//	
//	2008-Nov-19
//
//		lParam			iP		The iP of a mms_a memory chunk.  The first
//								uint in the chunk is the number of DChg2
//								structs following in the chunk.
//	
#define	WM_DATA_CHANGED2_pD		(WM_USER + 45)


//	2004-Jan-06		When the last active DS service thread gets a 
//					disconnect from its application it posts this message to
//					the main DS thread.
//
//		wParam			0
//
//		lParam			0
//	
#define	WM_SHUTDOWN_DS			(WM_USER + 46)


//	2005-Jan-16		A msg sent to the main thread to wake its loop.
//
//	See comments for PulseMainThd() in App_v01.h.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_PULSE_MAIN_THD		(WM_USER + 47)


//	2009-Aug-22		For ImgProc library.
//
//	See use in ImgProc library.
//
//		wParam			W, H, V		Lowest 12 bits (0-11) is the width.
//									Bits 12 - 23 is the height.
//									Bits 24 - 27 specifies the view.  0 for 
//									'A', 1 for 'B' and so on.
//
//		lParam			BYTE *		Memory containing the image.
//
#define	WM_IMAGE_TO_BM				(WM_USER + 48)


//	2011-Aug-01
//
//	Sent across processes from a DS thread serving a named pipe connection
//	with another GP to a local app process.
//
//	Sent with SendMessage() so that the DS thread is blocked while the app
//	processes it.  See NPMT_REMOTE_OP of CDSThd::DoRemoteMsg().
//
//		wParam						GP ID requesting the op.
//
//		lParam						Thread ID of the DS thread serving the
//									named pipe connection.
//
//	So the app uses lParam to access (with a DIM call to the DS) the pipe 
//	message buffer and therefore everything about the operation.
//
#define	WM_REMOTE_OP				(WM_USER + 49)


//	2011-Aug-21
//
//	Sent across processes from a DS thread serving a named pipe connection
//	with another GP to a local app process.
//
//		wParam						A enum value of RemoteNotificationType.
//
//		lParam						Remote GP Id.  That of the GP that sent
//									the notification.
//
#define	WM_REMOTE_NOTIFICATION		(WM_USER + 50)


//	2011-Aug-25
//
//	A window posts this message to itself to remote PE info.
//
//		wParam						Remote GP Id.
//
//		lParam						Remote PE Id.
//
#define	WM_GET_REMOTE_PE_INFO		(WM_USER + 51)


//	2011-Aug-25
//
//	A window posts this message to itself to destroy a remomte PE rep.
//
//		wParam						Remote GP Id.
//
//		lParam						0
//
#define	WM_DESTROY_REMOTE_PE_REP	(WM_USER + 52)


/*
//	2011-Sep-15
//
//	A PE posts this to the BPW thread when the PE is commanded remotely to
//	start tracing.  The PE can/should not execute the tracing routine in the
//	context of processing the remote command.  So it posts this message so
//	that it can finish the command.
//
//		wParam						PE Id.
//
//		lParam						CPtr required by PE's Trace().
//
#define	WM_START_PE_TRACE			(WM_USER + 53)
*/
//	2011-Sep-16
//
//	Posted directly to the PE thread.
//
//		wParam						CPtr required by PE's Trace().
//
//		lParam						0
//
#define	WM_TRACE_PE2				(WM_USER + 53)


//	2011-Nov-07
//
//	A PE posts this to itself after receiving a submit command from a remote
//	PE.
//
//		wParam						Boolean, UI ok.
//
//		lParam						0
//
#define	WM_SUBMIT					(WM_USER + 54)


//	2011-Nov-07
//
//	The PE that has sent a submit command to a remote PE will post this to 
//	itself, after the submit is completed, to get the state of the remote
//	PE.
//
//		wParam						0
//
//		lParam						0
//
#define	WM_POST_SUBMIT				(WM_USER + 55)


//	2012-Sep-10
//
//		wParam						0
//
//		lParam						0
//
#define	WM_SNAPSHOT_PE				(WM_USER + 56)


//	2012-Sep-10
//
//		wParam						0
//
//		lParam						0
//
#define	WM_SNAPSHOT_RESTORE_PE		(WM_USER + 57)


//	2013-Apr-12
//
//	Posted by CDlgSystemPageSelect::OnSetActive() once (after the dialog is
//	first displayed) to set focus to a control other than that that is first 
//	in the tab order.
//
//		wParam			0
//
//		lParam			0
//
#define	WM_MY_SET_FOCUS				(WM_USER + 58)


//	2014-Aug-23
//
//	Remote2 stuff posts this to relay a message that was received from another
//	RR node to a thread that has registered to receive such messages on this 
//	node.
//
//	The message is included.  The app thread can examine it to determine 
//	what to do.
//
//		wParam			iP from MMS_Alloc().  That is the poster allocated
//						a buffer with the MMS stuff and copied the message 
//						(with the header) into it.  The thread that processes 
//						this WM_NODE_MSG is responsible for freeing that 
//						memory.
//
//		lParam			0
//
#define	WM_NODE_MSG					(WM_USER + 59)


//	Timer IDs.
//
#define	TMR_TRACE_PE		1

#define	TMR_THD_HPR			2



enum	SvrType
{
	stUNKNOWN		= 0,
	stUT			= 1,
	stMRD			= 2,
};


//	2012-Nov-01		Got to have the ODef type Id in mrd.cpp.
//
#define	ODEF_TYPE_NAME		"^ODef"


#define	iP_NULL				0xFFFFFFFF

#endif

//	rs_defs.h
