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
PyObject *vectorTranspose(Vector *);
PyObject *vectorCopy(Vector *);

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

    // Ensure the two vectors are of the same dimensions
    if (self->dimensions != other->dimensions) {
        PyErr_SetString(PyExc_ValueError, "Vectors must be of the same length in order to perform dot product.");
        return 0;
    }
    if (self->dimensions == 3) {
        a = Vector_GetValue(self, 2) * Vector_GetValue(other, 3) - Vector_GetValue(self, 3) * Vector_GetValue(other, 2);
        b = Vector_GetValue(self, 3) * Vector_GetValue(other, 1) - Vector_GetValue(self, 1) * Vector_GetValue(other, 3);
        c = Vector_GetValue(self, 1) * Vector_GetValue(other, 2) - Vector_GetValue(self, 2) * Vector_GetValue(other, 1);
        if ((cross = _vectorNew(3, self->height, self->width)) == NULL)
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

    VECTOR_TYPE selfLength = _vectorLength(self),
                otherLength = _vectorLength(other),
                dotProd = _vectorDot(self, other);

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

    if ((unit = _vectorNew(self->dimensions, self->height, self->width)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Vector_SetValue(unit, i, Vector_GetValue(self, i) / length);

    return (PyObject *)unit;
}

PyObject *vectorIsUnit(Vector *self) {
    if (_vectorLength(self) == 1)
        return Py_True;
    return Py_False;
}

PyObject *vectorTranspose(Vector *self) {
    Vector *copy = _vectorCopy(self);

    copy->height = self->width;
    copy->width = self->height;

    return (PyObject *)copy;
}

PyObject *vectorCopy(Vector *self) {
    return (PyObject *)_vectorCopy(self);
}
