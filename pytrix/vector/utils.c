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

VECTOR_TYPE *_newVectorData(unsigned int);
Vector *_vectorNew(unsigned int, unsigned int, unsigned int);
Vector *_vectorCopy(Vector *);
PyObject *_vectorToTuple(Vector *);
double _vectorDot(Vector *, Vector *);
double _vectorLength(Vector *);


VECTOR_TYPE *_newVectorData(unsigned int dimensions) {
/*  Construct a data buffer for a new vector.

    Inputs: dimensions - The number of dimensions the buffer needs to be able to hold data for.

    Outputs: Either a VECTOR_TYPE pointer to the newly created buffer or NULL if unsuccessful.
*/

    VECTOR_TYPE *data = PyMem_New(VECTOR_TYPE, dimensions);

    return data;
}


Vector *_vectorNew(unsigned int dimensions, unsigned int height, unsigned int width) {
    PyObject *PyVector;
    Vector *newVector;
    VECTOR_TYPE *data;

    data = _newVectorData(dimensions);

    if (data == NULL)
        return NULL;

    newVector = PyObject_New(Vector, &VectorType);

    if (newVector == NULL) {
        PyMem_Free(data);
        return NULL;
    }

    newVector->dimensions = dimensions;
    newVector->height = height;
    newVector->width = width;
    newVector->data = data;

    PyVector = PyObject_Init((PyObject *)newVector, &VectorType);

    return (Vector *)PyVector;
}


Vector *_vectorCopy(Vector *v) {
    Vector *copy = _vectorNew(v->dimensions, v->height, v->width);

    if (copy == NULL)
        return NULL;

    memcpy(copy->data, v->data, sizeof(VECTOR_TYPE) * v->dimensions);

    return copy;
}


PyObject *_vectorToTuple(Vector *self) {
    PyObject *tuple;
    unsigned int i;
    PyObject *item;

    if ((tuple = PyTuple_New(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++) {
        item = PyFloat_FromDouble(Vector_GetValue(self, i));

        if (item == NULL) {
            Py_DECREF(tuple);
            return NULL;
        }

        PyTuple_SET_ITEM(tuple, i, item);
    }

    return tuple;
}


double _vectorDot(Vector *self, Vector *other) {
/*  Calculates the dot product of this vector with another.

    Inputs: self  - This Vector object.
            other - The other Vector object.

    Outputs: A double representing the dot product of the two vectors.

    Note: Calling functions should test whether an error was set using PyErr_Occurred().
*/

    VECTOR_TYPE product = 0;
    unsigned int i;

    // Ensure the two vectors are of the same dimensions
    if (self->dimensions != other->dimensions) {
        PyErr_SetString(PyExc_ValueError, "Vectors must be of the same length in order to perform dot product.");
        return 0;
    }

    for (i = 0; i < self->dimensions; i++) {
        product += *(self->data + i) * *(other->data + i);
    }

    return product;
}


double _vectorLength(Vector *self) {
    return sqrt(_vectorDot(self, self));
}
