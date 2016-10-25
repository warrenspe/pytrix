/*
 *  pytrix.c: A module used to construct and interact with matrices & vectors.
 *  Copyright (C) 2016 Warren Spencer warrenspencer27@gmail.com

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "headers/pytrix.h"

void initTypes(PyObject *module) {
/* API function which serializes a python object.
 *
 * Inputs: self - Unused. The required `self` parameter of any python-visible function.
 *         args - Positional arguments passed to the function.
 *
 * Outputs: A Python string containing the serialized value of the passed object, or NULL if an error occurs.
 */


    VectorType.tp_methods = VectorMethods;
    VectorType.tp_as_number = &VectorNumberMethods;
    VectorType.tp_richcompare = vectorRichCmp;
    MatrixType.tp_methods = MatrixMethods;

    if (PyType_Ready(&VectorType) < 0)
        return;
    if (PyType_Ready(&VectorIterType) < 0)
        return;
    if (PyType_Ready(&MatrixType) < 0)
        return;

    Py_INCREF(&VectorType);
    Py_INCREF(&VectorIterType);
    Py_INCREF(&MatrixType);
    PyModule_AddObject(module, "Vector", (PyObject *)&VectorType);
    PyModule_AddObject(module, "Matrix", (PyObject *)&MatrixType);
}


static PyMethodDef pytrixMethods[] = {
     {NULL, NULL, 0, NULL}   /* sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef pytrixDef = {
    PyModuleDef_HEAD_INIT,
    "pytrix",
    "Provides access to Vector & Matrix data types.",
    -1,
    pytrixMethods,
    NULL,
    NULL,
    NULL,
    NULL
};


PyMODINIT_FUNC PyInit_pytrix(void) {
    PyObject *module = PyModule_Create(&pytrixDef);
    initTypes(module);
    return module;
}

#else
PyMODINIT_FUNC initpytrix(void) {
    PyObject *module = Py_InitModule3("pytrix", pytrixMethods, "Provides access to Vector & Matrix data types.");
    initTypes(module);
}
#endif
