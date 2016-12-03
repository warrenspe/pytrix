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

Vector *_vectorNew(unsigned int dimensions) {
/*  Creates a new vector objects with the given number of dimensions.

    Inputs: dimensions - The number of dimensions to create the new vector object with.

    Outputs: A pointer to a Vector object, or NULL if an error occurred..
*/

    Vector *newVector;
    VECTOR_TYPE *data;

    if ((data = PyMem_New(VECTOR_TYPE, dimensions)) == NULL)
        return NULL;

    if ((newVector = PyObject_New(Vector, &VectorType)) == NULL) {
        PyMem_Free(data);
        return NULL;
    }

    newVector->dimensions = dimensions;
    newVector->data = data;

    return newVector;
}


void _vectorCopyData(Vector *in, Vector *out) {
/*  Copies the data from in to out.
    Assumes that in.dimensions == out.dimensions.

    Inputs: in  - A pointer to the Vector to copy the data from.
            out - A pointer to the Vector to copy the data to.
*/

    memcpy(out->data, in->data, sizeof(VECTOR_TYPE) * out->dimensions);
}


Vector *_vectorCopy(Vector *v) {
/*  Creates a new copy of the given vector.

    Inputs: v - The vector to copy.

    Outputs: A Vector object with the same number of dimensions and data as v, or NULL if an exception occurred.
*/

    Vector *copy = _vectorNew(v->dimensions);

    if (copy == NULL)
        return NULL;

    _vectorCopyData(v, copy);

    return copy;
}


unsigned char _assertVectorDimensionsEqual(Vector *a, Vector *b) {
/*  Asserts that two vectors dimensions are equal.  Sets a PyError if they are not.

    Inputs: a - A pointer to the first Vector.
            b - A pointer to the second Vector.

    Outputs: 1 if the two vectors have the same number of dimensions, else 0.
*/

    if (a->dimensions != b->dimensions) {
        PyErr_SetString(PyExc_ValueError, "Vectors must be of the same dimensions.");
        return 0;
    }

    return 1;
}


unsigned char _assertVector(PyObject *toCheck) {
/*  Asserts that the given PyObject is a Vector object.

    Inputs: toCheck - A PyObject to test if whether or not it is a Vector.

    Outputs: 1 if toCheck is a Vector, else 0.
*/

    if (!Vector_Check(toCheck)) {
        PyErr_Format(PyExc_TypeError, "Given object is not a Vector: \"%.400s\"", Py_TYPE(toCheck)->tp_name);
        return 0;
    }

    return 1;
}


PyObject *_vectorToTuple(Vector *self) {
/*  Constructs a PyTuple from the given vector where the i-th index of the tuple contains the i-th dimension component
    of the vector.

    Inputs: self - The vector to convert to a PyTuple.

    Outputs: A PyTuple corresponding to self if successful, else sets a PyError and returns NULL.
*/

    PyObject *tuple;
    unsigned int i;
    PyObject *item;

    if ((tuple = PyTuple_New(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++) {
        item = PyNumber_FROM_VECTOR_TYPE(Vector_GetValue(self, i));

        if (item == NULL) {
            Py_DECREF(tuple);
            return NULL;
        }

        PyTuple_SET_ITEM(tuple, i, item);
    }

    return tuple;
}


VECTOR_TYPE _vectorDot(Vector *a, Vector *b) {
/*  Calculates the dot product of two vectors.

    Inputs: a - The first Vector object.
            b - The second Vector object.

    Outputs: A VECTOR_TYPE representing the dot product of the two vectors.

    Note: Calling functions should test whether an error was set using PyErr_Occurred().
*/

    VECTOR_TYPE product = 0;
    unsigned int i;

    if (!_assertVectorDimensionsEqual(a, b))
        return 0;

    for (i = 0; i < a->dimensions; i++)
        product += Vector_GetValue(a, i) * Vector_GetValue(b, i);

    return product;
}


VECTOR_TYPE _vectorLength(Vector *self) {
/*  Returns the length of a vector.

    Inputs: self - The vector to determine the length of.

    Outputs: A VECTOR_TYPE containing the length of the vector.
*/

    return sqrt(_vectorDot(self, self));
}


Vector *_vectorAdd(Vector *a, Vector *b) {
/*  Adds the components of two vectors together to construct a third vector.

    Inputs: a - The first vector to add.
            b - The second vector to add.

    Outputs: A new third vector constructed by performing a + b, or NULL if an error occurred.
*/

    Vector *sum;
    unsigned int i;

    if (!_assertVectorDimensionsEqual(a, b))
        return NULL;

    if ((sum = _vectorNew(a->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < sum->dimensions; i++)
        Vector_SetValue(sum, i, Vector_GetValue(a, i) + Vector_GetValue(b, i));

    return sum;
}


Vector *_vectorSub(Vector *a, Vector *b) {
/*  Subtracts the components of two vectors together to construct a third vector.

    Inputs: a - The first vector to subtract from.
            b - The second vector to use to subtract from a.

    Outputs: A new third vector constructed by performing a - b, or NULL if an error occurred.
*/

    Vector *difference;
    unsigned int i;

    if (!_assertVectorDimensionsEqual(a, b))
        return NULL;

    if ((difference = _vectorNew(a->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < difference->dimensions; i++)
        Vector_SetValue(difference, i, Vector_GetValue(a, i) - Vector_GetValue(b, i));

    return difference;
}


Vector *_vectorMul(Vector *self, VECTOR_TYPE multiplier) {
/*  Multiplies the components of a vector by a scalar to construct a new vector.

    Inputs: self       - The vector to be multiplied by multiplier.
            multiplier - The scalar to multiply self by.

    Outputs: A new Vector object constructed by performing self * multiplier, or NULL if an error occurred.
*/

    Vector *product;
    unsigned int i;

    if ((product = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(product, i, Vector_GetValue(self, i) * multiplier);

    return product;
}


Vector *_vectorDiv(Vector *self, VECTOR_TYPE divisor) {
/*  Divides the components of a vector by a scalar to construct a new vector.

    Inputs: self    - The vector to be divided by divisor.
            divisor - The scalar to divide self by.

    Outputs: A new Vector object constructed by performing self / divisor or NULL if an error occurred.
*/

    Vector *quotient;
    unsigned int i;

    if (divisor == 0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "Vector division by zero.");
        return NULL;
    }

    if ((quotient = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(quotient, i, Vector_GetValue(self, i) / divisor);

    return quotient;
}


Vector *_vectorNeg(Vector *self) {
/*  Negates the components of a vector to construct a new vector.

    Inputs: self - The vector to negate.

    Outputs: A new Vector object constructed by negating the components of self.
*/

    Vector *neg;
    unsigned int i;

    if ((neg = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(neg, i, -Vector_GetValue(self, i));

    return neg;
}


unsigned char _vectorsEqual(Vector *a, Vector *b) {
/*  Tests if the components of two vectors are equal.

    Inputs: a - The first vector to compare with.
            b - The second vector to compare with.

    Outputs: 1 If the components in a and b are pairwise equal, else 0.
*/

    unsigned int i;

    if (a->dimensions != b->dimensions)
        return 0;

    for (i = 0; i < a->dimensions; i++) {
        if (Vector_GetValue(a, i) != Vector_GetValue(b, i))
            return 0;
    }

    return 1;
}
