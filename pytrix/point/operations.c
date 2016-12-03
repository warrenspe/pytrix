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
/*  Subtracts the components of a vector from a point to create a new third point. Alternatively, takes two points
    and constructs a vector which maps from the first to the second.
    Note that both arguments must be sanitized, as any Python object may be passed in either slot.

    Inputs: a - Supposedly the point to be subtracted from b.
            b - Supposedly the vector or another point to subtract from a.

    Outputs: If one of a, b is a vector: A new point constructed by performing a - b, or NULL if an exception occurred.
             If a & b are points: A new vector constructed by performing a - b, or NULL if an exception occurred.
*/

    if (Point_Check(a)) {
        if (Vector_Check(b)) {
            return (PyObject *)_pointVectorSub((Point *)a, (Vector *)b);

        } else if (Point_Check(b)) {
            return (PyObject *)_pointPointSub((Point *)a, (Point *)b);
        }
    }

    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
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
