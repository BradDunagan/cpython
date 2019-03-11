/*
	File:	error.h

	03-Aug-98

	A critical section object is used to protect a "pool" of error messages
	from multiple threads.

	When an error message is added it is put into the pool and a key 
	value (an integer) is returned to the caller.  The caller must provide 
	that key value to get the error message out of the pool.

	If the error pool becomes full then E() will return -1.  But the error
	message will be submitted to diagnostics.
	
	Therefore, a thread should call GetE() to free up the space used by E()
	so that other threads can submit errors.


	1999-Jul-02

	See file ae_.txt for explanations regarding AE_ errors.


	1999-Sep-28

	A program may have multiple diagnostics facilities (files, views, etc.).
	These functions submit diagnostics to the global facilities.

	That is, the global functions of diags.h, .cpp are called to log error 
	messages.
*/

#ifndef		__ERROR_H
#define		__ERROR_H

#include	"abb_rr_rs_typedefs.h"

#include	"abb_rr_rs_defs.h"


#define PORT


//	Application user errors.
//
//	08-Jan-1999		See notes in Journal.doc.
//
//	Hopefully, the application user will have done something to cause an 
//	error (instead of the application/DS developer).  In which case the DS 
//	will call M() and pass one of these values - except AE_DS_ERROR - M() 
//	should never be called with AE_DS_ERROR.
//
//	Only if the error is some DS flaw is AE_DS_ERROR used.  The E() function 
//	assigns AE_DS_ERROR to the error type.
//
//	2000-May-30
//
//	Changed AE_DS_ERROR to AE_INT_ERROR to include internal errors in
//	application code.
//
#define	AE_NO_ERROR					0

#define	AE_INT_ERROR				(-1)


#define	AE_RECORD_TOO_MANY_DRMS		(-2)
#define	AE_CONTEXT_TOO_MANY_DRMS	(-3)
#define	AE_DATA_TOO_LARGE			(-4)
#define	AE_OUT_OF_MEMORY			(-5)
#define	AE_CACHE_TOO_LARGE			(-6)
#define	AE_CACHE_ITEM_CAP_TOO_HIGH	(-7)
#define	AE_OUT_OF_CACHE_ITEMS		(-8)
#define	AE_ERROR					(-9)
#define	AE_OUT_OF_MMS_BLOCKS		(-10)
#define	AE_DRM_NOT_A_SERVER			(-11)
#define	AE_EXCEEDED_CHUNK_CAP		(-13)
#define	AE_OUT_OF_HEAP				(-14)
#define	AE_TOO_MANY_THDS_IN_REC_CTX	(-15)
#define	AE_TOO_MANY_ACTIVE_CTXS		(-16)
#define	AE_DEST_TOO_SMALL			(-17)
#define	AE_INAPPROPRIATE_TYPE_ID	(-18)
#define	AE_BAD_TYPE_ID				(-19)
#define	AE_TOO_MANY_DATASVRS		(-20)
#define	AE_DUPLICATE_DS_NAME		(-21)
#define	AE_USE_hS					(-23)


#define	AE_TOO_MANY_APPLICATIONS	(-1001)
#define	AE_NAME_TOO_LONG			(-1002)
#define	AE_NAMED_TYPE_NOT_LOCAL		(-1003)
#define	AE_NAMED_RECORD_NOT_LOCAL	(-1004)
#define	AE_MULTIPLE_NAMED_RECORDS	(-1005)
#define	AE_NO_MMS_HOLE				(-1006)
#define	AE_OUT_OF_MMS_AREAS			(-1007)
#define	AE_NAME_NOT_UNIQUE			(-1008)
#define	AE_CAN_NOT_INHERIT_CONTEXT	(-1009)
#define	AE_SC_OP_NOT_COMPELETED		(-1010)
#define	AE_BAD_NAME					(-1011)
#define	AE_UNRECOGNIZED_USER_NAME	(-1012)


