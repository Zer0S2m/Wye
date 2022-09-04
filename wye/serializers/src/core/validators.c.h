// Run validators
PyObject *RunValidators(PyObject *value, PyObject *validators);
PyObject *SplitKeysTreeRunValidatorDict(PyObject *keys_tree);

int *ValidationMaxLengthValue(PyObject *py_value, PyObject *max_length);
int *ValidationMinLengthValue(PyObject *py_value, PyObject *min_length);
int *CheckMaxMinLength(PyObject *rule, PyObject *py_value);


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
int *ValidationMaxLengthValue(PyObject *py_value, PyObject *max_length) {
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
int *ValidationMinLengthValue(PyObject *py_value, PyObject *min_length) {
    if (!PyObject_RichCompareBool(py_value, min_length, Py_GE))
        return (int *) 0;
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
        if (max_length != Py_None) {
            if (!ValidationMaxLengthValue(py_value, max_length))
                return SetMaxLengthError();
        }
        if (min_length != Py_None) {
            if (!ValidationMinLengthValue(py_value, min_length))
                return SetMinLengthError();
        }
    }

    return (int *) 1;
}
