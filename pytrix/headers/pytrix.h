/*
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

#ifndef __PYTRIX_H
#define __PYTRIX_H 1

    #define VECTOR_TYPE double

    // Standard Library Includes
    #include <Python.h>
    #include "structmember.h"
    #include <math.h>
    #include <string.h>
    #include <stdlib.h>

    // Struct Definitions
    typedef struct {
        PyObject_HEAD
        unsigned int dimensions;
        VECTOR_TYPE *data;

    } Vector;

    typedef struct {
        PyObject_HEAD
        unsigned int i;
        PyObject *iterating;

    } VectorIter;

    typedef struct {
        PyObject_HEAD
        unsigned int height;
        unsigned int width;
        VECTOR_TYPE *data;

    } Matrix;


    // Macro & Compatibility Includes
    #include "headers/py2_3compat.h"
    #include "headers/macros.h"

    // Type Includes
    #include "headers/vector_iter.h"
    #include "headers/vector.h"
    #include "headers/matrix.h"

    // Vector Function Includes
    #include "vector/utils.c"
    #include "vector/operations.c"

    // Matrix Function Includes


    // Vector Functions
    #if PY_MAJOR_VERSION >= 3
    static PyNumberMethods VectorNumberMethods = {
        (binaryfunc)vectorAdd,
        (binaryfunc)vectorSub,
        (binaryfunc)vectorMul,
        0,
        0,
        0,
        (unaryfunc)vectorNeg,
        0,
        0,
        (inquiry)vectorTrue,
    };
    #else
    static PyNumberMethods VectorNumberMethods = {
        (binaryfunc)vectorAdd,
        (binaryfunc)vectorSub,
        (binaryfunc)vectorMul,
        (binaryfunc)vectorDiv,
        0,
        0,
        0,
        (unaryfunc)vectorNeg,
        0,
        0,
        (inquiry)vectorTrue,
    };
    #endif

    static PyMethodDef VectorMethods[] = {
        {"copy", (PyCFunction)vectorCopy, METH_NOARGS,
            PyDoc_STR("Creates a copy of this vector.")},
        {"dot", (PyCFunction)vectorDot, METH_O,
            PyDoc_STR("Performs a dot product against another vector.")},
        {"cross", (PyCFunction)vectorCross, METH_O,
            PyDoc_STR("Performs a cross product against another vector.")},
        {"length", (PyCFunction)vectorLength, METH_NOARGS,
            PyDoc_STR("Calculates the length of the vector.")},
        {"unit", (PyCFunction)vectorUnit, METH_NOARGS,
            PyDoc_STR("Returns a new vector of length 1 going in the same direction as this vector.")},
        {"isUnit", (PyCFunction)vectorIsUnit, METH_NOARGS,
            PyDoc_STR("Determine if this vector is a unit vector.")},
        {"angleBetween", (PyCFunction)vectorAngleBetween, METH_O,
            PyDoc_STR("Calculate the angle between this vector and another vector.")},
        {"orthogonal", (PyCFunction)vectorOrthogonal, METH_O,
            PyDoc_STR("Determines whether or not another vector is orthogonal to this vector.")},
        {NULL}  // Sentinel
    };

    // Matrix Functions
    static PyMethodDef MatrixMethods[] = {
        {NULL}  /* Sentinel */
    };


    // Declarations for DLL import/export
    #ifndef PyMODINIT_FUNC
        #define PyMODINIT_FUNC void
    #endif

    // pytrix.c Function Prototypes
    #if PY_MAJOR_VERSION >= 3
    PyMODINIT_FUNC PyInit_pytrix(void);
    #else
    PyMODINIT_FUNC initpytrix(void);
    #endif

    void initTypes(PyObject *);

#endif
