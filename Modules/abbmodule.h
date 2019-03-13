
typedef	int (* PECBToAppFunc) ( const char * msg );


typedef struct {
	PyObject_HEAD
	PECBToAppFunc	callback;
}	ABBCallback;

