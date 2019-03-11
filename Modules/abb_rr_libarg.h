/*
		 1         2         3         4         5         6         7
1234567890123456789012345678901234567890123456789012345678901234567890123456789

	File:	libarg.h

	This defines a mechanism for passing and checking arguments to library
	functions called via RSP statements.
*/


#ifndef		__LIBARG_H
#define		__LIBARG_H


#include	"abb_rr_rs_typedefs.h"

#include    "abb_rr_RSPCallback.h"


struct	Ret;

class	CData;

class	CDVals;

class	CVariable;

class	CValue;

class	CWnd;

struct	ChanAMsg;

struct  LibArg;


enum	ArgType
{
	atUNDEFINED = 0,

	atTYPEMASK		= 0x000F,
	atPTRMASK		= 0x0100,
	atARRAYMASK		= 0x0200,

	atPTR			= 0x0100,
	atARRAY			= 0x0200,

	atCHAR			= 0x0001,
	atINT			= 0x0002,
	atDOUBLE		= 0x0003,
	atPVO			= 0x0004,
	atRID			= 0x0005,
	atDIDhC			= 0x0006,
	atM4			= 0x0007,
	atSTRUCT		= 0x0008,	//	See comments below.

	atCHAR_ARRAY	= 0x0201,	//	See comments below.
	atINT_ARRAY		= 0x0202,	//	See comments below.
	atDOUBLE_ARRAY	= 0x0203,
	atRID_ARRAY		= 0x0204,
	atDIDhC_ARRAY	= 0x0205,
	atM4_ARRAY		= 0x0206,
	atSTRING_ARRAY	= 0x0207,	//	See comments below.
	atSTRUCT_ARRAY	= 0x0208,
};		


//	atINT_ARRAY
//
//	The atINT_ARRAY arg will be a pointer to int.  Immediately after this 
//	there should be (packed in by the caller) one atINT.  It will be number 
//	of array items - i.e., the number of ints pointed at.
//
//	Note that RSP statements will * not * specify the atINT.  The number 
//	of array items will probably be packed in by CPC_ stuff at run time.
//
//		atINT_ARRAY			Ptr to int.
//
//			atINT				Number of array items.
//
//	No.  Added/use SzItm, nItms in LibArg.

//	atCHAR_ARRAY
//
//	The atCHAR_ARRAY will be a pointer to char.  Immediately after this
//	there should be (packed in by the caller) one atINT.  It will be the
//	number chars the memory pointed can hold.
//
//	The value of the atINT will be the * size * of memory allocated.  That
//	is, it will be 1 + the max length of string held by the memory because
//	the size includes the terminating '\0'.
//
//	The LibArg::pArg points at the same memory contents as a atCHAR | atPTR.
//	This type is used when the called library function needs the size
//	of memory pointed at.
//
//	This type should be specified instead of atCHAR | atPTR.
//
//	->	No.  Not in all cases.  Use atCHAR | atPTR for const strings.
//
//	Note that RSP statements will * not * specify the atINT.  The number 
//	of memory bytes allocated for the chars will probably be packed in 
//	by CPC_ stuff at run time.
//
//		atCHAR_ARRAY		Ptr to char.
//
//			atINT				Number of array items.
//
//	No.  Added/use SzItm, nItms in LibArg.


//	atSTRING_ARRAY
//
//	The atSTRING_ARRAY arg will be a pointer to BYTE.  Immediately after 
//	this there should be (packed in by the caller) two atINTs.  The first 
//	will be each string's size (counting null byte - i.e., 
//	TypeSpec::SizeString), the second will be number of array items.
//
//	Therefore the total number of bytes pointed at by atSTRING_ARRAY
//	will be the atINT value after the atSTRING_ARRAY multiplied by the
//	next atINT.
//
//	Note that RSP statements will * not * specify these atINTs.  The string
//	size and number of array items will probably be packed in by CPC_
//	stuff at run time.
//
//		atSTRING_ARRAY		Ptr to BYTE.
//
//			atINT				Each string's size.
//
//			atINT				Number of array items.
//
//	No.  Added/use SzItm, nItms in LibArg.


//	atSTRUCT
//
//	pArg points to first member of struct.  It points to CValue.
//
//	So, for example, if the first member is an int then pArg will point
//	to a Cvalue of size sizeof(CValue).



//	Library Argument Flags.
//
#define	LAF_ZERO		0x00000000