#define	AE_UT_TOO_MANY_KEYS			(-2001)
#define	AE_UT_COMP_KEY_TOO_LONG		(-2002)
#define	AE_UT_BAD_KEY_NAME_CHAR		(-2003)
#define	AE_UT_KEY_NAME_TOO_LONG		(-2004)
#define	AE_UT_BAD_INDEX_DEFINITION	(-2005)
#define	AE_UT_INVALID_KEY_TYPE		(-2006)
#define	AE_UT_INVALID_KEY_NAME		(-2007)
#define	AE_UT_INDEX_NAME_TOO_LONG	(-2008)
#define	AE_UT_TABLE_NAME_TOO_LONG	(-2009)
#define	AE_UT_TABLE_DEF_TOO_LONG	(-2010)
#define	AE_UT_INDEX_NOT_FOUND		(-2011)
#define	AE_UT_TOO_MANY_FIELDS		(-2012)
#define	AE_UT_INVALID_FIELD_TYPE	(-2013)
#define	AE_UT_FIELD_TYPE_MISMATCH	(-2014)
#define	AE_UT_NOT_ENOUGH_FIELDS		(-2015)
#define	AE_UT_QUERY_NOT_SET			(-2016)
#define	AE_UT_TOO_MANY_INDEXES		(-2017)
#define	AE_UT_MULTI_INDEXES_TYPE_A	(-2018)
#define	AE_UT_BASE_NOT_TYPE_A		(-2019)
#define	AE_UT_BAD_FIELD_NAME_CHAR	(-2020)
#define	AE_UT_FIELD_NAME_TOO_LONG	(-2021)
#define	AE_UT_BAD_TABLE_DEFINITION	(-2022)
#define	AE_UT_QUERY_IS_TOO_LONG		(-2023)
#define	AE_UT_INCONSISTENT_CONJUNCS	(-2024)
#define	AE_UT_EXPECTED_CONJUNC		(-2025)
#define	AE_UT_QUERY_SYNTAX			(-2026)
#define	AE_UT_LITERAL_OUT_OF_RANGE	(-2027)
#define	AE_UT_LITERAL_KEY_MISMATCH	(-2028)
#define	AE_UT_EXPECTED_NUMBER		(-2029)
#define	AE_UT_EXPECTED_STRING		(-2030)
#define	AE_UT_LITERAL_TOO_LONG		(-2031)
#define	AE_UT_IDENTIFIER_TOO_LONG	(-2032)
#define	AE_UT_MAX_TABLES			(-2033)
#define	AE_UT_TABLE_NOT_FOUND		(-2034)
#define	AE_UT_IMPROPER_INDEX		(-2035)
#define	AE_UT_KEY_TYPE_MISMATCH		(-2036)
#define	AE_UT_DUP_COMP_KEY			(-2037)
#define	AE_UT_DUP_COMP_KEYITM		(-2038)
#define	AE_UT_OUT_OF_SPACE			(-2039)
#define	AE_UT_RECORD_TOO_LONG		(-2040)
#define	AE_UT_RECORD_BUF_TOO_SMALL	(-2041)
#define	AE_UT_TOO_MANY_LONG_FIELDS	(-2042)
#define	AE_UT_ONE_INDEX				(-2043)
#define	AE_UT_TOO_MANY_CONDITIONS	(-2044)
#define	AE_UT_TOO_MANY_ACTIVE_CTXS	(-2045)
#define	AE_UT_RECORD_HAS_LNG_FLDS	(-2046)
#define	AE_UT_TABLE_EXISTS			(-2047)
#define	AE_UT_INDEX_EXISTS			(-2048)
#define	AE_UT_REF_NOT_FOUND			(-2049)
#define	AE_UT_IMPROPER_INDEX_FIELD	(-2050)
#define	AE_UT_NO_ONGOING_QUERY		(-2051)
#define	AE_UT_MAX_ONGOING_QUERYS	(-2052)
#define	AE_UT_SQLITE				(-2053)



