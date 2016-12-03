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

Point *_pointNew(unsigned int dimensions) {
/*  Creates a new point objects with the given number of dimensions.

    Inputs: dimensions - The number of dimensions to create the new point object with.

    Outputs: A pointer to a Point object, or NULL if an error occurred..
*/

    Point *newPoint;
    VECTOR_TYPE *data;

    if ((data = PyMem_New(VECTOR_TYPE, dimensions)) == NULL)
        return NULL;

    if ((newPoint = PyObject_New(Point, &PointType)) == NULL) {
        PyMem_Free(data);
        return NULL;
    }

    newPoint->dimensions = dimensions;
    newPoint->data = data;

    return newPoint;
}


void _pointCopyData(Point *in, Point *out) {
/*  Copies the data from in to out.
    Assumes that in.dimensions == out.dimensions.

    Inputs: in  - A pointer to the Point to copy the data from.
            out - A pointer to the Point to copy the data to.
*/

    memcpy(out->data, in->data, sizeof(VECTOR_TYPE) * out->dimensions);
}


Point *_pointCopy(Point *p) {
/*  Creates a new copy of the given point.

    Inputs: p - The point to copy.

    Outputs: A Point object with the same number of dimensions and data as p, or NULL if an exception occurred.
*/

    Point *copy = _pointNew(p->dimensions);

    if (copy == NULL)
        return NULL;

    _pointCopyData(p, copy);

    return copy;
}


unsigned char _assertPointDimensionsEqual(Point *a, Point *b) {
/*  Asserts that two points' dimensions are equal.  Sets a PyError if they are not.

    Inputs: a - A pointer to the first Point.
            b - A pointer to the second Point.

    Outputs: 1 if the two points have the same number of dimensions, else 0.
*/

    if (a->dimensions != b->dimensions) {
        PyErr_SetString(PyExc_ValueError, "Points must be of the same dimensions.");
        return 0;
    }

    return 1;
}


unsigned char _assertPointVectorDimensionsEqual(Point *a, Vector *b) {
/*  Asserts that a point and vector have equal dimensions.  Sets a PyError if they do not.

    Inputs: a - A pointer to the first Point.
            b - A vector to the second Point.

    Outputs: 1 if they have the same number of dimensions, else 0.
*/

    if (a->dimensions != b->dimensions) {
        PyErr_SetString(PyExc_ValueError, "Point/Vector must be of the same dimensions.");
        return 0;
    }

    return 1;
}


unsigned char _assertPoint(PyObject *toCheck) {
/*  Asserts that the given PyObject is a Point object.

    Inputs: toCheck - A PyObject to test if whether or not it is a Point.

    Outputs: 1 if toCheck is a Point, else 0.
*/

    if (!Point_Check(toCheck)) {
        PyErr_Format(PyExc_TypeError, "Given object is not a Point: \"%.400s\"", Py_TYPE(toCheck)->tp_name);
        return 0;
    }

    return 1;
}


PyObject *_pointToTuple(Point *self) {
/*  Constructs a PyTuple from the given point where the i-th index of the tuple contains the i-th dimension component
    of the point.

    Inputs: self - The point to convert to a PyTuple.

    Outputs: A PyTuple corresponding to self if successful, else sets a PyError and returns NULL.
*/

    PyObject *tuple;
    unsigned int i;
    PyObject *item;

    if ((tuple = PyTuple_New(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++) {
        item = PyNumber_FROM_VECTOR_TYPE(Point_GetValue(self, i));

        if (item == NULL) {
            Py_DECREF(tuple);
            return NULL;
        }

        PyTuple_SET_ITEM(tuple, i, item);
    }

    return tuple;
}


Point *_pointAdd(Point *a, Vector *b) {
/*  Adds the components of a point and vector together to construct a new point.

    Inputs: a - The point to add.
            b - The vector to add.

    Outputs: A new point constructed by performing a + b, or NULL if an error occurred.
*/

    Point *sum;
    unsigned int i;

    if (!_assertPointVectorDimensionsEqual(a, b))
        return NULL;

    if ((sum = _pointNew(a->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < sum->dimensions; i++)
        Point_SetValue(sum, i, Point_GetValue(a, i) + Vector_GetValue(b, i));

    return sum;
}


Point *_pointVectorSub(Point *a, Vector *b) {
/*  Subtracts the components of a point from a vector to construct a new point.

    Inputs: a - The point to subtract from.
            b - The vector to use to subtract from a.

    Outputs: A new point constructed by performing a - b, or NULL if an error occurred.
*/

    Point *difference;
    unsigned int i;

    if (!_assertPointVectorDimensionsEqual(a, b))
        return NULL;

    if ((difference = _pointNew(a->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < difference->dimensions; i++)
        Point_SetValue(difference, i, Point_GetValue(a, i) - Vector_GetValue(b, i));

    return difference;
}


Vector *_pointPointSub(Point *a, Point *b) {
/*  Calculates a Vector which maps point A to point B.

    Inputs: a - The point to start from.
            b - The point to end up at.

    Outputs: A new Vector constructed by performing a - b, or NULL if an error occurred.
*/

    Vector *difference;
    unsigned int i;

    if (!_assertPointDimensionsEqual(a, b))
        return NULL;

    if ((difference = _vectorNew(a->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < difference->dimensions; i++)
        Vector_SetValue(difference, i, Point_GetValue(a, i) - Point_GetValue(b, i));

    return difference;
}


Point *_pointNeg(Point *self) {
/*  Negates the components of a point to construct a new point.

    Inputs: self - The point to negate.

    Outputs: A new Point object constructed by negating the components of self.
*/

    Point *neg;
    unsigned int i;

    if ((neg = _pointNew(self->dimensions)) == NULL)
        return NULL;

    for (i = 0; i < self->dimensions; i++)
        Point_SetValue(neg, i, -Point_GetValue(self, i));

    return neg;
}


unsigned char _pointsEqual(Point *a, Point *b) {
/*  Tests if the components of two points are equal.

    Inputs: a - The first point to compare with.
            b - The second point to compare with.

    Outputs: 1 If the components in a and b are pairwise equal, else 0.
*/

    unsigned int i;

    if (a->dimensions != b->dimensions)
        return 0;

    for (i = 0; i < a->dimensions; i++) {
        if (Point_GetValue(a, i) != Point_GetValue(b, i))
            return 0;
    }

    return 1;
}
