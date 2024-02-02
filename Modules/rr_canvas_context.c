
#include	<Python.h>


static PyObject *	fill_style ( PyObject * self, PyObject * args ) {
}	//	get_canvas_context()

static PyObject *	fill ( PyObject * self, PyObject * args ) {
}	//	get_canvas_context()

static PyMethodDef	rr_canvas_context_Methods[] = {
        
	{ "fill_style", fill_style, METH_VARARGS, "Set the style for following fill operations." },
	{ "fill",       fill,       METH_VARARGS, "Fill an area of the canvas." },
	{ NULL, NULL, 0, NULL } 
};

static struct PyModuleDef       rr_canvas_context_Module = {
	PyModuleDef_HEAD_INIT,
	"rr_canvas_context",
	NULL,
	-1,
	rr_canvas_context_Methods
};

PyMODINIT_FUNC	PyInit_rr_canvas_context ( void ) {
	return PyModule_Create ( &rr_canvas_context_Module );
}

