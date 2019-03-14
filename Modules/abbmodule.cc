/*
		 1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

*/

#include    <Python.h>

#include	"abbmodule.h"

#include	"abb_rr_main.h"

#include	"abb_rr_world.h"


static PyObject *	SpamError;


static ABBCallback * abbCallback = NULL;

static void
ABBCallback_dealloc ( ABBCallback * self )
{
//	printf ( "ABBCallback_dealloc() self 0x%08X\n", (unsigned int)self );
	PyObject_Del ( self );
}

static PyTypeObject ABBCallback_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)

//	Members initialized in PyInit_abb().
//	.tp_name 		= "ABBCallback",
//	.tp_basicsize 	= sizeof(ABBCallback),
//	.tp_dealloc 	= (destructor)ABBCallback_dealloc,
//	.tp_flags 		= Py_TPFLAGS_DEFAULT
};



static int	CBFnc ( PECB * pCB )
{
	char msg [2048];		int i, nC, iC = 0;

	if ( (! abbCallback) || (! abbCallback->callback) ) {
		return -1; }

	printf ( "CBFnc()\n" );
	
	if ( pCB->Cmd == PECB_SYNC_VPS3 ) {

		RbtMovCBData * pCBD = (RbtMovCBData *)pCB->p;

		nC = snprintf ( &msg[iC], sizeof(msg) - iC - 1, 
						"{\"cmd\": \"PECB_SYNC_VPS3\", \"ents\": [" );
		if ( nC < 0 ) {
			printf ( "CBFnc() msg[] is too small" );
			return -1; }
		iC += nC;

		int nEnts = pCBD->pWEPV ? pCBD->pWEPV->nEnts : 0;

		for ( i = 0; i < nEnts; i++ ) {
			WorldEntPaul * pEnt = &pCBD->pWEPV->pEnts[i];
			
			nC = snprintf ( &msg[iC], sizeof(msg) - iC - 1, 
							"{\"EId\": %d, "
							"\"Theta\": %f, "
							"\"Dist\": %f, "
							"\"Length\": %f, "
							"\"Alpha\": %f},",
							pEnt->EId, pEnt->Theta, 
									   pEnt->Dist, 
									   pEnt->Length, 
									   pEnt->Alpha );
			if ( nC < 0 ) {
				printf ( "CBFnc() msg[] is too small" );
				return -1; }
			iC += nC;
		}

		if ( nEnts > 0 ) {
			iC -= 1; }		//	to overwrite the last comma
		nC = snprintf ( &msg[iC], sizeof(msg) - iC - 1, "]}" );
		if ( nC < 0 ) {
			printf ( "CBFnc() msg[] is too small" );
			return -1; }
		iC += nC;

		abbCallback->callback ( msg );
	}

	return 0;

}	//	CBFnc()


static PyObject *
spam_system ( PyObject * self, PyObject * args )
{
	const char *command;
	int sts = 0;

	//  On errors - see -
	//      https://docs.python.org/3/extending/extending.html#intermezzo-errors-and-exceptions
	//  The Python C API function will have probably set the exception. The
	//  client (us) should just return NULL and let what called us do what
	//  ever.
	if ( ! PyArg_ParseTuple ( args, "s", &command ) )
		return NULL;
//	sts = system ( command );
//	if ( sts < 0 ) {
//		PyErr_SetString ( SpamError, "System command failed" );
//		return NULL;
//	}

	RR_Test ( "Hello?", 42 );

	IdRbt ( 0, "Al" );

	PECB	CB ( 1, CBFnc,                 0, 0, 0 );
//	PECB	CB ( 1, abbCallback->callback, 0, 0, 0 );
//	PECB * pCB = newPECB ( 1, CBFnc, 0, 0, 0, 0 );

	ConfigureLink ( &CB, 0,  0, 1, 1,   0.0,   5.0,   0.0,  90.0 );
	ConfigureLink ( &CB, 1,  0, 1, 0,  90.0,   0.0,   0.0, -90.0 );
	ConfigureLink ( &CB, 2,  0, 1, 1,  90.0,   0.0,   8.0,   0.0 );
	ConfigureLink ( &CB, 3,  0, 1, 1,  90.0,   0.0,   0.0,  90.0 );
	ConfigureLink ( &CB, 4,  0, 1, 1,   0.0,   8.0,   0.0,  90.0 );
	ConfigureLink ( &CB, 5,  0, 1, 1,   0.0,   0.0,   0.0, -90.0 );
	ConfigureLink ( &CB, 6,  0, 1, 1,   0.0,   0.0,   0.0,   0.0 );

//	delete pCB;

	return PyLong_FromLong ( sts );
}


PyDoc_STRVAR(abb_new_callback_doc,
"new_callback()\n\
\n\
Create a new callback.");

static PyObject * 	abb_new_callback ( PyObject * self, PyObject * args )
{
	//	Expect no arguments.

	abbCallback = (ABBCallback *)PyObject_New ( ABBCallback, &ABBCallback_Type );

	if ( abbCallback == NULL )
		return NULL;

	abbCallback->callback = NULL;

	Py_INCREF(abbCallback);
	return (PyObject *)abbCallback;

}	//	abb_new_callback()


static PyObject *
abb_configure_link ( PyObject * self, PyObject * args )
{
	int			iJ;
	int			EId;
	int 		bRot;
	int			bKin;
	double		Theta;
	double		Dist;
	double		Length;
	double		Alpha;

	//  On errors - see -
	//      https://docs.python.org/3/extending/extending.html#intermezzo-errors-and-exceptions
	//  The Python C API function will have probably set the exception. The
	//  client (us) should just return NULL and let what called us do what
	//  ever.
	//
	//	On parsing arguments -
	//		https://docs.python.org/3/c-api/arg.html?highlight=pyarg_parsetuple#parsing-arguments
	//		https://docs.python.org/3/c-api/arg.html?highlight=pyarg_parsetuple#c.PyArg_ParseTuple
	//
	if ( ! PyArg_ParseTuple ( args, "iiiidddd:configure_link", 
							  &iJ, &EId, &bRot, &bKin, 
							  &Theta, &Dist, &Length, &Alpha ) ) {
		return NULL; }

	PECB	CB ( 1, CBFnc, 0, 0, 0 );

	ConfigureLink ( &CB, iJ, EId, bRot, bKin, Theta, Dist, Length, Alpha );

	return PyLong_FromLong ( 0 );

}	//	abb_configure_link()


static PyMethodDef SpamMethods[] = {
//	...
	{"system",  spam_system, METH_VARARGS,
		"Execute a shell command."},
		
	{ "new_callback", abb_new_callback, METH_VARARGS, abb_new_callback_doc },
		
	{ "configure_link", abb_configure_link, METH_VARARGS,
		"Define the kinematics of a robot arm link."},
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


	ABBCallback_Type.tp_name 		= "ABBCallback";
	ABBCallback_Type.tp_basicsize 	= sizeof(ABBCallback);
	ABBCallback_Type.tp_dealloc 	= (destructor)ABBCallback_dealloc;
	ABBCallback_Type.tp_flags 		= Py_TPFLAGS_DEFAULT;

	if ( PyType_Ready ( &ABBCallback_Type ) < 0 )
		goto fail;
	Py_INCREF ( &ABBCallback_Type );
	PyModule_AddObject ( m, "abbcallback_type", (PyObject *)&ABBCallback_Type );

	return m;

 fail:
	Py_XDECREF(m);
	return NULL;

}	//	PyInit_abb()


