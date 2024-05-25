

//	Panel custom object.
//
typedef struct {
	PyObject_HEAD

//	int			frameId;
//	int			paneId;
	PyObject *	pane;			//	Pane name, e.g., "Arm Controls"
	PyObject *	panel;			//	Panel name, e.g., "pnlLeftArm"
	PyObject *	eleId;

}	Panel;

void
Panel_dealloc ( Panel * self );

PyObject *
Panel_new ( PyTypeObject * type, PyObject * args, PyObject * kwds );

int
Panel_init ( Panel * self, PyObject * args, PyObject * kwds );

PyObject *	
setPanelEleId ( PyObject * self, PyObject * args );

PyObject *	
Panel_size ( Panel * self, PyObject * args );

PyObject *	
Panel_load ( Panel * self, PyObject * args, PyObject * kwds );

