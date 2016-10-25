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

PyObject *vectorDot(Vector *, Vector *);
PyObject *vectorCross(Vector *, Vector *);
PyObject *vectorLength(Vector *);
PyObject *vectorAngleBetween(Vector *, Vector *);
PyObject *vectorUnit(Vector *);
PyObject *vectorIsUnit(Vector *);
PyObject *vectorCopy(Vector *);
PyObject *vectorAdd(PyObject *, PyObject *);
PyObject *vectorSub(PyObject *, PyObject *);
PyObject *vectorMul(PyObject *, PyObject *);
PyObject *vectorDiv(PyObject *, PyObject *);
PyObject *vectorNeg(Vector *);
PyObject *vectorRichCmp(PyObject *, PyObject *, int);
int vectorTrue(Vector *);

PyObject *vectorDot(Vector *self, Vector *other) {
    VECTOR_TYPE product = _vectorDot(self, other);
    if (PyErr_Occurred() == NULL)
        return PyFloat_FromDouble(product);

    return NULL;
}


PyObject *vectorCross(Vector *self, Vector *other) {
    Vector *cross;
    VECTOR_TYPE a,
                b,
                c;

    if (!_assertVectorDimensionsEqual(self, other))
        return NULL;

    if (self->dimensions == 3) {
        a = Vector_GetValue(self, 2) * Vector_GetValue(other, 3) - Vector_GetValue(self, 3) * Vector_GetValue(other, 2);
        b = Vector_GetValue(self, 3) * Vector_GetValue(other, 1) - Vector_GetValue(self, 1) * Vector_GetValue(other, 3);
        c = Vector_GetValue(self, 1) * Vector_GetValue(other, 2) - Vector_GetValue(self, 2) * Vector_GetValue(other, 1);
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
    return PyFloat_FromDouble(_vectorLength(self));
}


PyObject *vectorAngleBetween(Vector *self, Vector *other) {
/*  Calculates the angle between two vectors in radians.

    Inputs: self  - The first vector.
            other - The second vector.

    Outputs: A float containing the angle between the vectors.
*/

    VECTOR_TYPE dotProd = _vectorDot(self, other),
                otherLength = _vectorLength(other),
                selfLength = _vectorLength(self);

    return PyFloat_FromDouble(acos((dotProd / (selfLength * otherLength))));
}


PyObject *vectorUnit(Vector *self) {
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
    if (_vectorLength(self) == 1)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}


PyObject *vectorCopy(Vector *self) {
    return (PyObject *)_vectorCopy(self);
}


PyObject *vectorAdd(PyObject *a, PyObject *b) {
    if ((!_assertVector(a)) || (!_assertVector(b)))
        return NULL;

    return (PyObject *)_vectorAdd((Vector *)a, (Vector *)b);
}


PyObject *vectorSub(PyObject *a, PyObject *b) {
    if ((!_assertVector(a)) || (!_assertVector(b)))
        return NULL;

    return (PyObject *)_vectorSub((Vector *)a, (Vector *)b);
}


PyObject *vectorMul(PyObject *a, PyObject *b) {
    Vector *v;
    VECTOR_TYPE multiplier;

    // Assert that the non vector argument is a number
    if (PyNumber_Check(a)) {
        multiplier = PyFloat_AsDouble(a);
        v = (Vector *)b;
    } else if (PyNumber_Check(b)) {
        multiplier = PyFloat_AsDouble(b);
        v = (Vector *)a;
    } else {
        PyErr_SetString(PyExc_TypeError, "Vector can only be multiplied by a scalar.");
        return NULL;
    }

    if (PyErr_Occurred() != NULL)
        return NULL;

    return (PyObject *)_vectorMul(v, multiplier);
}


PyObject *vectorDiv(PyObject *a, PyObject *b) {
    VECTOR_TYPE divider;

    // Assert that the dividend is a vector and the divisor is a scalar
    if (!Vector_Check(a)) {
        PyErr_SetString(PyExc_TypeError, "A vector cannot be used to divide an object.");
        return NULL;
    } else if (!PyNumber_Check(b)) {
        PyErr_SetString(PyExc_TypeError, "Vector can only be divided by a scalar.");
        return NULL;
    }

    divider = PyFloat_AsDouble(b);

    if (PyErr_Occurred() != NULL)
        return NULL;

    return (PyObject *)_vectorMul((Vector *)a, divider);
}


PyObject *vectorNeg(Vector *self) {
    return (PyObject *)_vectorNeg(self);
}


PyObject *vectorRichCmp(PyObject *a, PyObject *b, int op) {
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
/*  Returns a 0 if the given vector has a magnitude of 0, else 1. */

    unsigned int i;

    for (i = 0; i < self->dimensions; i++) {
        if (Vector_GetValue(self, i) != 0)
            return 1;
    }

    return 0;
}
