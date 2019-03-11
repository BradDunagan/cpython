/*
		 1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

*/

#include    <Python.h>

#include	"abb_rr_main.h"



static PyObject *	SpamError;


static int	CBFnc ( PECB * pCB )
{
	printf ( "CBFnc()\n" );
	
	return 0;

}	//	CBFnc()


static PyObject *
spam_system ( PyObject * self, PyObject * args )
{
	const char *command;
	int sts;

	//  On errors - see -
	//      https://docs.python.org/3/extending/extending.html#intermezzo-errors-and-exceptions
	//  The Python C API function will have probably set the exception. The
	//  client (us) should just return NULL and let what called us do what
	//  ever.
	if ( ! PyArg_ParseTuple ( args, "s", &command ) )
		return NULL;
	sts = system ( command );
	if ( sts < 0 ) {
		PyErr_SetString ( SpamError, "System command failed" );
		return NULL;
	}

	RR_Test ( "Hello?", 42 );

	IdRbt ( 0, "Al" );

	PECB	CB ( 1, CBFnc, 0, 0, 0 );

	ConfigureLink ( &CB, 0,  0, 1, 1,   0.0,   5.0,   0.0,  90.0 );
	ConfigureLink ( &CB, 1,  0, 1, 0,  90.0,   0.0,   0.0, -90.0 );
	ConfigureLink ( &CB, 2,  0, 1, 1,  90.0,   0.0,   8.0,   0.0 );
	ConfigureLink ( &CB, 3,  0, 1, 1,  90.0,   0.0,   0.0,  90.0 );
	ConfigureLink ( &CB, 4,  0, 1, 1,   0.0,   8.0,   0.0,  90.0 );
	ConfigureLink ( &CB, 5,  0, 1, 1,   0.0,   0.0,   0.0, -90.0 );
	ConfigureLink ( &CB, 6,  0, 1, 1,   0.0,   0.0,   0.0,   0.0 );

	return PyLong_FromLong ( sts );
}


static PyMethodDef SpamMethods[] = {
//	...
	{"system",  spam_system, METH_VARARGS,
		"Execute a shell command."},
//	...
	{NULL, NULL, 0, NULL}        /* Sentinel */
};


static struct PyModuleDef spammodule = {
	PyModuleDef_HEAD_INIT,
	"abb",   /* name of module */
	NULL,     // spam_doc, /* module documentation, may be NULL */
	-1,       /* size of per-interpreter state of the module,
					or -1 if the module keeps state in global variables. */
	SpamMethods
};



extern "C"
{
	PyMODINIT_FUNC	PyInit_abb ( void );
}

PyMODINIT_FUNC
PyInit_abb ( void )
{
	PyObject * m;

	m = PyModule_Create ( &spammodule );
	if ( m == NULL )
		return NULL;

	SpamError = PyErr_NewException ( "spam.error", NULL, NULL );
	Py_INCREF ( SpamError );
	PyModule_AddObject ( m, "error", SpamError );
	return m;
}


