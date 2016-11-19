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

static void _freeMatrixData(Matrix *self) {
/*  Frees the data referenced by a matrix being de-initialized

    Inputs: self - The matrix whose data should be freed.
*/

    unsigned int i;

    if (self->data != NULL) {
        for (i = 0; i < self->rows; i++)
            Py_XDECREF(Matrix_GetVector(self, i));

        PyMem_Free(self->data);
    }
}

int matrixInit(Matrix *self, PyObject *args) {
/*  Initializes a matrix from a sequence of either vectors or sequences.

    Inputs: self - The Matrix being initialized.
            args - A packed version of the arguments passed to the initializer.

    Outputs: 0 If successful, else -1
*/

    unsigned int i,
                 j;
    PyObject *iterable = NULL,
             *rowSequence = NULL,
             *item = NULL;
    Vector *rowVector;


    // We expect either args to contain either a single iterable of iterables, or a series of iterables.
    // Assert that args isn't empty
    if (PySequence_Length(args) == 0) {
        PyErr_SetString(PyExc_TypeError,
                        "No input rows passed to Matrix.__init__. See help(Matrix) for more information.");
        return -1;
    }

    if ((iterable = PySequence_GetItem(args, 0)) == NULL)
        return -1;
    if (!Compatible_Input_Sequence_Check(iterable)) {
        PyErr_SetString(PyExc_TypeError, "First argument to Matrix.__init__ is not an iterable.");
        return -1;
    }

    // If iterable is empty
    if (PySequence_Length(iterable) == 0) {
        // Assert that we were only given a single argument.  One empty iterable argument implies empty matrix.
        // Multiple empty iterable arguments is undefined.
        if (PySequence_Length(args) != 1) {
            PyErr_SetString(PyExc_TypeError, "Passing more than one empty row to Matrix.__init__ is not allowed.");
            return -1;
        }

    // Otherwise iterable isn't empty.  Check if it contains iterables.
    } else {
        if ((item = PySequence_GetItem(iterable, 0)) == NULL)
            return -1;

        // Ensure that we were given an iterable
        if (Compatible_Input_Sequence_Check(item)) {
            // If iterable contains iterables, then we have a [[...], [...]] scenario.  Ensure that there were no other
            // arguments to Matrix.__init__
            if (PySequence_Length(args) != 1) {
                PyErr_SetString(PyExc_TypeError, "2d iterable passed with other arguments. Behavior is undefined.");
                return -1;
            }

            // If the length of the inner iterable is empty, ensure it is the only iterable in our iterable list
            // In other words, ([[], []]) is undefined for the same reasons that ([], []) is
            if (PySequence_Length(item) == 0 && PySequence_Length(iterable) != 1) {
                PyErr_SetString(PyExc_TypeError, "Passing more than one empty row to Matrix.__init__ is not allowed.");
                return -1;
            }

        // Otherwise our iterable doesn't contain iterables.  So, to get the [[...], [...]] structure required below
        // we will set iterable to args.  As args is an iterable containing iterables.
        } else {
            iterable = args;
        }
    }


    // Parse iterable, perform validation, and parse it into our matrix.
    self->rows = PySequence_Fast_GET_SIZE(iterable);
    self->columns = 0;

    // If our iterable has any rows ensure they are all of the same length
    if (self->rows != 0) {
        for (i = 0; i < self->rows; i++) {
            item = PySequence_Fast_GET_ITEM(iterable, i);
            if (!Compatible_Input_Sequence_Check(item)) {
                PyErr_Format(PyExc_TypeError, "Non-sequence in iterable slot %d", i);
                return -1;
            }
            if (i == 0) {
                self->columns = PySequence_Fast_GET_SIZE(item);
            } else {
                if (PySequence_Fast_GET_SIZE(item) != self->columns) {
                    PyErr_Format(PyExc_TypeError, "Length of rows in slots 0 & %d differ (%d & %d).",
                                 i, self->columns, (int)PySequence_Fast_GET_SIZE(item));
                    return -1;
                }
            }
        }
    }

    // If we have already been initialized, free the memory reserved in our data variable before it is reinitialized
    if (self->data != NULL)
        _freeMatrixData(self);

    if ((self->data = PyMem_Malloc(self->rows * sizeof(Vector *))) == NULL)
        return -1;

    memset(self->data, '\x00', sizeof(Vector *) * self->rows);

    // Initialize self->data from the given iterable
    for (i = 0; i < self->rows; i++) {
        rowSequence = PySequence_Fast_GET_ITEM(iterable, i);
        rowVector = _vectorNew(self->columns);
        for (j = 0; j < self->columns; j++) {
            item = PySequence_Fast_GET_ITEM(rowSequence, j);
            if (!PyNumber_Check(item)) {
                PyErr_Format(PyExc_TypeError, "Non-numeric object in row %d, column %d", i, j);
                return -1;
            }
            Vector_SetValue(rowVector, j, PyNumber_AS_VECTOR_TYPE(item));
        }
        Matrix_SetVector(self, i, rowVector);
    }

    return 0;
}

void matrixDeInit(Matrix *self) {
    /* De-allocates a matrix. */

    _freeMatrixData(self);
    PyObject_Del(self);
}

