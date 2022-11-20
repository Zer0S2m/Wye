/* Logs */

void DisplayLog(PyObject *, char *);


/**
 * @brief
 *
 * @param py_value in python -> Any
 * @param index_color
 */
void DisplayLog(PyObject *py_value, char *index_color) {
    PyObject *repr = PyObject_Repr(py_value);
    PyObject *str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
    const char *bytes = PyBytes_AS_STRING(str);

    time_t _time;
    struct tm *currtime;
    _time = time(NULL);
    currtime = localtime(&_time);

    char *color;
    char *green_color = "g";
    char *red_color = "r";
    if (index_color == red_color)
        color = RED_COLOR;
    else if (index_color == green_color)
        color = GREEN_COLOR;
    else
        color = RESET_COLOR;

    printf(
        "%sLOG %d:%d:%d - %s%s\n",
        color,
        currtime->tm_hour,
        currtime->tm_min,
        currtime->tm_sec,
        bytes,
        RESET_COLOR
    );
}
