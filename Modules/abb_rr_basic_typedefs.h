#ifndef		__RS_BASIC_TYPEDEFS_H
#define		__RS_BASIC_TYPEDEFS_H



typedef	unsigned char		BYTE;

#ifdef	WIN32
    typedef	signed __int16		ssrt;

    typedef	unsigned __int16	usrt;

    typedef	signed __int32		sint;

    typedef unsigned __int32	uint;

    typedef	signed __int64		slng;

    typedef unsigned __int64	ulng;
#else
    #include	<sys/types.h>
    #ifdef	JS 
        #ifdef	JS_WIN32
            typedef  __int16			ssrt;

            typedef	unsigned __int16	usrt;

            typedef	signed __int32		sint;

            typedef unsigned __int32	uint;

            typedef	signed __int64		slng;

            typedef unsigned __int64	ulng;
        #else
            typedef	 int16_t			ssrt;

            typedef	u_int16_t			usrt;

            typedef	 int32_t			sint;

            typedef u_int32_t			uint;

            typedef	 int64_t			slng;

            typedef u_int64_t			ulng;

            typedef	u_int8_t			BYTE;
        #endif

        typedef long				LONG;

        typedef	uint				DWORD;

        #ifdef JS_WIN32
            typedef __int64				LONGLONG;
        #else
            typedef int64_t				LONGLONG;
        #endif

        typedef union _LARGE_INTEGER {
        struct {
            DWORD LowPart;
            LONG  HighPart;
        };
        struct {
            DWORD LowPart;
            LONG  HighPart;
        } u;
        LONGLONG QuadPart;
        } LARGE_INTEGER, *PLARGE_INTEGER;

    #else
        typedef	__int16_t			ssrt;

        typedef	__uint16_t			usrt;

        typedef	__int32_t			sint;

        typedef __uint32_t			uint;

        typedef	__int64_t			slng;

        typedef __uint64_t			ulng;

        typedef	uint				DWORD;

        typedef	__uint8_t			BYTE;
    #endif

    #define	PVOID				void *

    #define	LPCSTR              const char *

    typedef	int					BOOL;

    typedef	uint				HANDLE;

    typedef	uint				HWND;

    #define	ULONG				ulng

    #ifndef	JS_WIN32
        #define	_MAX_PATH			256
    #endif
    #define	_MAX_FNAME			256

    #define MAX_PATH            260

    #define	_snprintf			snprintf

    #define	_vsnprintf			vsnprintf

    #define	_stricmp			strcasecmp
#endif


#endif      //  __RS_BASIC_TYPEDEFS_H
