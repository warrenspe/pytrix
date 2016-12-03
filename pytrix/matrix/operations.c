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
        PyErr_Format(PyExc_IndexError, "Cannot return row number %u of Matrix with %u rows.", idx, m->rows);
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
        return (PyObject *)_matrixMatrixMul((Matrix *)a, (Matrix *)b, 1);

    // Otherwise, raise an exception. One of our operands will be a matrix, we are assured that by the fact that we
    // were called; figure out which one isn't for the error message.
    if (Matrix_Check(b)) {
        PyErr_Format(PyExc_TypeError, "Given object is not a Matrix/Vector/Scalar: \"%.400s\"", Py_TYPE(a)->tp_name);
    } else {
        PyErr_Format(PyExc_TypeError, "Given object is not a Matrix/Vector/Scalar: \"%.400s\"", Py_TYPE(b)->tp_name);
    }

    return NULL;
}


PyObject *matrixStrassenMul(PyObject *self, PyObject *args) {
/*  Multiplies a matrix by another matrix using strassens multiplications regardless of the size (assuming their sizes
    are at least 2x2).

    Inputs: self - The matrix the function was called on, the first to multiply with.
            args - A PyTuple containing the second matrix to multiply with and the strassen cutoff point.

    Outputs: A new Matrix object calculated by performing a * b, or NULL if an error occurs.
*/

    PyObject *otherMatrix;
    int minCutoff;

    if (!PyArg_ParseTuple(args, "Oi", &otherMatrix, &minCutoff))
        return NULL;

    if (!Matrix_Check(otherMatrix)) {
        PyErr_Format(PyExc_TypeError,
                     "Matrix._strassenMul must be appled to another Matrix, not: \"%.400s\"", Py_TYPE(otherMatrix)->tp_name);
        return NULL;
    }

    if (minCutoff < 2) {
        PyErr_Format(PyExc_ValueError, "minCutoff argument to Matrix.strassenMul cannot be less than 2: %i", minCutoff);
        return NULL;
    }

    return (PyObject *)strassenWinogradMatrixMatrixMul((Matrix *)self, (Matrix *)otherMatrix, (unsigned int)minCutoff);
}