//	Robot Scripting & Programing (RSP)
//
//	Robots & Systems Commands (RSC)
//
#define	AE_RSP_NEW_SRC_TOO_LONG		(-3001)
#define	AE_RSP_NO_ELE_WITH_ID		(-3002)
#define	AE_RSP_ELE_IS_NOT_PE		(-3003)
#define	AE_RSP_NO_PE_WITH_ID		(-3004)
#define	AE_RSP_NO_MDE_WITH_ID		(-3005)
#define	AE_RSP_OBJ_CHECK_FAILED		(-3006)
#define	AE_RSP_OBJ_NOT_ADOPTED		(-3007)
#define	AE_RSP_CURRENT_ODEF_IS_NEW	(-3008)
#define	AE_RSP_CALL_STACK			(-3009)
#define	AE_RSP_EDITOR_IS_CLOSED		(-3010)
#define	AE_RSP_FILE_OPEN			(-3011)
#define	AE_RSP_FILE_TOO_LARGE		(-3012)
#define	AE_RSP_FILE_READ			(-3013)
#define	AE_RSP_FILE_WRITE			(-3014)
#define	AE_RSP_ES_NOT_IN_PROGRESS	(-3015)
#define	AE_RSP_INVALID_STMT			(-3016)
#define	AE_RSP_EOT					(-3017)
#define	AE_RSP_OBJ_DEF_NOT_NEW		(-3018)
#define	AE_RSP_HDR_NOT_NEW			(-3019)
#define	AE_RSP_NAME_TOO_LONG		(-3020)
#define	AE_RSP_NO_JUMP_DURING_ES	(-3021)
#define	AE_RSP_STMT_CP_NOT_FOUND	(-3022)
#define	AE_RSP_INVALID_JUMP			(-3023)
#define	AE_RSP_INTER_SE_TIMEOUT		(-3024)
#define	AE_RSP_NO_ESFLAGS			(-3025)
#define	AE_RSP_DUP_NAMES			(-3026)
#define	AE_RSP_USER_CANCELED_CHECK	(-3027)
#define	AE_RSP_USER_REVERTED		(-3028)
#define	AE_RSP_USER_CANCELED		(-3029)
#define	AE_RSP_TOO_MANY_PES			(-3030)
#define	AE_RSP_MULT_NAMES_OF_1TYPE	(-3031)
#define	AE_RSP_INVALID_PATH			(-3032)
#define	AE_RSP_PATH_TOO_LONG		(-3033)
#define	AE_RSP_FILEPATH_TOO_LONG	(-3034)
#define	AE_RSP_FILENAME_TOO_LONG	(-3035)
#define	AE_RSP_NO_VPE_WITH_ID		(-3036)
#define	AE_RSP_NO_MDE_WITH_NAME		(-3037)
#define	AE_RSP_NO_VPE_WITH_NAME		(-3038)
#define	AE_RSP_NO_PE_WITH_NAME		(-3039)
#define	AE_RSP_CMD_REC_NOT_ARCHIVED	(-3040)
#define	AE_RSP_CMD_DEF_NOT_EXIST	(-3041)
#define	AE_RSP_ILLEGAL_STRUCT_OP	(-3042)
#define	AE_RSP_REF_TO_SAME_DEF		(-3043)		//	Rec mbr ref same def.
#define	AE_RSP_NUMBER_OF_ARGS		(-3044)
#define	AE_RSP_PRINTF				(-3045)
//
//	RSC
//
//		IObj errors
//
#define		AE_RSP_IOBJ				(-3051)
//
//	RSC
//
//		Message statement errors
//
#define	AE_RSP_CHANNEL				(-3061)
#define	AE_RSP_ALT					(-3062)



//	RSP and UDUI
//
#define	AE_RSP_NOT_FROM_UDUI		(-3101)
#define	AE_RSP_BAD_WND_ID			(-3102)
#define	AE_RSP_BAD_WND_NAME			(-3103)
#define	AE_RSP_WRONG_WND_TYPE		(-3104)
#define	AE_RSP_TEXT_TOO_LONG		(-3105)
#define	AE_RSP_LB_MULTISELECTED		(-3106)
#define	AE_RSP_LB_SINGLESELECT		(-3107)
#define	AE_RSP_LB_LAST_NOT_FOUND	(-3108)
#define	AE_RSP_LB_ITEM_NOT_FOUND	(-3109)
#define	AE_RSP_UPAGE_NOT_FOUND		(-3110)
#define	AE_RSP_INSUFFICIENT_SIZE	(-3111)
#define	AE_RSP_BUFFER_TOO_SMALL		(-3112)

