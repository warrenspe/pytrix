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

PyObject *vectorIter(Vector *);

PyObject *vectorIter(Vector *v) {
    VectorIter *i = PyObject_GC_New(VectorIter, &VectorIterType);
    if (i == NULL)
        return NULL;

    i->iterating = (PyObject *)v;
    i->i = 0;
    PyObject_GC_Track(i);
    Py_INCREF(i);
    return (PyObject *)i;
}
