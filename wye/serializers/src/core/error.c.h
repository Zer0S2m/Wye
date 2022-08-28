// Set error
int *SetValidationError();
int *SetAttributeError();
int *SetValidationDefaultError();


/**
 * @brief Set the Validation Error object
 *
 * @return int*
 */
int *SetValidationError() {
    PyErr_SetString(PyExc_TypeError, "<WyeSerializers>: Validation error");
    return (int *) 0;
}


/**
 * @brief Set the Attribute Error object
 *
 * @return int*
 */
int *SetAttributeError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Missing required parameters");
    return (int *) 0;
}


/**
 * @brief Set the Validation Default Error object
 *
 * @return int*
 */
int *SetValidationDefaultError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Validation error: default value");
    return (int *) 0;
}