PyObject *matrixNaiveMul(PyObject *self, PyObject *other) {
/*  Multiplies a matrix by another matrix using Naive matrix multiplications regardless of their sizes

    Inputs: self  - The matrix the function was called on, the first to multiply with.
            other - Presumably the second matrix to multiply this matrix with using the naive algorithm.

    Outputs: A new Matrix object calculated by performing a * b, or NULL if an error occurs.
*/

    if (!Matrix_Check(other)) {
        PyErr_Format(PyExc_TypeError,
                     "Matrix._naiveMul must be appled to another Matrix, not: \"%.400s\"", Py_TYPE(other)->tp_name);
        return NULL;
    }

    return (PyObject *)_matrixMatrixMul((Matrix *)self, (Matrix *)other, 0);
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


PyObject *matrixRow(PyObject *self, PyObject *args) {
/*  Returns the ith row from the Matrix self.

    Inputs: self - The matrix to return the ith row from.
            args - A PyTuple containing a PyNumber of the index of the row to return.

    Outputs: A Vector object of the ith row in self if successful, NULL if an error occurs.
*/

    long i;
    Matrix *m = (Matrix *)self;
    Vector *v;

    if (!PyArg_ParseTuple(args, "l", &i))
        return NULL;

    if (i >= m->rows || i < 0) {
        PyErr_Format(PyExc_IndexError, "Cannot return row number %ld of Matrix with %u rows.", i, m->rows);
        return NULL;
    }

    if ((v = _vectorCopy(Matrix_GetVector(m, i))) == NULL)
        return NULL;

    return (PyObject *)v;
}


PyObject *matrixColumn(PyObject *self, PyObject *args) {
/*  Returns the ith column from the Matrix self.

    Inputs: self - The matrix to return the ith column from.
            args - A PyTuple containing a PyNumber of the index of the column to return.

    Outputs: A Vector object of the ith column in self if successful, NULL if an error occurs.
*/

    long col,
         i;
    Matrix *m = (Matrix *)self;
    Vector *v;

    if (!PyArg_ParseTuple(args, "l", &col))
        return NULL;

    if (col >= m->columns || col < 0) {
        PyErr_Format(PyExc_IndexError, "Cannot return column number %ld of Matrix with %u columns.", col, m->columns);
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

    return Py_BuildValue("NN", (PyObject *)l, (PyObject *)u);
}


PyObject *matrixFactorLDU(PyObject *self) {
/*  Factors a matrix A into a lower triangular form, diagonal form and an upper triangular form such that self = L*D*U

    Inputs: self - The matrix to factor.

    Outputs: A PyTuple containing (L, D, U).
*/

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

    return Py_BuildValue("NNN", (PyObject *)l, (PyObject *)d, (PyObject *)u);
}


PyObject *matrixFactorPLU(PyObject *self) {
/*  Factors a matrix A into a lower triangular form, upper triangular form and permutation matrix
    such that self = A = P * L * U

    Inputs: self - The matrix to factor.

    Outputs: A PyTuple containing (P, L, U).
*/

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

    return Py_BuildValue("NNN", (PyObject *)p, (PyObject *)l, (PyObject *)u);
}


PyObject *matrixFactorPLDU(PyObject *self) {
/*  Factors a matrix A into a lower triangular form, upper triangular form, diagonal form,
    and permutation matrix such that self = A = P * L * D * U

    Inputs: self - The matrix to factor.

    Outputs: A PyTuple containing (P, L, D, U).
*/

    Matrix *p,
           *l,
           *d,
           *u,
           *m = (Matrix *)self;

    if ((l = _matrixNew(m->rows, m->rows)) == NULL)
        return NULL;
    if ((p = _matrixNew(m->rows, m->rows)) == NULL) {
        Py_DECREF(l);
        return NULL;
    }
    if ((d = _matrixNew(m->rows, m->rows)) == NULL) {
        Py_DECREF(p);
        Py_DECREF(l);
        return NULL;
    }
    if ((u = _matrixNew(m->rows, m->columns)) == NULL) {
        Py_DECREF(p);
        Py_DECREF(l);
        Py_DECREF(d);
        return NULL;
    }

    if (!_matrixPALDU(p, m, l, d, u, 1)) {
        Py_DECREF(p);
        Py_DECREF(l);
        Py_DECREF(d);
        Py_DECREF(u);
        return NULL;
    }

    return Py_BuildValue("NNNN", (PyObject *)p, (PyObject *)l, (PyObject *)d, (PyObject *)u);
}


PyObject *matrixInverse(PyObject *self) {
/*  Constructs and returns the inverse of a given matrix.

    Inputs: self - The matrix to inverse.

    Outputs: A new matrix Object constructed from the inverse of self.
*/

    return (PyObject *)_matrixInverse((Matrix *)self);
}


PyObject *matrixRank(PyObject *self) {
/*  Determines the rank of this matrix.
    The rank of a matrix is a count of the number of linearly idnependent rows in it.

    Inputs: self - The matrix to determine the rank of.

    Outputs: A PyNumber containing the rank of self.
*/

    unsigned int rank;

    rank = _matrixRank((Matrix *)self);

    if (PyErr_Occurred())
        return NULL;

    return PyLong_FromUnsignedLong((unsigned long)rank);
}


PyObject *matrixDeterminant(PyObject *self) {
/*  Calculates the determinant of this matrix.

    Inputs: self - The matrix to take the determinant of.

    Outputs: A PyNumber containing the determinant of self if successful, or an error if not.
*/

    VECTOR_TYPE determinant;
    Matrix *m = (Matrix *)self;

    if (m->rows != m->columns) {
        PyErr_SetString(PyExc_ValueError, "Matrix.determinant must be run on a square matrix.");
        return NULL;
    }

    determinant = _matrixDeterminant(m);

    if (PyErr_Occurred())
        return NULL;

    return PyNumber_FROM_VECTOR_TYPE(determinant);
}


PyObject *matrixTrace(PyObject *self) {
/*  Determines the trace of this matrix.
    The trace of a matrix is a sum of its components along the diagonal.
    Can only be computed on square matrices.

    Inputs: self - The matrix to calculate the trace for.

    Outputs: A PyNumber containing the trace of self.
*/

    Matrix *m = (Matrix *)self;
    VECTOR_TYPE trace = 0;
    unsigned int row;

    if (m->rows != m->columns) {
        PyErr_SetString(PyExc_ValueError, "Matrix.trace can only computed on square matrices.");
        return NULL;
    }

    for (row = 0; row < m->rows; row++)
        trace += Matrix_GetValue(m, row, row);

    return PyNumber_FROM_VECTOR_TYPE(trace);
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
/*  Determines whether or not a matrix is invertible.

    Inputs: self - The matrix to test for whether or not it is invertible.

    Outputs: A PyBoolean depicting whether or not self is invertible or not.
*/

    if (_matrixInvertible((Matrix *)self))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}
