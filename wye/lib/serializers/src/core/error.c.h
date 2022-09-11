/* Set error */

int *SetValidationError();
int *SetAttributeError();
int *SetValidationDefaultError();
int *SetMaxLengthError();
int *SetMinLengthError();
int *SetGTError();
int *SetGEError();
int *SetLTError();
int *SetLEError();
int *SetErrorFillType();


/**
 * @brief Set the Validation Error object
 *
 * @return int*
 */
int *SetValidationError() {
    PyErr_SetString(PyExc_TypeError, "<WyeSerializers>: Validation error type");
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


/**
 * @brief Set the Max Length Error object
 *
 * @return int*
 */
int *SetMaxLengthError() {
    PyErr_SetString(PyExc_ValueError, "<WyeSerializers>: Value is greater than allowed");
    return (int *) 0;
};


/**
 * @brief Set the Min Length Error object
 *
 * @return int*
 */
int *SetMinLengthError() {
    PyErr_SetString(PyExc_ValueError, "<WyeSerializers>: Value is less than allowed");
    return (int *) 0;
};


/**
 * @brief
 *
 * @return int*
 */
int *SetGTError() {
    PyErr_SetString(PyExc_ValueError, "<WyeSerializers>: Value is less than allowed");
    return (int *) 0;
}


/**
 * @brief
 *
 * @return int*
 */
int *SetGEError() {
    PyErr_SetString(PyExc_ValueError, "<WyeSerializers>: Value is greater than allowed");
    return (int *) 0;
}


/**
 * @brief
 *
 * @return int*
 */
int *SetLTError() {
    PyErr_SetString(PyExc_ValueError, "<WyeSerializers>: Value is greater than allowed");
    return (int *) 0;
}


/**
 * @brief
 *
 * @return int*
 */
int *SetLEError() {
    PyErr_SetString(PyExc_ValueError, "<WyeSerializers>: Value is less than allowed");
    return (int *) 0;
}


/**
 * @brief Set the Error Fill Type object
 *
 * @return int*
 */
int *SetErrorFillType() {
    PyErr_SetString(PyExc_ValueError, "<WyeSerializers>: Invalid fill type");
    return (int *) 0;
}
