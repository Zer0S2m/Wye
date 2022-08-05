int SetValidationError();
int SetAttributeError();
void *SetDefaultValue(PyObject *json, PyObject *rules);
int CheckField(PyObject *json_field, PyObject *type, PyObject *is_required);
static PyObject *method_build_json(PyObject *self, PyObject *args);

#define TYPE_FIELD_KEY "TYPE"
#define ALIAS_FIELD_KEY "ALIAS"
#define DEFAULT_FIELD_KEY "DEFAULT"
#define REQUIRED_FIELD_KEY "REQUIRED"
