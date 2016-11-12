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

Matrix *_matrixNewBlank(unsigned int rows, unsigned int columns) {
/*  Creates a new matrix object with the given number of rows / columns, but with an empty data buffer.

    Inputs: rows    - The number of rows to create the new matrix with.
            columns - The number of columns per row in the new matrix.

    Outputs: A pointer to the new Matrix object, or NULL if an error occurred.
*/

    Matrix *newMatrix;
    Vector **data;

    if ((data = PyMem_Malloc(rows * sizeof(Vector *))) == NULL)
        return NULL;

    if ((newMatrix = PyObject_New(Matrix, &MatrixType)) == NULL) {
        PyMem_Free(data);
        return NULL;
    }

    memset(data, '\x00', sizeof(Vector *) * rows);
    newMatrix->rows = rows;
    newMatrix->columns = columns;
    newMatrix->data = data;

    return newMatrix;
}


Matrix *_matrixNew(unsigned int rows, unsigned int columns) {
/*  Creates a new matrix object with the given number of rows / columns and vectors initialized in its data buffer.

    Inputs: rows    - The number of rows to create the new matrix with.
            columns - The number of columns per row in the new matrix.

    Outputs: A pointer to the new Matrix object, or NULL if an error occurred.
*/

    unsigned int i;
    Matrix *newMatrix;
    Vector *row;

    if ((newMatrix = _matrixNewBlank(rows, columns)) == NULL)
        return NULL;

    for (i = 0; i < rows; i++) {
        if ((row = _vectorNew(columns)) == NULL) {
            matrixDeInit(newMatrix);
            return NULL;
        }

        Matrix_SetVector(newMatrix, i, row);
    }

    return newMatrix;
}


Matrix *_matrixCopy(Matrix *m) {
/*  Creates a new copy of the given matrix.

    Inputs: m - The matrix to copy.

    Outputs: A Matrix object with the same number of rows/columns and data as m, or NULL if an exception occurred.
*/

    unsigned int i;
    Matrix *copy = _matrixNew(m->rows, m->columns);

    if (copy == NULL)
        return NULL;

    for (i = 0; i < m->rows; i++)
        memcpy(Matrix_GetVector(copy, i)->data, Matrix_GetVector(m, i)->data, sizeof(VECTOR_TYPE) * m->columns);

    return copy;
}


unsigned int _assertMatrix(PyObject *toCheck) {
/*  Asserts that the given PyObject is a Matrix object.

    Inputs: toCheck - A PyObject to test if whether or not it is a Matrix.

    Outputs: 1 if toCheck is a Matrix, else 0.
*/

    if (!Matrix_Check(toCheck)) {
        PyErr_Format(PyExc_TypeError, "Given object is not a Matrix: \"%.400s\"", Py_TYPE(toCheck)->tp_name);
        return 0;
    }

    return 1;
}


unsigned int _assertMatrixDimensionsEqual(Matrix *a, Matrix *b) {
/*  Asserts that two matrices dimensions are equal.  Sets a PyError if they are not.

    Inputs: a - A pointer to the first Matrix.
            b - A pointer to the second Matrix.

    Outputs: 1 if the two matrices have the same number of dimensions, else 0.
*/

    if (a->rows != b->rows || a->columns != b->columns) {
        PyErr_SetString(PyExc_ValueError, "Matrices must be of the same dimensions.");
        return 0;
    }

    return 1;
}


Matrix *_matrixAdd(Matrix *a, Matrix *b) {
/*  Adds the components of two matrices together to construct a third matrix.

    Inputs: a - The first matrix to add.
            b - The second matrix to add.

    Outputs: A new third matrix constructed by performing a + b, or NULL if an error occurred.
*/

    unsigned int i;
    Matrix *sum;
    Vector *row;

    if (!_assertMatrixDimensionsEqual(a, b))
        return NULL;

    if ((sum = _matrixNewBlank(a->rows, a->columns)) == NULL)
        return NULL;

    for (i = 0; i < sum->rows; i++) {
        if ((row = _vectorAdd(Matrix_GetVector(a, i), Matrix_GetVector(b, i))) == NULL) {
            Py_DECREF(sum);
            return NULL;
        }
        Matrix_SetVector(sum, i, row);
    }

    return sum;
}


Matrix *_matrixSub(Matrix *a, Matrix *b) {
/*  Subtracts the components of two matrices together to construct a third matrix.

    Inputs: a - The first matrix to subtract from.
            b - The second matrix to be subtracted from a.

    Outputs: A new third matrix constructed by performing a - b, or NULL if an error occurred.
*/

    unsigned int i;
    Matrix *difference;
    Vector *row;

    if (!_assertMatrixDimensionsEqual(a, b))
        return NULL;

    if ((difference = _matrixNewBlank(a->rows, a->columns)) == NULL)
        return NULL;

    for (i = 0; i < difference->rows; i++) {
        if ((row = _vectorSub(Matrix_GetVector(a, i), Matrix_GetVector(b, i))) == NULL) {
            Py_DECREF(difference);
            return NULL;
        }
        Matrix_SetVector(difference, i, row);
    }

    return difference;
}