//	RSP and Model Dialog
//
#define	AE_RSP_MDL_NOT_SELECTED		(-3201)
#define	AE_RSP_BAD_COMP_ID			(-3202)
#define	AE_RSP_SELECT_PG_NOT_INIT	(-3203)
#define	AE_RSP_COLOR_PG_NOT_INIT	(-3204)
#define	AE_RSP_COLOR_NAME_NOT_FOUND	(-3205)
#define	AE_RSP_ILLEGAL_BOX			(-3206)
#define	AE_RSP_ILLEGAL_CONE			(-3207)
#define	AE_RSP_ILLEGAL_TORUS		(-3208)
#define	AE_RSP_COMP_NAME_TOO_LONG	(-3209)
#define	AE_RSP_MULT_COMPS_SAME_NAME	(-3210)
#define	AE_RSP_COMP_NAME_NOT_FOUND	(-3211)
#define	AE_RSP_TYPE_PATH_TOO_LONG	(-3212)
#define	AE_RSP_TEX_NAME_NOT_FOUND	(-3213)
#define	AE_RSP_TEX_NOT_SELECTED		(-3214)

//	Running RSP fast code (CPC_Stmt).
//
#define	AE_RUN_NULL_PTR				(-3301)
#define	AE_RUN_BILIB_ARG			(-3302)
#define	AE_RUN_CHAN_BUFFER_SIZE		(-3302)
#define	AE_RUN_CHAN_BOTH_SNDING		(-3304)
#define	AE_RUN_CHAN_BOTH_RCVING		(-3305)
#define	AE_RUN_CHAN_MSG_TYPE		(-3306)
#define	AE_RUN_ARRAY_INDEX			(-3307)
#define	AE_RUN_STRING_TOO_LONG		(-3308)
#define	AE_RUN_STRING_ARRAYS		(-3309)
#define	AE_RUN_BILIB_VALID_TYPE_ID	(-3310)
#define	AE_RUN_ILLEGAL_CAST			(-3311)
#define	AE_RUN_UNEXPECTED_MBR_TYPE	(-3312)
#define	AE_RUN_TOO_MANY_ARGS		(-3313)
#define	AE_RUN_DIVISION_BY_0		(-3314)
#define	AE_RUN_TYPE_DOES_NOT_EXIST	(-3315)
#define	AE_RUN_BAD_DID				(-3316)
#define	AE_RUN_FUNC_NOT_MEMBER		(-3317)
#define	AE_RUN_NOT_A_MEMBER			(-3318)
#define	AE_RUN_INCOMPAT_TYPE		(-3319)
#define	AE_RUN_ILLEGAL_PTR			(-3320)
#define	AE_RUN_ILLEGAL_DID			(-3321)
#define	AE_RUN_ILLEGAL_REC_OP		(-3322)
#define	AE_RUN_NOT_A_PTR			(-3323)
#define	AE_RUN_UNEXPECTED_TYPE		(-3324)
#define	AE_RUN_ILLEGAL_PTR_OP		(-3325)
#define	AE_RUN_ILLEGAL_ARG			(-3326)


//	RSP and User-Written DLLs.
//
#define	AE_RSP_DLL_USER_ERROR		(-3401)
#define	AE_RSP_DLL_LIB_IF_ERROR		(-3402)
#define	AE_RSP_DLL_INCORRECT_ARG	(-3403)
#define	AE_RSP_DLL_NO_ERROR_FUNC	(-3404)
#define	AE_RSP_DLL_NO_REQ_FUNC		(-3405)
#define	AE_RSP_DLL_NO_GRIP_ORG		(-3406)
#define	AE_RSP_DLL_NO_BASE			(-3407)
#define	AE_RSP_DLL_BAD_LINK_IDX		(-3408)


