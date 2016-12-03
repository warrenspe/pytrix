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


void _matrixCopyData(Matrix *in, Matrix *out) {
/*  Copies the contents from one matrix to another.  Does no error checking.

    Inputs: in  - The matrix to copy the data from.
            out - The matrix to copy the data to.
*/

    unsigned int i;

    for (i = 0; i < in->rows; i++)
        memcpy(Matrix_GetVector(out, i)->data, Matrix_GetVector(in, i)->data, sizeof(VECTOR_TYPE) * in->columns);
}


Matrix *_matrixCopy(Matrix *m) {
/*  Creates a new copy of the given matrix.

    Inputs: m - The matrix to copy.

    Outputs: A Matrix object with the same number of rows/columns and data as m, or NULL if an exception occurred.
*/

    Matrix *copy = _matrixNew(m->rows, m->columns);

    if (copy == NULL)
        return NULL;

    _matrixCopyData(m, copy);

    return copy;
}


void _matrixInitBlank(Matrix *m) {
/*  Initializes a matrix so that it is completely filled with 0s.

    Inputs: m - The matrix to clear.
*/

    unsigned int row;

    for (row = 0; row < m->rows; row++)
        memset(Matrix_GetVector(m, row)->data, '\x00', sizeof(VECTOR_TYPE) * m->columns);
}


unsigned char _matrixInitIdentity(Matrix *m, unsigned char blank) {
/*  Initializes a matrix so that it is an identity matrix.  Requires that m be a square matrix.

    Inputs: m     - The matrix to initialize to an identity matrix.
            blank - 1 or 0 indicating whether or not we should blank the matrix before initializing it

    Outputs: 1 if successful, 0 if an error occurred.
*/

    unsigned int row;

    // Assert that the matrix is square
    if (m->rows != m->columns) {
        PyErr_SetString(PyExc_ValueError, "Cannot initialize non-square matrix as identity.");
        return 0;
    }

    if (blank)
        _matrixInitBlank(m);

    for (row = 0; row < m->rows; row++)
        Matrix_SetValue(m, row, row, 1);

    return 1;
}


