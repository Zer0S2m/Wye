/* Run validators */

PyObject *RunValidators(PyObject *value, PyObject *validators);
PyObject *SplitKeysTreeRunValidatorDict(PyObject *keys_tree);

int *ValidationMaxLengthValueNumber(PyObject *py_value, PyObject *max_length);
int *ValidationMinLengthValueNumber(PyObject *py_value, PyObject *min_length);
int *_CheckMaxMinLength(PyObject *max_length, PyObject *min_length, PyObject *py_value);
int *CheckMaxMinLength(PyObject *rule, PyObject *py_value);

int *_CheckOpidNumber(PyObject *opid_value, PyObject *py_value, int opid);
int *CheckOpidNumber(PyObject *rule, PyObject *py_value);


/**
 * @brief
 *
 * @param value in python -> Any
 * @param validators in python -> List[Callable[[Any], Any]]
 * @return PyObject*
 */
PyObject *RunValidators(PyObject *value, PyObject *validators) {
    PyObject *new_value = value;
    for (int i_validator = 0; i_validator < PyObject_Length(validators); i_validator++){
        PyObject *validator = PySequence_GetItem(validators, i_validator);
        new_value = PyObject_CallFunction(validator, "O", new_value);
    }

    return new_value;
}


/**
 * @brief
 *
 * @param keys_tree in python -> List[str]
 * @return PyObject*
 */
PyObject *SplitKeysTreeRunValidatorDict(PyObject *keys_tree) {
    PyObject *new_path = PyList_GetSlice(
        keys_tree,
        0,
        PyList_Size(keys_tree) - 1
    );
    new_path = PyUnicode_Join(PyUnicode_FromString("--"), new_path);
    return new_path;
}


/**
 * @brief
 *
 * @param py_value in python -> int
 * @param max_length in python -> int
 * @return int*
 */
int *ValidationMaxLengthValueNumber(PyObject *py_value, PyObject *max_length) {
    if (!PyObject_RichCompareBool(max_length, py_value, Py_GE))
        return (int *) 0;
    return (int *) 1;
}


/**
 * @brief
 *
 * @param py_value in python -> int
 * @param min_length in python -> int
 * @return int*
 */
int *ValidationMinLengthValueNumber(PyObject *py_value, PyObject *min_length) {
    if (!PyObject_RichCompareBool(py_value, min_length, Py_GE))
        return (int *) 0;
    return (int *) 1;
}


/**
 * @brief
 *
 * @param max_length in python -> int
 * @param min_length in python -> int
 * @param py_value in python -> int
 * @return int*
 */
int *_CheckMaxMinLength(PyObject *max_length, PyObject *min_length, PyObject *py_value) {
    if (max_length != Py_None) {
        if (!ValidationMaxLengthValueNumber(py_value, max_length))
            return SetMaxLengthError();
    }
    if (min_length != Py_None) {
        if (!ValidationMinLengthValueNumber(py_value, min_length))
            return SetMinLengthError();
    }

    return (int *) 1;
}


/**
 * @brief
 *
 * @param rule in python -> DIct[str, Any]
 * @param py_value in python -> int
 * @return int*
 */
int *CheckMaxMinLength(PyObject *rule, PyObject *py_value) {
    PyObject *max_length = PyDict_GetItemString(rule, MAX_LENGTH_FIELD_KEY);
    PyObject *min_length = PyDict_GetItemString(rule, MIN_LENGTH_FIELD_KEY);

    if (PyNumber_Check(py_value)) {
        if (!_CheckMaxMinLength(max_length, min_length, py_value))
            return (int *) 0;
    }
    if (PyUnicode_Check(py_value)) {
        PyObject *length_py_value = PyLong_FromLong((long) PyObject_Length(py_value));
        if (!_CheckMaxMinLength(max_length, min_length, length_py_value))
            return (int *) 0;
    }

    return (int *) 1;
}


/**
 * @brief
 *
 * @param rule in python -> Dict[str, Any]
 * @param py_value in python -> int
 * @param opid int
 * @return int*
 */
int *_CheckOpidNumber(PyObject *opid_value, PyObject *py_value, int opid) {
    if (!PyObject_RichCompareBool(py_value, opid_value, opid))
        return (int *) 0;
    return (int *) 1;
}


/**
 * @brief
 *
 * @param rule in python -> Dict[str, Any]
 * @param py_value in python -> int
 * @return int*
 */
int *CheckOpidNumber(PyObject *rule, PyObject *py_value) {
    PyObject *gt = PyDict_GetItemString(rule, GT_FIELD_KEY);
    PyObject *ge = PyDict_GetItemString(rule, GE_FIELD_KEY);
    PyObject *lt = PyDict_GetItemString(rule, LT_FIELD_KEY);
    PyObject *le = PyDict_GetItemString(rule, LE_FIELD_KEY);

    if (gt != Py_None) {
        if (!_CheckOpidNumber(gt, py_value, Py_GT))
            return SetGTError();
    }
    if (ge != Py_None) {
        if (!_CheckOpidNumber(ge, py_value, Py_GE))
            return SetGEError();
    }
    if (lt != Py_None) {
        if (!_CheckOpidNumber(lt, py_value, Py_LT))
            return SetLTError();
    }
    if (le != Py_None) {
        if (!_CheckOpidNumber(le, py_value, Py_LE))
            return SetLEError();
    }

    return (int *) 1;
}