//	RSP and Bags and Sets
//
#define	AE_RSP_INVALID_FIELD_SPECS	(-3501)
#define	AE_RSP_INVALID_KEY_SPECS	(-3502)
#define	AE_RSP_BAD_TBL_NBR			(-3503)
#define	AE_RSP_BAD_FLD_NBR			(-3504)
#define	AE_RSP_BAD_IDX_NBR			(-3505)

#define	AE_RSP_SET_DUP_KEY			(-3551)
#define	AE_RSP_SET_NOT_FOUND		(-3552)
#define	AE_RSP_SET_TOO_MANY_INDXS	(-3553)
#define	AE_RSP_SET_FULL				(-3554)


//	RSP and Entity DLL
//
#define	AE_RSP_ENT_NOT_SELECTED		(-3601)
#define	AE_RSP_ENT_ID_NOT_OF_ENTITY	(-3602)
#define	AE_RSP_ENT_TYPE_ID			(-3603)
#define	AE_RSP_MDL_ID_NOT_OF_MODEL	(-3604)
#define	AE_RSP_MDL_TYPE_ID			(-3605)


//	RSP and View-ports
//
#define	AE_RSP_VP_IS_CLOSED			(-3701)
#define	AE_RSP_VP_NULL_ENTITY_ID	(-3702)
#define	AE_RSP_VP_NULL_RECORD_ID	(-3703)
#define	AE_RSP_VP_UNRECOG_EVENT		(-3704)
#define	AE_RSP_VP_ENTITY_NOT_FOUND	(-3705)
#define	AE_RSP_VP_ORTHO				(-3706)


//	RSP and COW
//
#define	AE_RSP_COW_BAD_PIC_SPEC		(-3801)
#define	AE_RSP_COW_BAD_IMG_SPEC		(-3802)
#define	AE_RSP_COW_BAD_EDGE_OP		(-3803)
#define	AE_RSP_COW_BAD_IMG_SMPL_SZ	(-3804)
#define	AE_RSP_COW_BAD_PARAM		(-3805)
#define	AE_RSP_COW_IV_WRONG_THREAD	(-3806)
#define	AE_RSP_COW_BAD_HANDLE		(-3807)
#define	AE_RSP_COW_NEED_MORE_IMGS	(-3808)
#define	AE_RSP_COW_NO_WORLD			(-3809)
#define	AE_RSP_COW_TYPE_MISMATCH	(-3810)
#define	AE_RSP_COW_MOVE_IN_PROGRESS	(-3811)
#define	AE_RSP_COW_SAME_SVR			(-3812)
#define	AE_RSP_COW_BAD_POS_NBR		(-3813)


//	COM ports
//
#define	AE_COM_PORT_NAME			(-3851)


//	RSP and GenRobot DLL
//
#define	AE_RSP_BAD_ENTITY_SPEC		(-3901)
#define	AE_RSP_ENTITY_NOT_SELECTED	(-3902)
#define	AE_RSP_INVALID_JOINT_INDEX	(-3903)
#define	AE_RSP_INVALID_JOINT_VALUE	(-3904)
#define	AE_RSP_INVALID_TRANS_ORDER	(-3905)
#define	AE_RSP_INVALID_KINEMATICS	(-3906)
#define	AE_RSP_INVALID_JOINT_TYPE	(-3907)
#define	AE_RSP_MOD_NAME_TOO_LONG	(-3908)



//	Entities, Models & Components
//
#define	AE_EMC_B_NOT_INSIDE_A		(-4001)
#define	AE_EMC_B_ALL_INSIDE_A		(-4002)
#define	AE_EMC_VRTS_NOT_ON_SURFACE	(-4003)
#define	AE_EMC_BOX_HOLE				(-4004)
#define	AE_EMC_TOO_MANY_COMP_ORGS	(-4005)
#define	AE_EMC_NO_COPLANAR_SURFACE	(-4006)
#define	AE_EMC_SURFACE_NOT_FLAT		(-4007)
#define	AE_EMC_TOO_MANY_COPLANARS	(-4008)
#define	AE_EMC_TOO_MANY_BUTT_VGRPS	(-4009)
#define	AE_EMC_COMPS_INTERSECT		(-4010)