Matrix *_scalarMatrixMul(Matrix *self, VECTOR_TYPE multiplier) {
/*  Multiplies the components of a matrix by a scalar.

    Inputs: self       - The matrix to by multiplied by scalar.
            multiplier - The scalar to multiply self by.

    Outputs: A new matrix constructed by performing self * multiplier, or NULL if an error occurred.
*/

    unsigned int i;
    Matrix *product;
    Vector *row;

    if ((product = _matrixNew(self->rows, self->columns)) == NULL)
        return NULL;

    for (i = 0; i < self->rows; i++) {
        if ((row = _vectorMul(Matrix_GetVector(self, i), multiplier)) == NULL) {
            Py_DECREF(product);
            return NULL;
        }
        Matrix_SetVector(product, i, row);
    }

    return product;
}


Vector *_vectorMatrixMul(PyObject *a, PyObject *b) {
/*  Multiplies a Matrix by a Vector.

    Inputs: a - Either the Vector or the matrix to multiply.
            b - Either the Vector or the matrix to multiply.

    Outputs: A new Vector constructed by performing a * b, or NULL if an error occurred.
*/

    unsigned int i,
                 j;
    Vector *inVector,
           *outVector;
    Matrix *inMatrix;
    VECTOR_TYPE val;

    // There are two cases here.  If the vector is the first argument, then we perform left-hand vector multiplication
    if (Vector_Check(a) && Matrix_Check(b)) {
        inVector = (Vector *)a;
        inMatrix = (Matrix *)b;

        if (inVector->dimensions != inMatrix->rows) {
            PyErr_SetString(PyExc_ValueError, "Vector * Matrix multiplication must have V.dimensions = M.rows");
            return NULL;
        }

        outVector = _vectorNew(inVector->dimensions);

        for (i = 0; i < inMatrix->columns; i++) {
            val = 0;
            for (j = 0; j < inVector->dimensions; j++) {
                val += Vector_GetValue(inVector, j) * Matrix_GetValue(inMatrix, j, i);
            }

            Vector_SetValue(outVector, i, val);
        }

        return outVector;

    // If the vector is the second, right-hand vector multiplication
    } else if (Matrix_Check(a) && Vector_Check(b)) {
        inVector = (Vector *)b;
        inMatrix = (Matrix *)a;

        if (inVector->dimensions != inMatrix->columns) {
            PyErr_SetString(PyExc_ValueError, "Matrix * Vector multiplication must have M.cols = V.dimensions");
            return NULL;
        }

        outVector = _vectorNew(inVector->dimensions);

        for (i = 0; i < inMatrix->rows; i++) {
            val = 0;
            for (j = 0; j < inVector->dimensions; j++) {
                val += Matrix_GetValue(inMatrix, i, j) * Vector_GetValue(inVector, j);
            }

            Vector_SetValue(outVector, i, val);
        }

        return outVector;
    }

    // Otherwise, our inputs are undefined. Return an error
    PyErr_SetString(PyExc_TypeError, "Undefined inputs. Vector-Matrix multiplication requires a Vector & Matrix.");
    return NULL;
}


Matrix *_matrixMatrixMul(Matrix *a, Matrix *b) {
/*  Multiplies a matrix by another matrix.

    Inputs: a - The first matrix to be multiplied.
            b - The second matrix to be multiplied.

    Outputs: A new matrix constructed by performing a * b, or NULL if an error occurred.
*/

    unsigned int row,
                 col,
                 i;
    Matrix *outMatrix;
    VECTOR_TYPE val;

    if (a->columns != b->rows) {
        PyErr_SetString(PyExc_ValueError, "A * B Matrix multiplication requires A.columns = B.rows");
        return NULL;
    }

    // Check if the Strassen-Winograd algorithm is applicable & would offer any benefit
    if (a->rows == a->columns && b->rows == b->columns && a->rows == b->rows && a->rows * a->columns > STRASSEN_CUTOFF) {
        return strassenWinogradMatrixMatrixMul(a, b);
    }

    if ((outMatrix = _matrixNew(a->rows, b->columns)) == NULL)
        return NULL;

    // Otherwise use the naive n^3 algorithm
    for (row = 0; row < a->rows; row++) {
        for (col = 0; col < b->columns; col++) {
            val = 0;
            for (i = 0; i < a->columns; i++)
                val += Matrix_GetValue(a, row, i) * Matrix_GetValue(b, i, col);
            Matrix_SetValue(outMatrix, row, col, val);
        }
    }

    return outMatrix;
}

Matrix *_matrixNeg(Matrix *a) {
/*  Negates the components of a matrix.

    Inputs: a - The matrix to negate.

    Outputs: A new matrix constructed by negating the components in a.
*/

    Matrix *neg;
    unsigned int row,
                 col;

    if ((neg = _matrixNew(a->rows, a->columns)) == NULL)
        return NULL;

    for (row = 0; row < a->rows; row++) {
        for (col = 0; col < a->columns; col++) {
            Matrix_SetValue(neg, row, col, -Matrix_GetValue(a, row, col));
        }
    }

    return neg;
}


unsigned char _matricesEqual(Matrix *a, Matrix *b) {
    unsigned int row,
                 col;

    if (a->rows != b->rows || a->columns != b->columns)
        return 0;

    for (row = 0; row < a->rows; row++) {
        for (col = 0; col < a->columns; col++) {
            if (Matrix_GetValue(a, row, col) != Matrix_GetValue(b, row, col))
                return 0;
        }
    }

    return 1;
}
