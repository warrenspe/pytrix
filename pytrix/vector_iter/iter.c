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

PyObject *VectorIter_next(VectorIter *self) {
/*  Returns the current value pointed to by the given iterator and increments the internal counter.

    Inputs: self - The VectorIter to iterate.

    Outputs: A PyFloat containing the next value pointed to by the iterator.
*/

    Vector *iterating = (Vector *)self->iterating;

    if (iterating != NULL && self->i < iterating->dimensions)
        return PyNumber_FROM_VECTOR_TYPE(Vector_GetValue(iterating, ((self->i)++)));

    // Now that we're done iterating over this object we can remove our reference to it.
    Py_XDECREF(self->iterating);
    self->iterating = NULL;
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
}


int VectorIter_traverse(VectorIter *self, visitproc visit, void *arg) {
/*  Function used by pythons garbage collector to determine cyclic references between objects. */

    Py_VISIT(self->iterating);
    return 0;
}


void VectorIter_dealloc(VectorIter *self) {
/*  Deallocates a VectorIterator. */

    PyObject_GC_UnTrack(self);

    // If we didn't finish iterating over the object, we will still have to remove our reference to it
    Py_XDECREF(self->iterating);
    PyObject_GC_Del(self);
}
