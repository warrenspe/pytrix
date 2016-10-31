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

PyObject *matrixStr(Matrix *self) {
/*  Constructs a human-readable Python String representing a human readable version of this matrix.

    Inputs: self - The matrix for which the human readable format will be produced.

    Outputs: A PyString containing the human readable form of the matrix.
*/

    unsigned int i;
    const char *cNewline = "\n\00";
    PyObject *matrixRepr,
             *newlineStr,
             *vectorReprs,
             *tuple,
             *list,
             *listRepr;

    // Create a list of all the vector reprs
    if ((vectorReprs = PyList_New(self->rows)) == NULL)
        return NULL;

    for (i = 0; i < self->rows; i++) {
        if ((tuple = _vectorToTuple(Matrix_GetVector(self, i))) == NULL) {
            Py_DECREF(vectorReprs);
            return NULL;
        }
        if ((list = PySequence_List(tuple)) == NULL) {
            Py_DECREF(vectorReprs);
            Py_DECREF(tuple);
            return NULL;
        }

        if ((listRepr = PyObject_Repr(list)) == NULL) {
            Py_DECREF(vectorReprs);
            Py_DECREF(tuple);
            Py_DECREF(list);
        }

        PyList_SET_ITEM(vectorReprs, i, listRepr);
        Py_DECREF(tuple);
        Py_DECREF(list);
    }

    // Create a newline-delimited version of all the vector reprs joined
    if ((newlineStr = PyBytes_FromString(cNewline)) == NULL || PyErr_Occurred() != NULL) {
        Py_DECREF(vectorReprs);
        return NULL;
    }

    if ((matrixRepr = PyObject_CallMethod(newlineStr, "join", "O", vectorReprs)) == NULL) {
        Py_DECREF(newlineStr);
        Py_DECREF(vectorReprs);
        return NULL;
    }

    Py_DECREF(newlineStr);
    Py_DECREF(vectorReprs);
    return matrixRepr; // TODO figure out refcounts, leave nothing alive
}
