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

static int matrixInit(Matrix *, PyObject *, PyObject *);
static void matrixDeInit(Matrix *);

static int matrixInit(Matrix *self, PyObject *args, PyObject *kwargs) {
    PyObject *iterable = NULL,
             *group = NULL,
             *item = NULL;
    unsigned int i,
                 j;
    static char *kwargList[] = {"iterable", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwargList, &iterable))
        return -1;

    // Assert that we got an iterable, and only contains iterables of the same length
    if ((iterable = PySequence_Fast(iterable, "Non-sequence type passed.")) == NULL)
        return -1;
    if (PySequence_Fast_GET_SIZE(iterable) == 0) {
        PyErr_SetString(PyExc_ValueError, "Empty iterable passed.");
        return -1;
    }
    for (i = 0; i < PySequence_Fast_GET_SIZE(iterable); i++) {
        item = PySequence_Fast_GET_ITEM(iterable, i);
        if (!PySequence_Check(item)) {
            PyErr_Format(PyExc_TypeError, "Non-sequence in iterable slot %d", i);
            return -1;
        }
        if (i == 0) {
            j = PySequence_Fast_GET_SIZE(item);
        } else {
            if (PySequence_Fast_GET_SIZE(item) != j) {
                PyErr_Format(PyExc_TypeError, "Length of rows in slots 0 & %d differ (%d & %d).",
                             i, j, (int)PySequence_Fast_GET_SIZE(item));
                return -1;
            }
        }
    }

    // If we have already been initialized, free the memory reserved in our data variable before it is reinitialized
    if (self->data != NULL) {
        matrixDeInit(self);
    }

    self->height = PySequence_Fast_GET_SIZE(iterable);
    self->width = PySequence_Fast_GET_SIZE(PySequence_Fast_GET_ITEM(iterable, 0));
    self->data = PyMem_New(typeof(*(self->data)), self->height * self->width);

    // Initialize self->data from the given iterable
    for (i = 0; i < self->height; i++) {
        group = PySequence_Fast_GET_ITEM(iterable, i);
        for (j = 0; j < self->width; j++) {
            item = PyNumber_Float(PySequence_Fast_GET_ITEM(group, j));
            if (item == NULL) {
                PyErr_Format(PyExc_TypeError, "Non-numeric object in iterable slot %d", i);
                return -1;
            }
        }
        Matrix_SetValue(self, i, j, PyFloat_AsDouble(item));
    }

    return 0;
}

static void matrixDeInit(Matrix *self) {
    PyMem_Free(self->data);
}

