// Run validators
PyObject *RunValidators(PyObject *value, PyObject *validators);
PyObject *SplitKeysTreeRunValidatorDict(PyObject *keys_tree);


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
