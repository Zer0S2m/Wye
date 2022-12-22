PyObject *ParserLine(PyObject *, PyObject *);
void DeleteUnusedSpace(PyObject *);


void DeleteUnusedSpace(PyObject *line) {

}


PyObject *ParseLine(PyObject *line, PyObject *re_module) {
    PyObject *objects = PyObject_CallMethod(re_module, "findall", "OO", REGEX_VAR, line);
    return objects;
}


/**
 * @brief
 *
 * @param lines in python -> List[str]
 * @return PyObject*
 */
PyObject *RunParse(PyObject *lines) {
    PyObject *ready_lines = PyList_New(0);
    PyObject *__re_module = PyImport_ImportModule("re");

    for (Py_ssize_t i_line = 0; i_line < PyList_Size(lines); i_line++) {
        PyObject *ready_line = ParseLine(
            PyList_GetItem(lines, i_line),
            __re_module
        );
        PyList_Append(ready_lines, ready_line);
    }

    return ready_lines;
}