#define	LAF_CHANGED		0x10000000


//	2014-Jul-14		Delete pArg.  No matter what.
//
#define	LAF_DELETE_ARG	0x00000010



//	2002-Jul-15
//
//	Special returns/flags to support long library operations.
//
//	These are specified in the  nLibArgs  parameter on return from the DLL
//	function call.
//
//	These may also be returned as ENo values.  The caller should check
//	ENo against this bit to determine if its a LAR_ value or an ENo.
//
#define	LAR_NOT_AN_ENO		0x10000000

//	LAR_IN_PROGRESS		Call function is still in progress.  The function
//						should be called again until this is not returned.
//						The Ntrp should probably check for Windows messages 
//						and update windows.
//
//	For example, a robot is moving and view-ports should be updated at each
//	incremental move.
//
#define	LAR_IN_PROGRESS		0x10000001

//	2003-Oct-31		Adding wait time bits to LAR_IN_PROGRESS.
//
//	The library function can set these bits to indicate the PE should sleep
//	(or do something else, like the Windows message loop) the specified time
//	before calling the library funciton again.
//
//	LIP: LAR In Progress.
//
//	These wait time bits, if specified, are set in the same return word
//	that LAR_IN_PROGRESS is set in.
//
#define	LIP_WAIT_TIME_MASK			0x0FFFFF00

#define	GET_LIP_WAIT_TIME(w)		((w & LIP_WAIT_TIME_MASK) >> 8)

#define	SET_LIP_WAIT_TIME(w,t)		((w |= ((t << 8) & LIP_WAIT_TIME_MASK)))







//	PECB - ProcEle CallBack
//
//	A function provided to DLLs that they can call back in order to send 
//	messages to other threads.
//
//	The message is a ChanAMsg.
//
//	The function is provided to the DLL via the PECB structure.  The user's
//	library function calls the callback function passing the PECB structure
//	back.  Before the call the user would set the structure parameters.
//
//	Returns ENo.
//
struct	PECB;


#define	PECB_CMD_MASK		0x00FFFFFF

#define	PECB_CMD_FLAGS_MASK	0xFF000000

//	Send a message.
//
#define	PECB_CMD_SEND		0x00000001

#define	PECB_CMD_NO_RSP		0x01000000


//	Have the BPW post a notification.
//
//	When the library calls the memory of iP should be set as follows:
//
//		DIDhC	Id and hC of data that has changed or been added.
//
//		int		The change type.  Eg., DCHG_REC_ADD.
//
//	The memory (iP) passed in the call will be freed in its function.
//
//	When the call returns the library iP will not be set (ie., nothing is
//	returned to the library).
//
#define	PECB_CMD_GEN_UPDT	0x00000100


//	To display status messages during long operations.
//
//	The memory of iP will be a 0 terminated text string.
//
//	The memory (iP) passed in the call will be freed in its function.
//
//	No return (iP on return will not be set).
//
#define	PECB_CMD_STATUS		0x00000200


//	Library is doing a long operation.  App should do windows messages.
//
#define	PECB_IN_PROGRESS	0x00000400


//	To synchronize multiple PEs with the viewport(s).  
//
//	Before moving a world entity SyncVPs() is called.  When all the positions
//	of entities have been set ReleaseVPs() is called.  If multiple PEs (or
//	the library code called by a PE) sync a the same time then no VPs are
//	redrawn until  * all *  those PEs release.
//
#define	PECB_SYNC_VPS		0x00000800

#define	PECB_RELEASE_VPS	0x00001000

#define	PECB_SYNC_VPS2		0x00002000

#define	PECB_RELEASE_VPS2	0x00004000

#define	PECB_SYNC_VPS3		0x00008000


//	2013-Oct-15		Callbacks from built-in library functions.
//
//	The first 4 bytes (an int) indicates the funciton to be done during 
//	the callback.  See BIL_CB_FNC_ definitions in bilib.h.
//
#define	PECB_BIL_CALLBACK	0x00010000



//	PECB Flags
//
//	Library function set this flag when it calls Notify() on some data.
//
//	R & S looks for this flag when running (as opposed to stepping).  If
//	set then R & S will break out of its tight bytecode execution loop
//	and do the notifications.
//
#define	PECBF_NOTIFICATION_POSTED		0x00000001

