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
        unsigned int rows;
        unsigned int columns;
        Vector **data;

    } Matrix;

    typedef struct {
        PyObject_HEAD
        unsigned int i;
        PyObject *iterating;

    } MatrixIter;


    // Macro & Compatibility Includes
    #include "headers/py2_3compat.h"
    #include "headers/macros.h"

    // Function Definition Includes
    #include "headers/vector_functions.h"
    #include "headers/vector_iter_functions.h"
    #include "headers/matrix_functions.h"
    #include "headers/matrix_iter_functions.h"

    // Type Includes
    #include "headers/vector_iter.h"
    #include "headers/matrix_iter.h"
    #include "headers/vector.h"
    #include "headers/matrix.h"

    // Vector Function Includes
    #include "vector/init.c"
    #include "vector/iter.c"
    #include "vector/utils.c"
    #include "vector/operations.c"

    // Matrix Function Includes
    #include "matrix/init.c"
    #include "matrix/iter.c"
    #include "matrix/utils.c"
    #include "matrix/operations.c"

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