//	Importing, exporting.
//
#define	AE_IMP_USER_CANCELED		(-5001)
#define	AE_IMP_FILE_OPEN			(-5002)
#define	AE_IMP_FILE_CLOSE			(-5003)
#define	AE_IMP_FILE_SEEK			(-5004)
#define	AE_IMP_FILE_READ			(-5005)
#define	AE_IMP_LINE					(-5006)
#define	AE_IMP_BAD_TYPE				(-5007)
#define	AE_IMP_BAD_INT				(-5008)
#define	AE_IMP_BAD_DOUBLE			(-5009)
#define	AE_IMP_BAD_CHAR				(-5010)
#define	AE_IMP_BAD_STRING			(-5011)
#define	AE_IMP_BAD_OBJECT			(-5012)
#define	AE_IMP_OBJECT_RECURSION		(-5013)
#define	AE_IMP_OBJECT_NOT_FOUND		(-5014)
#define	AE_IMP_OVER_RUN				(-5015)
#define	AE_IMP_HAVE_ERROR			(-5016)
#define	AE_IMP_ID					(-5017)
#define	AE_IMP_TYPE_RECURSION		(-5018)
#define	AE_IMP_TYPE_NOT_IMPORTED	(-5019)
#define	AE_IMP_TYPE_B4_DATA			(-5020)
#define	AE_IMP_ISA_ERROR			(-5021)
#define	AE_IMP_NO_FOREIGN_TYPE		(-5022)
#define	AE_IMP_FT_NOT_MAPPED		(-5023)
#define	AE_IMP_GET_TYPE				(-5024)

#define	AE_EXP_USER_CANCELED		(-5101)
#define	AE_EXP_FILE_OPEN			(-5102)
#define	AE_EXP_FILE_WRITE			(-5103)
#define	AE_EXP_FILE_CLOSE			(-5104)
#define	AE_EXP_OBJECT_RECURSION		(-5105)
#define	AE_EXP_TREE_TOO_DEEP		(-5106)
#define	AE_EXP_NAME_TOO_LONG		(-5107)



//	Reporting
//
#define	AE_RPT_SCTN_NOT_FOUND		(-6001)
#define	AE_RPT_SCTN_ASSOC			(-6002)


//	Remote GP
//
#define	AE_RMT_NOT_CONNECTED		(-6501)


//	World
//
#define	AE_WRLD_CHILD_NOT_FOUND		(-7001)
#define	AE_WRLD_NOT_PARENT			(-7002)
#define	AE_WRLD_COMP_NOT_FOUND		(-7003)
#define	AE_WRLD_BAD_TYPE_ID			(-7004)
#define	AE_WRLD_BAD_MODEL_ID		(-7005)
#define	AE_WRLD_ENT_NOT_FOUND		(-7006)
#define	AE_WRLD_ENT_NOT_SELECTABLE	(-7007)


//	Image Processing
//
#define	AE_IP_OPEN_CV				(-7501)
#define	AE_IP_VP_SCENE_NOT_BUFFERED	(-7502)
#define	AE_IP_WINDOW_MUST_BE_OPEN	(-7503)
#define	AE_IP_UNEXPECTED_SRC		(-7504)
#define	AE_IP_BAD_INDEX				(-7505)
#define	AE_IP_BAD_PIC_SPEC			(-7506)
#define	AE_IP_BAD_IMG_SPEC			(-7507)
#define	AE_IP_BAD_EDGE_OP			(-7508)
#define	AE_IP_BAD_IMG_SMPL_SZ		(-7509)
#define	AE_IP_BAD_PARAM				(-7510)
#define	AE_IP_IV_WRONG_THREAD		(-7511)
#define	AE_IP_BAD_HANDLE			(-7512)
#define	AE_IP_NEED_MORE_IMGS		(-7513)
#define	AE_IP_NO_WORLD				(-7514)
#define	AE_IP_TYPE_MISMATCH			(-7515)
#define	AE_IP_MOVE_IN_PROGRESS		(-7516)
#define	AE_IP_SAME_SVR				(-7517)
#define	AE_IP_BAD_POS_NBR			(-7518)
#define	AE_IP_INCONSISTENT			(-7519)
#define	AE_IP_BAD_SPECS				(-7520)
#define	AE_IP_CAMERA				(-7521)
#define	AE_IP_PIPE_TIMEOUT			(-7522)


