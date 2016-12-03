/*
 *  pytrix.c: A module used to construct and interact with matrices vectors, & points.
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
/* Initialize the Vector, Matrix & VectorIter types.
 *
 * Inputs: module - The module object we will attach the types to.
 */

    if (PyType_Ready(&PointType) < 0)
        return;
    if (PyType_Ready(&PointIterType) < 0)
        return;
    if (PyType_Ready(&VectorType) < 0)
        return;
    if (PyType_Ready(&VectorIterType) < 0)
        return;
    if (PyType_Ready(&MatrixType) < 0)
        return;
    if (PyType_Ready(&MatrixIterType) < 0)
        return;

    Py_INCREF(&PointType);
    Py_INCREF(&PointIterType);
    Py_INCREF(&VectorType);
    Py_INCREF(&VectorIterType);
    Py_INCREF(&MatrixType);
    Py_INCREF(&MatrixIterType);
    PyModule_AddObject(module, "Point", (PyObject *)&PointType);
    PyModule_AddObject(module, "Vector", (PyObject *)&VectorType);
    PyModule_AddObject(module, "Matrix", (PyObject *)&MatrixType);
}


static PyMethodDef pytrixMethods[] = {
    {"identityMatrix", (PyCFunction)identityMatrix, METH_VARARGS,
        PyDoc_STR("Convenience function for constructing identity matrices.")},
    {"rotation2DMatrix", (PyCFunction)rotation2DMatrix, METH_VARARGS,
        PyDoc_STR("Convenience function for constructing 2D rotation matrices.")},
    {"rotation3DMatrix", (PyCFunction)rotation3DMatrix, METH_VARARGS,
        PyDoc_STR("Convenience function for constructing 3D rotation matrices.")},
    {"permutationMatrix", (PyCFunction)permutationMatrix, METH_VARARGS,
        PyDoc_STR("Convenience function for constructing permutation matrices.")},
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
