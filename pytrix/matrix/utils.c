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

Matrix *_matrixNew(unsigned int rows, unsigned int columns) {
/*  Creates a new matrix object with the given number of rows / columns.

    Inputs: rows    - The number of rows to create the new matrix with.
            columns - The number of columns per row in the new matrix.

    Outputs: A pointer to the new Matrix object, or NULL if an error occurred.
*/

    unsigned int i;
    Matrix *newMatrix;
    Vector *data,
           *row;

    if ((data = PyMem_Malloc(rows * sizeof(Vector *))) == NULL)
        return NULL;

    if ((newMatrix = PyObject_New(Matrix, &MatrixType)) == NULL) {
        PyMem_Free(data);
        return NULL;
    }

    memset(data, '\x00', sizeof(Vector *) * rows);
    newMatrix->rows = rows;
    newMatrix->columns = columns;
    newMatrix->data = &data;

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