//	Pololu equipped bots.
//
#define	AE_POLOLU_BAD_IR_SPEC		(-8001)


//	Report native record.
//
#define	AE_RPT_UI_MUST_BE_OPEN		(-8501)


//	1999-Jun-22
//
//	Return values indicating error.  
//
//	If a function returns APP_ERR the caller should use GetDSErr() to get an
//	error message.
//
//	->	If CHECK_APP_ERR is returned the caller may * not * use GetDSErr() 
//		to get details because the value of iApp may be incorrect.
//
//	1999-Jul-05
//
//	Originally defined for ds_app_if.h, .cpp.  Now possibly used in other 
//	modules that bypass ds_app_if.
//
//	2004-Feb-14		GetDSErr() does not exist.
//
//	APP_ERR is used by the DS when it forms the error message it sends back
//	to the app.  Theh app does not need to call the DS again to get the
//	message.  See how functions in DSThdForm.cpp calls DS_GetError() and
//	DS_GetError() itself.
//
//	Furthermore, CHECK_APP_ERR is not used.
//
#define	APP_ERR			-1
/*
#define	CHECK_APP_ERR	-2		Not used, obsolete.
*/


PORT void	ResetE();
//
//		Clears all messages from the pool.


PORT int	LogErrors ( int OnOff );
//
//		Turns logging of errors on/off.  When off the error message is
//		not sent to diagnostics.
//
//		Logging is initially on.  The DS might turn logging off when it knows
//		many errors may occur.
//
//		OnOff		If non-zero then logging is turned on.  Otherwise, off.
//
//		Returns the state of logging before the function was called.


PORT uint	GetNErrs();
//
//		Returns the total number of errors that have occured in the session.
//		Only counts the number of times E() was called.  Does not count
//		T() or M() calls.


PORT void	StartBufferErrors ( char * Buffer, uint SzBuffer );
//
//	Appends any error or notice to the specified buffer until it is full
//	or StopBufferErrors() is called.
//
//	Only the error and notice messages by the same thread are appended
//	to the buffer.  Each thread may specify its own buffer.
//
//	The number of threads that can have a buffer active at the same time
//	is limited to some number.  If the limit is reached and this function
//	is called then the call is ignored - the buffer will remain empty.


PORT void	StopBufferErrors();
//
//	See StartBufferErrors().  The particular buffer is identified by the
//	calling thread.


PORT int	M ( int AE, const char * Func, const char * Fmt, ... );
//
//	Message.  For application errors.  For example, an app tries to make too 
//	many DRMs in a record.  Inserts the message into a slot of the pool.
//
//	The message is * not * logged to the diagnostics file.
//
//	Normally the text should probably not contain any function/file info.
//	This message is something that might be displayed to the user.
//
//	Returns a unique non-zero value used to retrieve the message later with 
//	GetE().
//
//	AE		One of the values of the AE_ macros with the exception that it
//			should * not * be AE_INT_ERROR.
//
//	11-Jan-1999
//
//	Message * is * logged.  Done to indicate the original cause of the error.
//
//	Also added Func parameter.  The function is written to the log but is not 
//	included in the pool buffer. I.e., it is not sent to the app/user.


PORT int	E ( const char * Func, const char * Fmt, ... );
//
//	Error.  Inserts the error message into a slot of the pool.
//
//	Assigns AE_INT_ERROR to the message type.
//
//	Returns a unique non-zero value used to retrieve the error message later
//	with GetE().


