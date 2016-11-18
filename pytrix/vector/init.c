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


static int vectorInit(Vector *self, PyObject *args) {
/*  Initializes a vector from a sequence.

    Inputs: self - The Vector being initialized.
            args - A packed version of the arguments passed to the initializer.

    Outputs: 0 If successful, else -1
*/

    unsigned int i;
    PyObject *iterable = NULL,
             *item = NULL;

    // We expect either args to contain either a single list of values, or a series of values.
    // Assert that args isn't empty
    if (PySequence_Length(args) == 0) {
        PyErr_SetString(PyExc_TypeError,
                        "Vector.__init__ takes either an iterable of values or an unpacked sequence of values.");
        return -1;
    }

    // Check the first value of args.  If it's an iterable, assert that args contains exactly one item
    if ((item = PySequence_GetItem(args, 0)) == NULL)
        return -1;
    if (Compatible_Input_Sequence_Check(item)) {
        if (PySequence_Length(args) != 1) {
            PyErr_SetString(PyExc_TypeError,
                            "Vector.__init__ takes either an iterable of values or an unpacked sequence of values.");
            return -1;
        }
        iterable = item;

    // Otherwise, assume that we were given an unpacked sequence of values to initialize from
    } else {
        iterable = args;
    }

    // If we have already been initialized, free the memory reserved in our data variable before it is reinitialized
    if (self->data != NULL)
        PyMem_Free(self->data);

    self->dimensions = PySequence_Fast_GET_SIZE(iterable);
    if ((self->data = PyMem_New(VECTOR_TYPE, self->dimensions)) == NULL)
        return -1;

    // Initialize self->data from the given iterable
    for (i = 0; i < self->dimensions; i++) {
        item = PySequence_Fast_GET_ITEM(iterable, i);
        if (!PyNumber_Check(item)) {
            PyErr_Format(PyExc_TypeError, "Non-numeric object in iterable slot %d", i);
            return -1;
        }
        Vector_SetValue(self, i, PyNumber_AS_VECTOR_TYPE(item));
    }

    return 0;
}


static void vectorDeInit(Vector *self) {
    /* De-allocates a vector. */

    PyMem_Free(self->data);
    PyObject_Del(self);
}
