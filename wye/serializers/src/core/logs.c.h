// Logs
void DisplayLog(PyObject *py_value);


/**
 * @brief
 *
 * @param py_value in python -> Any
 */
void DisplayLog(PyObject *py_value) {
    PyObject* repr = PyObject_Repr(py_value);
    PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
    const char *bytes = PyBytes_AS_STRING(str);

    printf("%sLOG: %s%s\n", GREEN_COLOR, bytes, RESET_COLOR);
}