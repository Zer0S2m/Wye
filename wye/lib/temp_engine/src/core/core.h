#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define START_VAR PyUnicode_FromString("{{")
#define END_VAR PyUnicode_FromString("}}")
#define REGEX_VAR PyUnicode_FromString("{{\\s{0,1000}[\\w.]{1,1000}\\s{0,1000}}}")

const char *GetFilenameTemplate(PyObject *);

static PyObject *method_build_template(PyObject *, PyObject *);
