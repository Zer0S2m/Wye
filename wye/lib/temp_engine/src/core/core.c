#include <core/core.h>

#include "parser.c.h"
#include "node.c.h"


/**
 * @brief Get the Filename Template object
 *
 * @param path_temp in python -> str
 * @return const char*
 */
const char *GetFilenameTemplate(PyObject *path_temp) {
    return PyBytes_AS_STRING(
        PyUnicode_AsEncodedString(PyObject_Repr(path_temp), "utf-8", "~E~")
    );
}


/**
 * @brief
 *
 * @param self
 * @param args in python -> kwargs (Dict[str, Any])
 * @return PyObject*
 */
static PyObject *method_build_template(PyObject *self, PyObject *args) {
    PyObject *path_temp, *context;
    if (!PyArg_ParseTuple(args, "OO", &path_temp, &context))
        return NULL;

    PyGILState_STATE gil_state = PyGILState_Ensure();

    PyObject *io_module = PyImport_ImportModule("io");
    PyObject *opened_file = PyObject_CallMethod(io_module, "open", "Os", path_temp, "r");

    Py_DECREF(io_module);

    PyObject *lines_from_file = PyObject_CallMethod(opened_file, "readlines", NULL);

    PyObject_CallMethod(opened_file, "close", NULL);
    PyGILState_Release(gil_state);

    PyObject *ready_lines = RunParse(lines_from_file);

    return ready_lines;
}


static PyMethodDef WyeTempEngineMethods[] = {
    {
        "build_template",
        method_build_template,
        METH_VARARGS,
        "Build template"
    },
    { NULL, NULL, 0, NULL }
};


static struct PyModuleDef wye_temp_engine_modules = {
    PyModuleDef_HEAD_INIT,
    "wye_temp_engine",
    "Python template engine interface for Wye",
    -1,
    WyeTempEngineMethods
};


PyMODINIT_FUNC PyInit_wye_temp_engine(void) {
    PyObject *module = PyModule_Create(&wye_temp_engine_modules);
    return module;
}