unsigned char _assertMatrix(PyObject *toCheck) {
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


unsigned char _assertMatrixDimensionsEqual(Matrix *a, Matrix *b) {
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


unsigned char _inplaceMatrixMatrixMul(Matrix *left, Matrix *right, Matrix *out) {
/*  Performs matrix multiplication of left * right inserting the result into out.
    Note that out may safely be one of left, right.

    Inputs: left  - The left matrix to multiply
            right - The right matrix to multiply.
            out   - The matrix to insert the results into.

    Outputs: 1 if successful, 0 if an error occurred.
*/

    unsigned int row,
                 col,
                 i;
    Vector *temp;
    VECTOR_TYPE val = 0;

    if (left->columns != right->rows) {
        PyErr_SetString(PyExc_ValueError, "A * B Matrix multiplication requires A.columns = B.rows");
        return 0;
    }

    if (out->rows != left->rows || out->columns != right->columns) {
        PyErr_SetString(PyExc_ValueError, "Out Matrix.rows != left Matrix.rows or Out Matrix.columns != right Matrix.Columns");
        return 0;
    }

    // If we're doing an in-place left-hand side matrix multiplication
    if (left == out) {
        if ((temp = _vectorNew(left->columns)) == NULL)
            return 0;

        for (row = 0; row < left->rows; row++) {
            for (col = 0; col < right->columns; col++) {
                val = 0;
                for (i = 0; i < left->columns; i++)
                    val += Matrix_GetValue(left, row, i) * Matrix_GetValue(right, i, col);
                Vector_SetValue(temp, col, val);
            }
            _vectorCopyData(temp, Matrix_GetVector(out, row));
        }

        Py_DECREF(temp);

    // If we're doing an in-place right-hand side matrix multiplication
    } else if (right == out) {
        if ((temp = _vectorNew(left->columns)) == NULL)
            return 0;

        for (col = 0; col < right->columns; col++) {
            for (row = 0; row < left->rows; row++) {
                val = 0;
                for (i = 0; i < left->columns; i++)
                    val += Matrix_GetValue(left, row, i) * Matrix_GetValue(right, i, col);
                Vector_SetValue(temp, row, val);
            }
            for (i = 0; i < left->columns; i++)
                Matrix_SetValue(out, i, col, Vector_GetValue(temp, i));
        }

        Py_DECREF(temp);

    // Otherwise we're doing regular matrix multiplication
    } else {
        // Otherwise use the naive n^3 algorithm
        for (row = 0; row < left->rows; row++) {
            for (col = 0; col < right->columns; col++) {
                val = 0;
                for (i = 0; i < left->columns; i++)
                    val += Matrix_GetValue(left, row, i) * Matrix_GetValue(right, i, col);
                Matrix_SetValue(out, row, col, val);
            }
        }
    }

    return 1;
}


Matrix *_matrixMatrixMul(Matrix *a, Matrix *b, unsigned char checkStrassen) {
/*  Multiplies a matrix by another matrix.

    Inputs: a             - The first matrix to be multiplied.
            b             - The second matrix to be multiplied.
            checkStrassen - A boolean depicting whether or not we should check if using the Strassen-Winograd algorithm
                            could offer any speed benefit.

    Outputs: A new matrix constructed by performing a * b, or NULL if an error occurred.
*/

    Matrix *outMatrix;

    if (a->columns != b->rows) {
        PyErr_SetString(PyExc_ValueError, "A * B Matrix multiplication requires A.columns = B.rows");
        return NULL;
    }

    // Check if the Strassen-Winograd algorithm is applicable & would offer any benefit
    if (checkStrassen && a->rows == a->columns && b->rows == b->columns && a->rows == b->rows && a->rows >= MIN_STRASSEN_SIZE) {
        return strassenWinogradMatrixMatrixMul(a, b, STRASSEN_CUTOFF);
    }

    if ((outMatrix = _matrixNew(a->rows, b->columns)) == NULL)
        return NULL;

    // Otherwise use the naive n^3 algorithm
    if (!_inplaceMatrixMatrixMul(a, b, outMatrix)) {
        Py_DECREF(outMatrix);
        return NULL;
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


Matrix *_matrixTranspose(Matrix *self) {
/*  Creates and returns a new matrix constructed by taking the transpose of this matrix.

    Inputs: self - The matrix being transposed.

    Outputs: A new Matrix Object constructed by taking the transpose of self, or NULL if an error occurred.
*/

    Matrix *transposed;
    unsigned int row,
                 col;

    if ((transposed = _matrixNew(self->columns, self->rows)) == NULL)
        return NULL;

    for (row = 0; row < self->rows; row++) {
        for (col = 0; col < self->columns; col++) {
            Matrix_SetValue(transposed, col, row, Matrix_GetValue(self, row, col));
        }
    }

    return transposed;
}


Matrix *_matrixPermute(Matrix *self, unsigned int a, unsigned int b) {
/*  Returns a new matrix with the given rows permuted.

    Inputs: self - The matrix to permute.

    Outputs: A new copy of self with the given rows permuted.
*/

    Matrix *permuted;
    Vector *tempRow;

    // Ensure that the given integers are in range
    if (a >= self->rows) {
        PyErr_Format(PyExc_ValueError, "Given permute index is greater than Matrix.rows: %u", a);
        return NULL;
    }
    if (b >= self->rows) {
        PyErr_Format(PyExc_ValueError, "Given permute index is greater than Matrix.rows: %u", b);
        return NULL;
    }

    if ((permuted = _matrixCopy(self)) == NULL)
        return NULL;

    // Permute the rows of permuted
    tempRow = Matrix_GetVector(permuted, a);
    Matrix_SetVector(permuted, a, Matrix_GetVector(permuted, b));
    Matrix_SetVector(permuted, b, tempRow);

    return permuted;
}


unsigned char _matrixInvertible(Matrix *m) {
/*  Determines whether or not a matrix is invertible.

    Inputs: m - The matrix to test for whether or not it is invertible.

    Outputs: 0 or 1 depending on whether or not m is invertible.
             Calling functions should check whether or not an error was raised with PyErr_Occurred.
*/

    unsigned int rank;

    if (m->rows != m->columns)
        return 0;

    if (m->rows == 1)
        return Matrix_GetValue(m, 0, 0) != 0;

    if (m->rows == 2) {
        return _matrixDeterminant(m) != 0;
    }

    rank = _matrixRank((Matrix *)m);

    if (PyErr_Occurred())
        return 0;

    return rank == m->rows;
}


unsigned char _matrixSymmetrical(Matrix *self) {
/*  Determines whether or not the given matrix is symmetric.

    Inputs: self - The matrix to check for symmetry.

    Outputs: 1 if the matrix is symmetric, else 0.
*/

    unsigned int row,
                 col;

    // Non-square matrices are automatically non-symmetrical
    if (self->rows != self->columns)
        return 0;

    for (row = 0; row < self->rows; row++) {
        for (col = 0; col < self->columns; col++) {
            // Don't worry about values along the diagonal
            if (row != col) {
                if (Matrix_GetValue(self, row, col) != Matrix_GetValue(self, col, row))
                    return 0;
            }
        }
    }

    return 1;
}


unsigned int _matrixRank(Matrix *m) {
/*  Determines the rank of this matrix.
    The rank of a matrix is a count of the number of linearly idnependent rows in it.

    Inputs: m - The matrix to determine the rank of.

    Outputs: The rank of m.  Note that calling functions should check PyErr_Occurred
*/

    unsigned int rank = 0;
    unsigned int row = 0,
                 col;
    Matrix *u;

    PyErr_Clear();

    if ((u = _matrixNew(m->rows, m->columns)) == NULL)
        return 0;

    if (!_matrixPALDU(NULL, m, NULL, NULL, u, 1)) {
        Py_DECREF(u);
        return 0;
    }

    // Calculate the rank of u
    for (col = 0; col < u->columns && row < u->rows; col++) {
        if (Matrix_GetValue(u, row, col) != 0) {
            rank++;
            row++;
        }
    }

    Py_DECREF(u);
    return rank;
}


VECTOR_TYPE _matrixDeterminant(Matrix *m) {
/*  Calculates the determinant of a given matrix.

    Inputs: m - The matrix to take the determinant of.

    Outputs: The determinant of m.
             Calling functions should test whether or not an error occurred with PyErr_Occurred.
*/

    unsigned int row,
                 col,
                 tempIter,
                 permutations = 0;
    Matrix *tempM;
    Vector *tempRow;
    VECTOR_TYPE determinant = 1,
                multiplier;

    PyErr_Clear();

    if (m->columns != m->rows) {
        PyErr_SetString(PyExc_ValueError, "Cannot take determinant of a non-square matrix");
        return 0;
    }

    if (m->rows == 1)
        return Matrix_GetValue(m, 0, 0);

    if (m->rows == 2)
        return ((Matrix_GetValue(m, 1, 1) * Matrix_GetValue(m, 0, 0)) -
                      (Matrix_GetValue(m, 0, 1) * Matrix_GetValue(m, 1, 0)));

    if ((tempM = _matrixCopy(m)) == NULL)
        return 0;

    for (col = 0; col < tempM->columns; col++) {
        // Find a pivot row and move it into its correct position
        for (row = col; row < tempM->rows; row++) {
            if (Matrix_GetValue(tempM, row, col) != 0) {
                // Permute this row to its correct position
                if (row != col) {
                    permutations++;
                    tempRow = Matrix_GetVector(tempM, row);
                    Matrix_SetVector(tempM, row, Matrix_GetVector(tempM, col));
                    Matrix_SetVector(tempM, col, tempRow);
                }
                break;
            }
        }
        // If we were unable to find a pivot, our determinant is 0
        if (row == tempM->rows) {
            Py_DECREF(tempM);
            return 0;
        }

        // Eliminate all lower rows
        for (row = col + 1; row < tempM->rows; row++) {
            if (Matrix_GetValue(tempM, row, col) != 0) {
                multiplier = Matrix_GetValue(tempM, row, col) / Matrix_GetValue(tempM, col, col);
                for (tempIter = col; tempIter < tempM->columns; tempIter++) {
                    Matrix_SetValue(tempM, row, tempIter, Matrix_GetValue(tempM, row, tempIter) -
                                                     (multiplier * Matrix_GetValue(tempM, col, tempIter)));
                }
            }
        }
    }

    for (tempIter = 0; tempIter < m->rows; tempIter++)
        determinant *= Matrix_GetValue(tempM, tempIter, tempIter);

    // If we performed an odd number of permutations make the determinant negative.
    if (permutations & 1)
        determinant = -determinant;

    Py_DECREF(tempM);

    return determinant;
}


unsigned char _matrixPALDU(Matrix *p, Matrix *a, Matrix *l, Matrix *d, Matrix *u, unsigned char allowPerms) {
/*  Calculates the (P^-1)A=LDU decomposition for the given Matrix a.
    Notes:
        * All parameters other than a, u and allowPerms are optional.  If passed, they should be a pointer to an empty
          Matrix object. If not passed, they will not be computed.
        * p is the inverse of a matrix constructed from the permutations applied to u during decomposition.
        * l is a unit lower-triangular matrix constructed from the row operations applied to u during decomposition.
        * d is a diagonal matrix constructed from the pivots of L
            if d is passed, l must also be passed.
        * u is a unit upper-triangular matrix constructed from the remains of a after decomposition.
        * allowPerms is a boolean indicating whether or not we're allowed to make permutations to u.
            if 0 and a permutation is required, an exception is set and 0 returned.

    Inputs: a       - The matrix to perform the decomposition on.
            p, l, d - Either NULL pointers or pointers to Matrix objects to be populated.
            u       - A pointer to the reduced row-echelon form of a.

    Outputs: 1 if successful, 0 if an error occurred.
*/

    unsigned int col,
                 row = 0,
                 rowIter,
                 colIter;
    VECTOR_TYPE multiplier,
                pivot;
    Vector *tempRow;

    // Ensure that all the matrices given have the same dimensions as a.
    if (u == NULL || !_assertMatrixDimensionsEqual(a, u))
        return 0;
    _matrixInitBlank(u);
    if (l != NULL) {
        if (l->columns != l->rows) {
            PyErr_SetString(PyExc_ValueError, "Lower matrix argument to PALDU must be square.");
            return 0;
        }
        if (l->columns != a->rows) {
            PyErr_SetString(PyExc_ValueError, "Lower matrix argument to PALDU's columns must equal input matrix's rows.");
            return 0;
        }
        _matrixInitBlank(l);
    }
    if (d != NULL) {
        if (l == NULL) {
            PyErr_SetString(PyExc_ValueError, "D argument must be accompanied by a L argument to PALDU.");
            return 0;
        }
        if (!_assertMatrixDimensionsEqual(l, d))
            return 0;
        if (!_matrixInitIdentity(d, 1))
            return 0;
    }
    if (p != NULL) {
        if (p->columns != p->rows) {
            PyErr_SetString(PyExc_ValueError, "Permutation matrix argument to PALDU must be square.");
            return 0;
        }
        if (a->rows != p->columns) {
            PyErr_SetString(PyExc_ValueError, "Permutation matrix argument to PALDU's columns must equal input matrix's rows.");
            return 0;
        }
        if (!_matrixInitIdentity(p, 1))
            return 0;
    }

    // Copy the data from a into u; which we will act upon to convert it into an upper trianguler and in the
    // process populate any other matrices we need to.
    _matrixCopyData(a, u);

    // Process each column of the matrix in turn
    for (col = 0; col < u->columns && row < u->rows; col++) {
        // If the pivot in the current row and column is a 0, we either require a permutation or all rows below us to
        // also have 0 for this column.
        if (Matrix_GetValue(u, row, col) == 0) {
            for (rowIter = row + 1; rowIter < u->rows; rowIter++) {
                // If we've found a row to permute with
                if (Matrix_GetValue(u, rowIter, col) != 0) {
                    // If we're not allowed to permute, raise an exception
                    if (!allowPerms) {
                        PyErr_SetString(PyExc_ValueError, "Cannot factor non-invertible Matrix.");
                        return 0;
                    }

                    // Update p
                    if (p != NULL) {
                        tempRow = Matrix_GetVector(p, row);
                        Matrix_SetVector(p, row, Matrix_GetVector(p, rowIter));
                        Matrix_SetVector(p, rowIter, tempRow);
                    }

                    // Permute the rows of u and l
                    tempRow = Matrix_GetVector(u, row);
                    Matrix_SetVector(u, row, Matrix_GetVector(u, rowIter));
                    Matrix_SetVector(u, rowIter, tempRow);
                    if (l != NULL) {
                        tempRow = Matrix_GetVector(l, row);
                        Matrix_SetVector(l, row, Matrix_GetVector(l, rowIter));
                        Matrix_SetVector(l, rowIter, tempRow);
                    }
                    break;
                }
            }

            // If we're here and rowIter == u->rows, all rows below us are 0.  Move on to the next column
            if (rowIter == u->rows)
                continue;
        }

        pivot = Matrix_GetValue(u, row, col);

        // We have found a pivot.  For each row below us, if the value is non-zero, eliminate it
        for (rowIter = row + 1; rowIter < u->rows; rowIter++) {
            if (Matrix_GetValue(u, rowIter, col) != 0) {
                // Eliminate this row
                multiplier = Matrix_GetValue(u, rowIter, col) / pivot;
                Matrix_SetValue(u, rowIter, col, 0);
                for (colIter = col + 1; colIter < u->columns; colIter++) {
                    Matrix_SetValue(u, rowIter, colIter,
                        Matrix_GetValue(u, rowIter, colIter) - (multiplier * Matrix_GetValue(u, row, colIter)));
                }

                // Update l from this elimination
                if (l != NULL)
                    Matrix_SetValue(l, rowIter, row, multiplier);
            }
        }

        // Record the pivot in d if passed
        if (d != NULL) {
            Matrix_SetValue(d, row, row, pivot);

            // Reduce this row now that we're done with it
            for (colIter = col; colIter < u->columns; colIter++)
                Matrix_SetValue(u, row, colIter, Matrix_GetValue(u, row, colIter) / pivot);
        }

        // We're done with this row
        row++;
    }

    // Initialize the diagonal 1's of l
    if (l != NULL && !_matrixInitIdentity(l, 0))
        return 0;

    return 1;
}


Matrix *_matrixInverse(Matrix *m) {
/*  Attempts to find the inverse for the given matrix.

    Inputs: m - The matrix to inverse.

    Outputs: A Matrix object if successful.  NULL if an error occurs.
*/

    Matrix *inverse,
           *tempM;
    Vector *tempRow;
    VECTOR_TYPE multiplier;
    unsigned int row,
                 col,
                 tempIter;

    if (m->rows != m->columns) {
        PyErr_SetString(PyExc_ValueError, "Cannot find inverse of non-square matrix.");
        return NULL;
    }

    if ((inverse = _matrixNew(m->columns, m->rows)) == NULL)
        return NULL;

    // If m is 1 x 1 we can do this in 1 operation
    if (m->rows == 1) {
        if (Matrix_GetValue(m, 0, 0) == 0) {
            PyErr_SetString(PyExc_ValueError, "Cannot take inverse of non-invertible matrix.");
            Py_DECREF(inverse);
            return NULL;
        }
        Matrix_SetValue(inverse, 0, 0, 1 / Matrix_GetValue(m, 0, 0));
        return inverse;
    }

    // If m is 2 x 2 we can do this faster without guassian elimination
    if (m->rows == 2) {
        multiplier = _matrixDeterminant(m);
        if (PyErr_Occurred()) {
            Py_DECREF(inverse);
            return NULL;
        }

        // Assert that it's invertible
        if (multiplier == 0) {
            PyErr_SetString(PyExc_ValueError, "Cannot take inverse of non-invertible matrix.");
            Py_DECREF(inverse);
            return NULL;
        }

        multiplier = 1 / multiplier;
        Matrix_SetValue(inverse, 0, 0, multiplier * Matrix_GetValue(m, 1, 1));
        Matrix_SetValue(inverse, 0, 1, -(multiplier * Matrix_GetValue(m, 0, 1)));
        Matrix_SetValue(inverse, 1, 0, -(multiplier * Matrix_GetValue(m, 1, 0)));
        Matrix_SetValue(inverse, 1, 1, multiplier * Matrix_GetValue(m, 0, 0));
        return inverse;
    }

    if ((tempM = _matrixCopy(m)) == NULL) {
        Py_DECREF(inverse);
        return NULL;
    }
    if (!_matrixInitIdentity(inverse, 1)) {
        Py_DECREF(inverse);
        Py_DECREF(tempM);
        return NULL;
    }

    // Otherwise use guassian elimination to construct our inverse matrix
    for (col = 0; col < tempM->columns; col++) {
        // Find a pivot row and move it into its correct position
        for (row = col; row < tempM->rows; row++) {
            if (Matrix_GetValue(tempM, row, col) != 0) {
                // Permute this row to its correct position
                if (row != col) {
                    tempRow = Matrix_GetVector(tempM, row);
                    Matrix_SetVector(tempM, row, Matrix_GetVector(tempM, col));
                    Matrix_SetVector(tempM, col, tempRow);
                    tempRow = Matrix_GetVector(inverse, row);
                    Matrix_SetVector(inverse, row, Matrix_GetVector(inverse, col));
                    Matrix_SetVector(inverse, col, tempRow);
                }
                break;
            }
        }
        // Ensure we were able to find a pivot
        if (row == tempM->rows) {
            PyErr_SetString(PyExc_ValueError, "Cannot take inverse of non-invertible matrix.");
            Py_DECREF(inverse);
            Py_DECREF(tempM);
            return NULL;
        }

        // Eliminate all non-pivot rows
        for (row = 0; row < tempM->rows; row++) {
            if (row != col && Matrix_GetValue(tempM, row, col) != 0) {
                multiplier = Matrix_GetValue(tempM, row, col) / Matrix_GetValue(tempM, col, col);
                for (tempIter = 0; tempIter < tempM->columns; tempIter++) {
                    // We only have to eliminate tempM row values where tempIter >= col
                    if (tempIter >= col) {
                        Matrix_SetValue(tempM, row, tempIter, Matrix_GetValue(tempM, row, tempIter) -
                                                         (multiplier * Matrix_GetValue(tempM, col, tempIter)));
                    }
                    Matrix_SetValue(inverse, row, tempIter, Matrix_GetValue(inverse, row, tempIter) -
                                                     (multiplier * Matrix_GetValue(inverse, col, tempIter)));
                }
            }
        }
        // Reduce our pivot row so the pivot is 1
        if (Matrix_GetValue(tempM, col, col) != 1) {
            multiplier = 1 / Matrix_GetValue(tempM, col, col);
            for (tempIter = 0; tempIter < tempM->columns; tempIter++) {
                // We only have to reduce tempM row values where tempIter >= col
                if (tempIter >= col) {
                    Matrix_SetValue(tempM, col, tempIter, multiplier * Matrix_GetValue(tempM, col, tempIter));
                }
                Matrix_SetValue(inverse, col, tempIter, multiplier * Matrix_GetValue(inverse, col, tempIter));
            }
        }
    }

    Py_DECREF(tempM);

    return inverse;
}
