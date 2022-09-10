/* Utils for build json from objects */

PyObject *GetFieldByPath(PyObject *object, PyObject *key_tree);
void BuildOneFieldInJson(PyObject *json, PyObject *key_tree, PyObject *value);
void ConvertObjectToJson(PyObject *json, PyObject *object, PyObject *keys_tree);


/**
 * @brief Get the field by path object
 *
 * @param object in python -> object
 * @param key_tree in python -> List[str]
 * @return PyObject*
 */
PyObject *GetFieldByPath(PyObject *object, PyObject *key_tree) {
    PyObject *part = object;

    for (int i_key_tree = 0; i_key_tree < PyList_Size(key_tree); i_key_tree++) {
        PyObject *param = PyList_GetItem(key_tree, i_key_tree);
        if (!part)
            return NULL;
        part = PyObject_GetAttr(part, param);
    }

    return part;
}


/**
 * @brief
 *
 * @param json in python -> Dict[str, Any]
 * @param key_tree in python -> List[str]
 * @param value in python -> Any
 */
void BuildOneFieldInJson(PyObject *json, PyObject *key_tree, PyObject *value) {
    Py_ssize_t length_key_tree = PyList_Size(key_tree);
    for (int i_key_tree = 0; i_key_tree < length_key_tree; i_key_tree++) {
        PyObject *param = PyList_GetItem(key_tree, i_key_tree);

        if (length_key_tree - 1 == i_key_tree) {
            PyDict_SetItem(json, param, value);
        }
        else {
            PyObject *part_raw_json = PyDict_GetItem(json, param);
            if (!PyDict_Check(part_raw_json))
                PyDict_SetItem(json, param, PyDict_New());

            json = PyDict_GetItem(json, param);
        }
    }
}


/**
 * @brief
 *
 * @param json in python -> Dict
 * @param object in python -> object
 * @param keys_tree in python List[List[str]]
 */
void ConvertObjectToJson(PyObject *json, PyObject *object, PyObject *keys_tree) {
    for (int i_key_tree = 0; i_key_tree < PyList_Size(keys_tree); i_key_tree++) {
        PyObject *key_tree = PyList_GetItem(keys_tree, i_key_tree);
        PyObject *part_json_object = GetFieldByPath(object, key_tree);

        if (!part_json_object)
            continue;

        BuildOneFieldInJson(json, key_tree, part_json_object);
    }
}
