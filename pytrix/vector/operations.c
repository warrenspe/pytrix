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

PyObject *vectorStr(Vector *self) {
/*  Constructs a human-readable Python String representing a human readable version of this vector.

    Inputs: self - The vector for which the human readable format will be produced.

    Outputs: A PyString containing the human readable form of the vector.
*/

    PyObject *tuple,
             *tupleRepr;

    if ((tuple = _vectorToTuple(self)) == NULL)
        return NULL;

    tupleRepr = PyObject_Repr(tuple);
    Py_DECREF(tuple);

    return tupleRepr;
}


PyObject *vectorDot(Vector *self, PyObject *other) {
/*  Calculates the dot product of this vector with another of the same dimensions.

    Inputs: self  - The first Vector in the dot product.
            other - Supposedly the other Vector in the dot product.  However - this can be anything, so
                    we have to check that it's a vector before we can go and use it.

    Outputs: A PyFloat containing the value of self * other if successful, else an Exception is raised.
*/

    if (!_assertVector(other))
        return NULL;

    VECTOR_TYPE product = _vectorDot(self, (Vector *)other);
    if (PyErr_Occurred() == NULL)
        return PyNumber_FROM_VECTOR_TYPE(product);

    return NULL;
}


PyObject *vectorCross(Vector *self, PyObject *otherPy) {
/*  Calculates the cross product of this vector with another, both being 3 dimensional.

    Inputs: self  - The first Vector in the cross product.
            other - Supposedly the other Vector in the cross product.  However - this can be anything, so
                    we have to check that it's a vector before we can go and use it.

    Outputs: A Vector containing the value of self x other if successful, else an Exception is raised.
*/

    Vector *cross,
           *other;
    VECTOR_TYPE a,
                b,
                c;

    if (!_assertVector(otherPy))
        return NULL;

    other = (Vector *)otherPy;

    if (!_assertVectorDimensionsEqual(self, other))
        return NULL;

    if (self->dimensions == 3) {
        a = Vector_GetValue(self, 1) * Vector_GetValue(other, 2) - Vector_GetValue(self, 2) * Vector_GetValue(other, 1);
        b = Vector_GetValue(self, 2) * Vector_GetValue(other, 0) - Vector_GetValue(self, 0) * Vector_GetValue(other, 2);
        c = Vector_GetValue(self, 0) * Vector_GetValue(other, 1) - Vector_GetValue(self, 1) * Vector_GetValue(other, 0);
        if ((cross = _vectorNew(3)) == NULL)
            return NULL;
        Vector_SetValue(cross, 0, a);
        Vector_SetValue(cross, 1, b);
        Vector_SetValue(cross, 2, c);
        return (PyObject *)cross;

    } else
        PyErr_SetString(PyExc_ValueError, "Cross product can only be computed on vectors of 3 dimensions.");

    return NULL;
}


PyObject *vectorLength(Vector *self) {
/*  Calculates the length of a vector.

    Inputs: self - The vector whose length is being calculated.

    Outputs: A PyFloat containing the length of the object, or an Exception.
*/

    return PyNumber_FROM_VECTOR_TYPE(_vectorLength(self));
}


PyObject *vectorAngleBetween(Vector *self, PyObject *other) {
/*  Calculates the angle between two vectors in radians.

    Inputs: self  - The first vector.
            other - Presumably the second vector.  However, this can be any python object so we must validate that it
                    is a vector before we go ahead and use it.

    Outputs: A PyFloat containing the angle between the vectors.
*/

    VECTOR_TYPE dotProd,
                otherLength,
                selfLength;

    if (!_assertVector(other))
        return NULL;

    dotProd = _vectorDot(self, (Vector *)other);
    otherLength = _vectorLength((Vector *)other);
    selfLength = _vectorLength(self);

    return PyNumber_FROM_VECTOR_TYPE(acos((dotProd / (selfLength * otherLength))));
}


