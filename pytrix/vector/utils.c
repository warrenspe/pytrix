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
Vector *_vectorNew(unsigned int);
Vector *_vectorCopy(Vector *);
unsigned int _assertVectorDimensionsEqual(Vector *, Vector *);
unsigned int _assertVector(PyObject *);

PyObject *_vectorToTuple(Vector *);
double _vectorDot(Vector *, Vector *);
double _vectorLength(Vector *);
Vector *_vectorAdd(Vector *, Vector *);
Vector *_vectorSub(Vector *, Vector *);
Vector *_vectorMul(Vector *, VECTOR_TYPE);
Vector *_vectorDiv(Vector *, VECTOR_TYPE);
Vector *_vectorNeg(Vector *);
unsigned char _vectorTrue(Vector *);
unsigned char _vectorsEqual(Vector *, Vector *);

VECTOR_TYPE *_newVectorData(unsigned int dimensions) {
/*  Construct a data buffer for a new vector.

    Inputs: dimensions - The number of dimensions the buffer needs to be able to hold data for.

    Outputs: Either a VECTOR_TYPE pointer to the newly created buffer or NULL if unsuccessful.
*/

    VECTOR_TYPE *data = PyMem_New(VECTOR_TYPE, dimensions);

    return data;
}


Vector *_vectorNew(unsigned int dimensions) {
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
    newVector->data = data;

    PyVector = PyObject_Init((PyObject *)newVector, &VectorType);

    return (Vector *)PyVector;
}


Vector *_vectorCopy(Vector *v) {
    Vector *copy = _vectorNew(v->dimensions);

    if (copy == NULL)
        return NULL;

    memcpy(copy->data, v->data, sizeof(VECTOR_TYPE) * v->dimensions);

    return copy;
}


unsigned int _assertVectorDimensionsEqual(Vector *self, Vector *other) {
    if (self->dimensions != other->dimensions) {
        PyErr_SetString(PyExc_ValueError, "Vectors must be of the same dimensions.");
        return 0;
    }

    return 1;
}


unsigned int _assertVector(PyObject *toCheck) {
    if (!Vector_Check(toCheck)) {
        PyErr_Format(PyExc_TypeError, "Given object is not a Vector: \"%.400s\"", Py_TYPE(toCheck)->tp_name);
        return 0;
    }

    return 1;
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

    if (!_assertVectorDimensionsEqual(self, other))
        return 0;

    for (i = 0; i < self->dimensions; i++) {
        product += *(self->data + i) * *(other->data + i);
    }

    return product;
}


double _vectorLength(Vector *self) {
    return sqrt(_vectorDot(self, self));
}


Vector *_vectorAdd(Vector *self, Vector *other) {
    Vector *sum;
    unsigned int i;

    if (!_assertVectorDimensionsEqual(self, other))
        return NULL;

    if ((sum = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(sum, i, Vector_GetValue(self, i) + Vector_GetValue(other, i));

    return sum;
}


Vector *_vectorSub(Vector *self, Vector *other) {
    Vector *difference;
    unsigned int i;

    if (!_assertVectorDimensionsEqual(self, other))
        return NULL;

    if ((difference = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(difference, i, Vector_GetValue(self, i) - Vector_GetValue(other, i));

    return difference;
}


Vector *_vectorMul(Vector *self, VECTOR_TYPE multiplier) {
    Vector *product;
    unsigned int i;

    if ((product = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(product, i, Vector_GetValue(self, i) * multiplier);

    return product;
}


Vector *_vectorDiv(Vector *self, VECTOR_TYPE divisor) {
    Vector *quotient;
    unsigned int i;

    if ((quotient = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(quotient, i, Vector_GetValue(self, i) / divisor);

    return quotient;
}


Vector *_vectorNeg(Vector *self) {
    Vector *neg;
    unsigned int i;

    if ((neg = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(neg, i, -Vector_GetValue(self, i));

    return neg;
}


unsigned char _vectorsEqual(Vector *self, Vector *other) {
    unsigned int i;

    if (self->dimensions != other->dimensions)
        return 0;

    for (i = 0; i < self->dimensions; i++) {
        if (Vector_GetValue(self, i) != Vector_GetValue(other, i))
            return 0;
    }

    return 1;
}
