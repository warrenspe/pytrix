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

PyObject *VectorIter_next(VectorIter *);
int VectorIter_traverse(VectorIter *, visitproc, void *);
void VectorIter_dealloc(VectorIter *);

PyObject *VectorIter_next(VectorIter *self) {
    Vector *iterating = (Vector *)self->iterating;

    if (self->i < iterating->dimensions)
        return PyFloat_FromDouble(Vector_GetValue(iterating, ((self->i)++)));

    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
}


int VectorIter_traverse(VectorIter *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterating);
    return 0;
}


void VectorIter_dealloc(VectorIter *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterating);
    PyObject_GC_Del(self);
}


