
typedef	PyObject *	(* UICBToAppFunc) ( int flags, const char * fmt, ... );


void
UIErr ( const char * fmt, ... );

PyObject *
AppCallPart2 ( const char * fnc, PyObject * response );