PyObject *vectorUnit(Vector *self) {
/*  Creates a new unit vector (vector of length 1) from the given vector.

    Inputs: self - The vector to construct the unit vector from.

    Outputs: A Vector object of length 1 of the same direction as self.
*/

    unsigned int i;
    Vector *unit;
    VECTOR_TYPE length = _vectorLength(self);

    if (length == 0) {
        PyErr_SetString(PyExc_ValueError, "Cannot determine unit vector of vector with magnitude 0.");
        return NULL;
    }

    if ((unit = _vectorNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(unit, i, Vector_GetValue(self, i) / length);

    return (PyObject *)unit;
}


PyObject *vectorIsUnit(Vector *self) {
/*  Determines if a vector is a unit vector (has a length of 1).

    Inputs: self - The vector to determine if is a unit vector.

    Outputs: A PyTrue or PyFalse based on the length of self.

    Note: Because floating point values have slight amounts of error in them, allow a 10^-10 margin of error
*/

    if (fabs(_vectorLength(self) - 1) < .00000000001)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}


PyObject *vectorCopy(Vector *self) {
/*  Creates a copy of this vector object.

    Inputs: self - The vector to copy.

    Outputs: A copy of self.
*/

    return (PyObject *)_vectorCopy(self);
}


PyObject *vectorAdd(PyObject *a, PyObject *b) {
/*  Adds the components of two vectors together to create a new third vector.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly the first vector to add.
            b - Supposedly the second vector to add.

    Outputs: A new vector constructed by performing a + b, or NULL if an exception occurred.
*/

    if (!(Vector_Check(a) && Vector_Check(b))) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return (PyObject *)_vectorAdd((Vector *)a, (Vector *)b);
}


PyObject *vectorSub(PyObject *a, PyObject *b) {
/*  Subtracts the components of two vectors to create a new third vector.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly the first vector to subtract from.
            b - Supposedly the second vector to use to subtract from a.

    Outputs: A new vector constructed by performing a - b, or NULL if an exception occurred.
*/

    if (!(Vector_Check(a) && Vector_Check(b))) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return (PyObject *)_vectorSub((Vector *)a, (Vector *)b);
}


PyObject *vectorMul(PyObject *a, PyObject *b) {
/*  Multiplies each of the components of a vector by a scalar or a matrix to create a new vector.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.
    We expect one of a or b to be a Vector object, and the other object to be a python object implementing the
    numerical protocol, or a Matrix.  They can come in either order if it is a scalar, if we're multiplying with a
    matrix, the first argument will be the vector and the second the matrix.

    Inputs: a - Supposedly either the vector to multiply by b, or the scalar to multiply b with.
            b - Supposedly either the vector to multiply by a, or the scalar to multiply a with.

    Outputs: A new vector constructed by performing a * b, or NULL if an exception occurred.
*/

    Vector *v;
    VECTOR_TYPE multiplier;

    // Assert that the non vector argument is a number
    if (PyNumber_Check(a)) {
        multiplier = PyNumber_AS_VECTOR_TYPE(a);
        v = (Vector *)b;
    } else if (PyNumber_Check(b)) {
        multiplier = PyNumber_AS_VECTOR_TYPE(b);
        v = (Vector *)a;
    } else if (Matrix_Check(b)) {
        return (PyObject *)_vectorMatrixMul(a, b);

    } else {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    if (PyErr_Occurred() != NULL)
        return NULL;

    return (PyObject *)_vectorMul(v, multiplier);
}


PyObject *vectorDiv(PyObject *a, PyObject *b) {
/*  Divides each of the components of a vector by a scalar to create a new vector.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.
    We expect a to be the vector object being divided, and the other object to be a python object implementing the
    numerical protocol.

    Inputs: a - Supposedly either the vector to divide by b.
            b - Supposedly the scalar to divide a by.

    Outputs: A new vector constructed by performing a / b, or NULL if an exception occurred.
*/

    VECTOR_TYPE divider;

    // Assert that the dividend is a vector and the divisor is a scalar
    if (!Vector_Check(a)) {
        PyErr_SetString(PyExc_TypeError, "A vector cannot be used to divide an object.");
        return NULL;
    } else if (!PyNumber_Check(b)) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    divider = PyNumber_AS_VECTOR_TYPE(b);

    if (PyErr_Occurred() != NULL)
        return NULL;

    return (PyObject *)_vectorDiv((Vector *)a, divider);
}


PyObject *vectorNeg(Vector *self) {
/*  Negates the components of a vector to create a new vector.

    Inputs: self - The vector to negate.

    Outputs: A new vector with negated components from self.
*/

    return (PyObject *)_vectorNeg(self);
}


PyObject *vectorOrthogonal(Vector *self, PyObject *other) {
/*  Determines if two vectors are orthogonal.

    Inputs: self  - The first vector.
            other - Presumably the second vector.  However, this can be any python object so we must validate that it
                    is a vector before we go ahead and use it.

    Outputs: A PyTrue or PyFalse depending on whether or not self and other are orthogonal.
*/

    int product;

    if (!_assertVector(other))
        return NULL;

    product = _vectorDot(self, (Vector *)other);

    if (PyErr_Occurred())
        return NULL;

    if (product == 0)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}


PyObject *vectorRichCmp(PyObject *a, PyObject *b, int op) {
/*  Performs (in)equality checking for vectors.
    Note that both arguments must be sanitized, as any python object may be passed.  We expect both to be vectors.

    Inputs: a  - The first vector to compare.
            b  - The second vector to compare.
            op - An operation code.  See https://docs.python.org/3/c-api/typeobj.html#c.PyTypeObject.tp_richcompare

    Outputs: A PyTrue or PyFalse depending which op code was given and the values of a and b.
*/

    if ((!_assertVector(a)) || (!_assertVector(b)))
        return NULL;

    switch (op) {
        case Py_EQ:
            if (_vectorsEqual((Vector *)a, (Vector *)b))
                Py_RETURN_TRUE;
            Py_RETURN_FALSE;

        case Py_NE:
            if (_vectorsEqual((Vector *)a, (Vector *)b))
                Py_RETURN_FALSE;
            Py_RETURN_TRUE;

        default:
            Py_INCREF(Py_NotImplemented);
            return Py_NotImplemented;
    }
}


int vectorTrue(Vector *self) {
/*  Determines if a vector has a magnitude of 0 or not.

    Inputs: self - The vector to check.

    Outputs: 1 / 0 based on whether or not self is a 0 magnitude vector or not.
*/

    unsigned int i;

    for (i = 0; i < self->dimensions; i++) {
        if (Vector_GetValue(self, i) != 0)
            return 1;
    }

    return 0;
}


PyObject *vectorItem(PyObject *self, Py_ssize_t i) {
/*  Returns the ith item in the vector.

    Inputs: self - The Vector which the item should be pulled from.
            i    - The item in the ith dimension of self will be returned.

    Outputs: A PyObject containing the value of the item in the ith slot of self.
*/

    Vector *v = (Vector *)self;
    unsigned int idx = (unsigned int)i;

    if (idx >= v->dimensions) {
        PyErr_Format(PyExc_IndexError, "Cannot return item in index %x of Vector with %x dimensions.", idx, v->dimensions);
        return NULL;
    }

    return PyNumber_FROM_VECTOR_TYPE(Vector_GetValue(v, idx));
}