PORT int	T ( int ENo, const char * Func, const char * Fmt, ... );
//
//	08-Jan-1999		See other notes in Journal.doc.
//
//	Trace.  This only writes the message to the diagnostics log (if on).
//	Does not insert the message into the pool.
//	
//	ENo		The value returned by E() or M().  
//
//	Looks up error in pool identified by ENo.  Calls BCDEr() if not found.
//
//	Returns the same ENo value passed in.


PORT int	T ( int ENo, const char * Func );
//
//	2005-Jul-02
//
//	Trace.  Like the other T.  It is so common for a function to simply
//	state "Returning error." or "Throwing up." that this provides a more 
//	compact way of doing it.  The message is "(trace)".
//
//	Does not insert the message into the pool.
//	
//	ENo		The value returned by E() or M().  
//
//	Looks up error in pool identified by ENo.  Calls BCDEr() if not found.
//
//	Returns the same ENo value passed in.


PORT int	Notice ( const char * Func, const char * Fmt, ... );
//
//	2004-Jul-08		Generally, an error has occured - but we don't want
//					"Error" to be in the diagnostics.
//
//	The diagnostic message is preceded with "Notice" instead of "Error".
//
//	No ENo is generated.
//
//	Always returns 0.


PORT int	bcder ( const char * Func, const char * Fmt, ... );
//
//	Used to report/log errors where, while coding, the cause is more evident.
//	Buffer too small, invalid logic, etc..
//
//	Applies a special diagnostic type, "BCDError", to the message in the 
//	diagnostics log file.
//
//	Otherwise, works identically to E().


PORT int	syser ( const char * Func, const char * Fmt, ... );
//
//	Assumes an error result was returned from an OS function call.  Calls
//	GetLastError() and FormatMessage() to get more info.
//
//	Finally, calls and returns the value from E().


PORT int	sysnotice ( const char * Func, const char * Fmt, ... );
//
//	->	Returns 0.  * Does not return ENo. *
//
//	Assumes an error result was returned from an OS function call.  Calls
//	GetLastError() and FormatMessage() to get more info.
//
//	Unlike syser(), does not call E().  Instead of "Error" in the 
//	diagnositics, you get a "Notice".


PORT int	GetE ( int & ENo, int & AE, char * sE, uint SizeE );
//
//	Returns -1 if ENo, sE or SizeE is bad.
//
//	Returns the size of sE required if it is too small to retrieve
//	the specified error message.  Note that sE is still filled with
//	as much of the message that will fit.  sE will be 0 terminated.
//	The message will not be removed from the pool.
//
//	Returns 0 if ENo is valid and the message fits in sE.  The message
//	will be removed from the pool.
//
//	08-Jan-1999
//
//	Provides AE.  If AE is AE_INT_ERROR then the error message is probably
//	not meant for the application to see.  If AE is any other value then
//	hopefully the message is something user readable.
//
//	2011-Oct-18
//
//	Changing the ENo parameter to a reference.
//
//	If the passed ENo is bad then, on return, ENo will be that of a valid
//	error indicating the previous one was bad and -1 will be returned.
//
//	In general, this function deletes the error.
//
//	However, if < 0 is returned then ENo will be an error the caller must 
//	still deal with and to no error message will have been retrieved.  
//	This function should not be called again on that ENo.
//
//	As before, if > 0 is returned it is the size of buffer required to
//	hold the error string (the passed buffer will be filled with what fits).  
//	But, its pointless, since the error will have been deleted and the 
//	error string will no longer be retrievable.
//
//	If 0 is returned then all is groovy.  (The error message is retrieved
//	and the error is deleted.)


PORT int	GetAE ( int ENo );
//
//	Returns AE_ERROR if ENo is bad.
//
//	Otherwise, returns one of the other AE_ constants.  
//
//	The error is not removed from the pool.


PORT void	DelE ( int ENo );
//
//	Removes the specified message from the pool.


#endif

//	error.h
