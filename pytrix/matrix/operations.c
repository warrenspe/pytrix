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

PyObject *matrixStr(Matrix *self) {
/*  Constructs a human-readable Python String representing a human readable version of this matrix.

    Inputs: self - The matrix for which the human readable format will be produced.

    Outputs: A PyString containing the human readable form of the matrix.
*/

    unsigned int i;
    const char *cNewline = "\n\00";
    PyObject *matrixRepr,
             *newlineStr,
             *vectorReprs,
             *tuple,
             *list,
             *listRepr;

    // Create a list of all the vector reprs
    if ((vectorReprs = PyList_New(self->rows)) == NULL)
        return NULL;

    for (i = 0; i < self->rows; i++) {
        if ((tuple = _vectorToTuple(Matrix_GetVector(self, i))) == NULL) {
            Py_DECREF(vectorReprs);
            return NULL;
        }
        if ((list = PySequence_List(tuple)) == NULL) {
            Py_DECREF(vectorReprs);
            Py_DECREF(tuple);
            return NULL;
        }

        if ((listRepr = PyObject_Repr(list)) == NULL) {
            Py_DECREF(vectorReprs);
            Py_DECREF(tuple);
            Py_DECREF(list);
        }

        PyList_SET_ITEM(vectorReprs, i, listRepr);
        Py_DECREF(tuple);
        Py_DECREF(list);
    }

    // Create a newline-delimited version of all the vector reprs joined
    if ((newlineStr = PyBytes_FromString(cNewline)) == NULL || PyErr_Occurred() != NULL) {
        Py_DECREF(vectorReprs);
        return NULL;
    }

    if ((matrixRepr = PyObject_CallMethod(newlineStr, "join", "O", vectorReprs)) == NULL) {
        Py_DECREF(newlineStr);
        Py_DECREF(vectorReprs);
        return NULL;
    }

    Py_DECREF(newlineStr);
    Py_DECREF(vectorReprs);
    return matrixRepr; // TODO figure out refcounts, leave nothing alive
}


PyObject *matrixAdd(PyObject *a, PyObject *b) {
/*  Adds the components of two matrices together to create a new matrix.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly the first matrix to add.
            b - Supposedly the second matrix to add.

    Outputs: A new matrix constructed by performing a + b, or NULL if an exception occurred.
*/

    if (!(Matrix_Check(a) && Matrix_Check(b))) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return (PyObject *)_matrixAdd((Matrix *)a, (Matrix *)b);
}


PyObject *matrixSub(PyObject *a, PyObject *b) {
/*  Subtracts the components of two matrices to create a new matrix.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly the first matrix to subtract from.
            b - Supposedly the second matrix to be subtracted from a.

    Outputs: A new matrix constructed by performing a - b, or NULL if an exception occurred.
*/

    if (!(Matrix_Check(a) && Matrix_Check(b))) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return (PyObject *)_matrixSub((Matrix *)a, (Matrix *)b);
}


PyObject *matrixMul(PyObject *a, PyObject *b) {
/*  Multiplies a matrix by either a Vector, scalar or another matrix.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly either the vector/matrix/scalar to multiply the matrix b with, or a matrix to be multiplied by.
    Inputs: b - Supposedly either the matrix to be multiplied by or the vector/matrix/scalar to multiply the matrix a with.

    Outputs:
        * If one argument is a Matrix and the other a scalar, a Matrix constructed by performing component-wise
          multiplication with the scalar.
        * If one argument is a Matrix and the other a Vector, a Vector constructed by performing vector-matrix
          multiplication.
        * If both arguments are Matrices, a Matrix constructed by performing matrix multiplication.

        * NULL if none of the above stand true, or if an exception was encountered.
*/

    // If one of our operands are scalars, perform scalar multiplication
    if (PyNumber_Check(a))
        return (PyObject *)_scalarMatrixMul((Matrix *)b, PyNumber_AS_VECTOR_TYPE(a));
    if (PyNumber_Check(b))
        return (PyObject *)_scalarMatrixMul((Matrix *)a, PyNumber_AS_VECTOR_TYPE(b));

    // Otherwise, if one of our other operands is a Vector, perform vector multiplication
    if (Vector_Check(b))
        return (PyObject *)_vectorMatrixMul(a, b);

    // Otherwise if both our operands are matrices, perform matrix multiplication
    if (Matrix_Check(a) && Matrix_Check(b))
        return (PyObject *)_matrixMatrixMul((Matrix *)a, (Matrix *)b);

    // Otherwise, raise an exception. One of our operands will be a matrix, we are assured that by the fact that we
    // were called; figure out which one isn't for the error message.
    if (Matrix_Check(b)) {
        PyErr_Format(PyExc_TypeError, "Given object is not a Matrix/Vector/Scalar: \"%.400s\"", Py_TYPE(a)->tp_name);
    } else {
        PyErr_Format(PyExc_TypeError, "Given object is not a Matrix/Vector/Scalar: \"%.400s\"", Py_TYPE(b)->tp_name);
    }

    return NULL;
}


PyObject *matrixNeg(PyObject *a) {
/*  Negates the components in a matrix.

    Inputs: a - The matrix to negate.

    Outputs: A new Matrix object constructed from a, with the components negated.
*/

    return (PyObject *)_matrixNeg((Matrix *)a);
}


int matrixTrue(Matrix *a) {
/*  Determines if a matrix is the zero matrix or not.

    Inputs: a - The matrix to check.

    Outputs: A PyTrue if the matrix is not a zero matrix.  A PyFalse if it is.
*/

    unsigned int row,
                 col;

    for (row = 0; row < a->rows; row++) {
        for (col = 0; col < a->columns; col++) {
            if (Matrix_GetValue(a, row, col) != 0)
                return 1;
        }
    }

    return 0;
}


PyObject *matrixRichCmp(PyObject *a, PyObject *b, int op) {
/*  Performs (in)equality checking for matrices.
    Note that both arguments must be sanitized, as any python object may be passed.  We expect both to be matrices.

    Inputs: a  - The first matrix to compare.
            b  - The second matrix to compare.
            op - An operation code.  See https://docs.python.org/3/c-api/typeobj.html#c.PyTypeObject.tp_richcompare

    Outputs: A PyTrue or PyFalse depending which op code was given and the values of a and b.
*/

    if ((!_assertMatrix(a)) || (!_assertMatrix(b)))
        return NULL;

    switch (op) {
        case Py_EQ:
            if (_matricesEqual((Matrix *)a, (Matrix *)b))
                Py_RETURN_TRUE;
            Py_RETURN_FALSE;

        case Py_NE:
            if (_matricesEqual((Matrix *)a, (Matrix *)b))
                Py_RETURN_FALSE;
            Py_RETURN_TRUE;

        default:
            Py_INCREF(Py_NotImplemented);
            return Py_NotImplemented;
    }
}
