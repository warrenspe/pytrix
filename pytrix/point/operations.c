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

PyObject *pointStr(Point *self) {
/*  Constructs a human-readable Python String representing a human readable version of this point.

    Inputs: self - The point for which the human readable format will be produced.

    Outputs: A PyString containing the human readable form of the point.
*/

    PyObject *tuple,
             *tupleRepr;

    if ((tuple = _pointToTuple(self)) == NULL)
        return NULL;

    tupleRepr = PyObject_Repr(tuple);
    Py_DECREF(tuple);

    return tupleRepr;
}


PyObject *pointCopy(Point *self) {
/*  Creates a copy of this point object.

    Inputs: self - The point to copy.

    Outputs: A copy of self.
*/

    return (PyObject *)_pointCopy(self);
}


PyObject *pointAdd(PyObject *a, PyObject *b) {
/*  Adds the components of a point together with a vector to create a new point.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly either the point or vector to add.
            b - Supposedly either the point or vector to add.

    Outputs: A new point constructed by performing a + b, or NULL if an exception occurred.
*/

    if (Point_Check(a)) {
        if (Vector_Check(b))
            return (PyObject *)_pointAdd((Point *)a, (Vector *)b);

    } else if (Vector_Check(a)) {
        if (Point_Check(b))
            return (PyObject *)_pointAdd((Point *)b, (Vector *)a);

    }

    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
}


PyObject *pointSub(PyObject *a, PyObject *b) {
/*  Subtracts the components of a vector from a point to create a new third point.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly the point to be subtracted from b.
            b - Supposedly the vector to subtract from a.

    Outputs: A new point constructed by performing a - b, or NULL if an exception occurred.
*/

    if (!(Point_Check(a) && Vector_Check(b))) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return (PyObject *)_pointSub((Point *)a, (Vector *)b);
}


PyObject *pointMul(PyObject *a, PyObject *b) {
/*  Multiplies each of the components of a point by a scalar or a matrix to create a new point.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.
    We expect one of a or b to be a Point object, and the other object to be a python object implementing the
    numerical protocol, or a Matrix.  They can come in either order if it is a scalar, if we're multiplying with a
    matrix, the first argument will be the point and the second the matrix.

    Inputs: a - Supposedly either the point to multiply by b, or the scalar to multiply b with.
            b - Supposedly either the point to multiply by a, or the scalar to multiply a with.

    Outputs: A new point constructed by performing a * b, or NULL if an exception occurred.
*/

    Point *p;
    VECTOR_TYPE multiplier;

    // Assert that the non point argument is a number
    if (PyNumber_Check(a)) {
        multiplier = PyNumber_AS_VECTOR_TYPE(a);
        p = (Point *)b;
    } else if (PyNumber_Check(b)) {
        multiplier = PyNumber_AS_VECTOR_TYPE(b);
        p = (Point *)a;
    } else if (Matrix_Check(b)) {
        return (PyObject *)_pointMatrixMul(a, b);

    } else {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    if (PyErr_Occurred() != NULL)
        return NULL;

    return (PyObject *)_pointMul(p, multiplier);
}


PyObject *pointDiv(PyObject *a, PyObject *b) {
/*  Divides each of the components of a point by a scalar to create a new point.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.
    We expect a to be the point object being divided, and the other object to be a python object implementing the
    numerical protocol.

    Inputs: a - Supposedly either the point to divide by b.
            b - Supposedly the scalar to divide a by.

    Outputs: A new point constructed by performing a / b, or NULL if an exception occurred.
*/

    VECTOR_TYPE divider;

    // Assert that the dividend is a point and the divisor is a scalar
    if (!Point_Check(a)) {
        PyErr_SetString(PyExc_TypeError, "A point cannot be used to divide an object.");
        return NULL;
    } else if (!PyNumber_Check(b)) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    divider = PyNumber_AS_VECTOR_TYPE(b);

    if (PyErr_Occurred() != NULL)
        return NULL;

    return (PyObject *)_pointDiv((Point *)a, divider);
}


PyObject *pointNeg(Point *self) {
/*  Negates the components of a point to create a new point.

    Inputs: self - The point to negate.

    Outputs: A new point with negated components from self.
*/

    return (PyObject *)_pointNeg(self);
}


PyObject *pointRichCmp(PyObject *a, PyObject *b, int op) {
/*  Performs (in)equality checking for points.
    Note that both arguments must be sanitized, as any python object may be passed.  We expect both to be points.

    Inputs: a  - The first point to compare.
            b  - The second point to compare.
            op - An operation code.  See https://docs.python.org/3/c-api/typeobj.html#c.PyTypeObject.tp_richcompare

    Outputs: A PyTrue or PyFalse depending which op code was given and the values of a and b.
*/

    if ((!_assertPoint(a)) || (!_assertPoint(b)))
        return NULL;

    switch (op) {
        case Py_EQ:
            if (_pointsEqual((Point *)a, (Point *)b))
                Py_RETURN_TRUE;
            Py_RETURN_FALSE;

        case Py_NE:
            if (_pointsEqual((Point *)a, (Point *)b))
                Py_RETURN_FALSE;
            Py_RETURN_TRUE;

        default:
            Py_INCREF(Py_NotImplemented);
            return Py_NotImplemented;
    }
}


int pointTrue(Point *self) {
/*  Determines if a point has a magnitude of 0 or not.

    Inputs: self - The point to check.

    Outputs: 1 / 0 based on whether or not self contains all 0's  or not.
*/

    unsigned int i;

    for (i = 0; i < self->dimensions; i++) {
        if (Point_GetValue(self, i) != 0)
            return 1;
    }

    return 0;
}


PyObject *pointItem(PyObject *self, Py_ssize_t i) {
/*  Returns the ith item in the point.

    Inputs: self - The Point which the item should be pulled from.
            i    - The item in the ith dimension of self will be returned.

    Outputs: A PyObject containing the value of the item in the ith slot of self.
*/

    Point *p = (Point *)self;
    unsigned int idx = (unsigned int)i;

    if (idx >= p->dimensions) {
        PyErr_Format(PyExc_IndexError, "Cannot return item in index %x of Point with %x dimensions.", idx, p->dimensions);
        return NULL;
    }

    return PyNumber_FROM_VECTOR_TYPE(Point_GetValue(p, idx));
}
