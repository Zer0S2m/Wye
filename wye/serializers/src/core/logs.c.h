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

    time_t _time;
    struct tm *currtime;
    _time = time(NULL);
    currtime = localtime(&_time);

    printf(
        "%sLOG %d:%d:%d - %s%s\n",
        GREEN_COLOR,
        currtime->tm_hour,
        currtime->tm_min,
        currtime->tm_sec,
        bytes,
        RESET_COLOR
    );
}