//	2011-Jan-17
//
//	To synchronize the VP updates when multiple PEs are moving entities
//	at the same time.
//
#define	PECBF_SYNCH_PES_REGISTER		0x00000002

#define	PECBF_SYNCH_PES_UNREGISTER		0x00000004



typedef	int (* PECBFunc) ( PECB * pCB );

#ifdef	WIN32
struct PORT	PECB		//	This is whats passed to the library function.
#else
struct		PECB
#endif
{
	//	PECB ( int PE_Id, PECBFunc, void *, void *,     CWnd * );
		PECB ( int PE_Id, PECBFunc, void *, PECallback, CWnd * );

	//	PECB ( int PE_Id, PECBFunc, void *, void *,     CWnd *, uint ChgFlags );
		PECB ( int PE_Id, PECBFunc, void *, PECallback, CWnd *, uint ChgFlags );

		int			PE_Id;

		PECBFunc	CallBack;	//	User's library function calls this 
								//	function passing back this structure in
								//	the call.

		//	Before the user calls CallBack() it sets these parameters and
		//
		//	CallBack() will free memory of iP that was set on the call.
		//
		//	When CallBack() returns iP may be set to other memory containing
		//	results.  Otherwise iP will be 0 on return.
		//
		//	2003-Nov-08	
		//
		//	Now this call-back facility may be used for additional purposes.
		//
		//	See PECB_CMD_GEN_UPDT, for example.
		//
		int			Cmd;		//	Usually PECB_CMD_SEND.  May be some other
								//	command that does not involve sending a 
								//	message - iP would be used to pass 
								//	parameters and results.

		int			DstSE_Id;	//	Destination element Id.  If -1 then sent 
								//	to the BPW.

		uint		iP;			//	On call this is the MMS memory containing 
								//	the message sent to the destination.  On 
								//	return this will be that message sent back 
								//	from the destination.

		uint		Flags;		//	2009-Jul-13		See PECBF_ macros.

		uint		ChgFlags;	//	2009-Dec-21		See DCHGF_ macros.

		CWnd *		pWnd;		//	2011-Jan-03		Calling window.

		void *		p;			//	2011-Jan-17		General pointer.


	//	void * 		Get_pPE() { return pPE; }

	//	void *		Get_cbPE() { return cbPE; }

	private:

		friend int	cpLibCallbackFunc ( PECB * pPECB );

		friend int	  LibCallbackFunc ( PECB * pPECB );

		void *		pPE;		//	User library should not muck with this.

	//	void *		cbPE;		//	User library should not muck with this.
		PECallback	cbPE;		//	User library should not muck with this.

};	//	PECB




//	Sends a message to the BPW telling it to call a function.
//
//	The function parameters must be 32 bit values because they are taken
//	out of the ...  Pointers are okay.
//
//	Multiple values can be recieved as well.
//
//	See CDModelDlg and CDVP for examples.
//
//	TypeRcv can be camtUndefined - in which case no response will be
//	expected (and, of course, nParamsRcv should be 0).
//
//	Returns ENo.
//	
#ifndef JS
PORT int	SendCall ( const char *			sW,
							 PECB *			pCB, 
							 int			SE_Id,
							 ChanAMsgType	TypeSnd,
							 ChanAMsgType	TypeRcv,
					   const char *			TextSnd,
							 uint			nParamsSnd, 
							 uint			nParamsRcv, ... );
#endif


//	Send a message to the BPW.
//
//	pAMsg is the message to send when called, the message received in
//	response on return.
//
//	This -
//	
//		-	Unlocks pAMsg.
//
//	If RsltType != camtUndefined -
//
//		-	Leaves pAMsg locked on return - unless -
//
//		-	If there is an error then pAMsg is unlocked and freed on return.
//
//	If no response is expected (if RsltType == camtUndefined) then there
//	will be no pAMsg on return.
//
//	Returns ENo.
//
#ifndef JS
PORT int	SendToBPW ( const char *		sW,
							  ChanAMsg * &	pAMsg, 
							  PECB *		pCB,
							  ChanAMsgType	RsltType );
#endif




//	Administration functions.  These are defined in every library.
//
typedef int	(* LIBFUNCP)(...);


//	Application, library specific, functions.
//
typedef Ret *	(* LIBUSERFUNCP)( PECB *	pCB,			//	2002-Nov-14
								  CData *	pObj, 
								  int *		nArgs, 
								  LibArg *	pArgs );


#endif

//	File:	libarg.h
