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

VECTOR_TYPE PyNumber_AS_VECTOR_TYPE(PyObject *);
PyObject *PyNumber_FROM_VECTOR_TYPE(VECTOR_TYPE);


VECTOR_TYPE PyNumber_AS_VECTOR_TYPE(PyObject *n) {
/*  Converts a PyObject containing any type of number to a VECTOR_TYPE.

    Inputs: n - The PyObject to convert.

    Outputs: A VECTOR_TYPE.
*/

    PyObject *pyFloat;
    VECTOR_TYPE retval;

    pyFloat = PyNumber_Float(n);
    retval = PyFloat_AS_DOUBLE(pyFloat);
    Py_DECREF(pyFloat);

    return retval;
}


PyObject *PyNumber_FROM_VECTOR_TYPE(VECTOR_TYPE v) {
/*  Converts a VECTOR_TYPE to a PyObject.

    Inputs: v - The VECTOR_TYPE to convert.

    Outputs: A PyObject *.
*/

    return PyFloat_FromDouble(v);
}
