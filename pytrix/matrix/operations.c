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


PyObject *matrixItem(PyObject *self, Py_ssize_t i) {
/*  Returns the ith row of the matrix.

    Inputs: self - The Matrix which the row should be pulled from.
            i    - The row in the ith position of self will be returned.

    Outputs: A PyObject containing the vector in the ith slot of self.
*/

    Matrix *m = (Matrix *)self;
    Vector *v;
    unsigned int idx = (unsigned int)i;

    if (idx >= m->rows) {
        PyErr_Format(PyExc_IndexError, "Cannot return row number %x of Matrix with %x rows.", idx, m->rows);
        return NULL;
    }

    if ((v = _vectorCopy(Matrix_GetVector(m, idx))) == NULL)
        return NULL;

    return (PyObject *)v;
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


PyObject *matrixTranspose(PyObject *self) {
/*  Creates and returns a new matrix constructed by taking the transpose of this matrix.

    Inputs: self - The matrix being transposed.

    Outputs: A new Matrix Object constructed by taking the transpose of self.
*/

    return (PyObject *)_matrixTranspose((Matrix *)self);
}


PyObject *matrixRow(PyObject *self, PyObject *idx) {
/*  Returns the ith row from the Matrix self.

    Inputs: self - The matrix to return the ith row from.
            idx  - The row number to return from self.

    Outputs: A Vector object of the ith row in self if successful, NULL if an error occurs.
*/

    unsigned long i = PyNumber_AS_UNSIGNED_LONG(idx);
    Matrix *m = (Matrix *)self;
    Vector *v;

    if (PyErr_Occurred())
        return NULL;

    if (i >= m->rows) {
        PyErr_Format(PyExc_IndexError, "Cannot return row number %lu of Matrix with %x rows.", i, m->rows);
        return NULL;
    }

    if ((v = _vectorCopy(Matrix_GetVector(m, i))) == NULL)
        return NULL;

    return (PyObject *)v;
}


PyObject *matrixColumn(PyObject *self, PyObject *idx) {
/*  Returns the ith column from the Matrix self.

    Inputs: self - The matrix to return the ith column from.
            idx  - The column number to return from self.

    Outputs: A Vector object of the ith column in self if successful, NULL if an error occurs.
*/

    unsigned long col = PyNumber_AS_UNSIGNED_LONG(idx),
                  i;
    Matrix *m = (Matrix *)self;
    Vector *v;

    if (PyErr_Occurred())
        return NULL;

    if (col >= m->columns) {
        PyErr_Format(PyExc_IndexError, "Cannot return column number %lu of Matrix with %x columns.", col, m->columns);
        return NULL;
    }

    if ((v = _vectorNew(m->rows)) == NULL)
        return NULL;

    for (i = 0; i < v->dimensions; i++)
        Vector_SetValue(v, i, Matrix_GetValue(m, i, col));

    return (PyObject *)v;
}


PyObject *matrixPermute(PyObject *self, PyObject *args) {
/*  Returns a new matrix with the given rows permuted.

    Inputs: self - The matrix to permute.
            args - A tuple containing the indices of the two rows to permute.

    Outputs: A new copy of self with the given rows permuted.
*/

    int a,
        b;

    if (!PyArg_ParseTuple(args, "ii", &a, &b))
        return NULL;

    if (a < 0) {
        PyErr_Format(PyExc_ValueError, "Matrix.permute indices cannot be less than 0: %d", a);
        return NULL;
    }
    if (b < 0) {
        PyErr_Format(PyExc_ValueError, "Matrix.permute indices cannot be less than 0: %d", b);
        return NULL;
    }

    return (PyObject *)_matrixPermute((Matrix *)self, (unsigned int)a, (unsigned int)b);
}


PyObject *matrixGaussianElim(PyObject *self) {
/*  Uses Gaussian elimination to convert the given matrix into an upper triangular matrix.

    Inputs: self - The matrix to perform gaussian elimination.

    Outputs: A new Matrix containing the elimination of self.
*/

    Matrix *upperTriangular,
           *m = (Matrix *)self;

    if (m->columns != m->rows + 1) {
        PyErr_SetString(PyExc_ValueError, "Gaussian Elimination must be performed on a Matrix with columns = rows + 1.");
        return NULL;
    }

    if ((upperTriangular = _matrixNew(m->rows, m->columns)) == NULL)
        return NULL;

    if (!_matrixPALDU(NULL, m, NULL, NULL, upperTriangular, 1)) {
        Py_DECREF(upperTriangular);
        return NULL;
    }

    return (PyObject *)upperTriangular;
}


PyObject *matrixFactorLU(PyObject *self) {
/*  Factors a matrix A into a lower triangular form and an upper triangular form such that self = L*U

    Inputs: self - The matrix to factor.

    Outputs: A PyTuple containing (L, U).
*/

    PyObject *pyTuple;
    Matrix *l,
           *u,
           *m = (Matrix *)self;

    if ((l = _matrixNew(m->rows, m->rows)) == NULL)
        return NULL;
    if ((u = _matrixNew(m->rows, m->columns)) == NULL) {
        Py_DECREF(l);
        return NULL;
    }

    if (!_matrixPALDU(NULL, m, l, NULL, u, 0)) {
        Py_DECREF(l);
        Py_DECREF(u);
        return NULL;
    }

    if ((pyTuple = PyTuple_New(2)) == NULL) {
        Py_DECREF(l);
        Py_DECREF(u);
        return NULL;
    }
    PyTuple_SET_ITEM(pyTuple, 0, (PyObject *)l);
    PyTuple_SET_ITEM(pyTuple, 1, (PyObject *)u);

    return pyTuple;
}


PyObject *matrixFactorLDU(PyObject *self) {
/*  Factors a matrix A into a lower triangular form, diagonal form and an upper triangular form such that self = L*D*U

    Inputs: self - The matrix to factor.

    Outputs: A PyTuple containing (L, D, U).
*/

    PyObject *pyTuple;
    Matrix *l,
           *d,
           *u,
           *m = (Matrix *)self;

    if ((l = _matrixNew(m->rows, m->rows)) == NULL)
        return NULL;
    if ((d = _matrixNew(m->rows, m->rows)) == NULL) {
        Py_DECREF(l);
        return NULL;
    }
    if ((u = _matrixNew(m->rows, m->columns)) == NULL) {
        Py_DECREF(l);
        Py_DECREF(d);
        return NULL;
    }

    if (!_matrixPALDU(NULL, m, l, d, u, 0)) {
        Py_DECREF(l);
        Py_DECREF(d);
        Py_DECREF(u);
        return NULL;
    }

    if ((pyTuple = PyTuple_New(3)) == NULL) {
        Py_DECREF(l);
        Py_DECREF(d);
        Py_DECREF(u);
        return NULL;
    }
    PyTuple_SET_ITEM(pyTuple, 0, (PyObject *)l);
    PyTuple_SET_ITEM(pyTuple, 1, (PyObject *)d);
    PyTuple_SET_ITEM(pyTuple, 2, (PyObject *)u);

    return pyTuple;
}


PyObject *matrixFactorPLU(PyObject *self) {
/*  Factors a matrix A into a lower triangular form, upper triangular form and permutation matrix
    such that self = (P^-1) * A = L * U

    Inputs: self - The matrix to factor.

    Outputs: A PyTuple containing (P, L, U).
*/

    PyObject *pyTuple;
    Matrix *p,
           *l,
           *u,
           *m = (Matrix *)self;

    if ((l = _matrixNew(m->rows, m->rows)) == NULL)
        return NULL;
    if ((p = _matrixNew(m->rows, m->rows)) == NULL) {
        Py_DECREF(l);
        return NULL;
    }
    if ((u = _matrixNew(m->rows, m->columns)) == NULL) {
        Py_DECREF(l);
        Py_DECREF(p);
        return NULL;
    }

    if (!_matrixPALDU(p, m, l, NULL, u, 1)) {
        Py_DECREF(l);
        Py_DECREF(p);
        Py_DECREF(u);
        return NULL;
    }

    if ((pyTuple = PyTuple_New(3)) == NULL) {
        Py_DECREF(l);
        Py_DECREF(p);
        Py_DECREF(u);
        return NULL;
    }
    PyTuple_SET_ITEM(pyTuple, 0, (PyObject *)p);
    PyTuple_SET_ITEM(pyTuple, 1, (PyObject *)l);
    PyTuple_SET_ITEM(pyTuple, 2, (PyObject *)u);

    return pyTuple;
}


PyObject *matrixIsSymmetrical(PyObject *self) {
/*  Determines whether or not self is symmetrical.

    Inputs: self - The Matrix to determine whether or not is symmetrical.

    Outputs: A PyTrue or PyFalse depending on whether or not self is symmetrical.
*/

    if (_matrixSymmetrical((Matrix *)self))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}


PyObject *matrixIsIdentity(PyObject *self) {
/*  Determine whether or not self is an identity matrix.

    Inputs: self - The matrix to determine whether or not it is an identity matrix.

    Outputs: A Python boolean depicting whether or not self is an identity matrix.
*/

    Matrix *m = (Matrix *)self;

    unsigned int row,
                 col;

    if (m->rows != m->columns)
        Py_RETURN_FALSE;

    for (row = 0; row < m->rows; row++) {
        for (col = 0; col < m->columns; col++) {
            if (row != col && Matrix_GetValue(m, row, col) != 0)
                Py_RETURN_FALSE;
        }
    }

    Py_RETURN_TRUE;
}


PyObject *matrixIsInvertible(PyObject *self) {


    // call PALDU passing just U, check if all diagonal are non-zero
}


