struct _VAR_NODE {
    PyObject *var_name;
    PyObject *context;

    PyObject *start_var;
    PyObject *end_var;
} VAR_NODE;
