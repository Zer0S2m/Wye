// Run validators
PyObject *RunValidators(PyObject *value, PyObject *validators);


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
