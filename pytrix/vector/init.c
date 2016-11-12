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

    if (!PyArg_ParseTuple(args, "O", &iterable))
        return -1;

    // Assert that we got an iterable
    if (!Compatible_Input_Sequence_Check(iterable)) {
        PyErr_SetString(PyExc_TypeError, "Non-sequence type passed.");
        return -1;
    }
    if ((iterable = PySequence_Fast(iterable, "Non-sequence type passed.")) == NULL)
        return -1;

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
