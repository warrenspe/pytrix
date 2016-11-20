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

PyObject *identityMatrix(PyObject *, PyObject *);
PyObject *rotation2DMatrix(PyObject *, PyObject *);
PyObject *rotation3DMatrix(PyObject *, PyObject *);
PyObject *permutationMatrix(PyObject *, PyObject *);


PyObject *identityMatrix(PyObject *self, PyObject *args) {
/*  Convenience function for constructing identity matrices of particular dimensions.

    Inputs: self - A pointer to the pytrix module.
            args - A PyTuple containing a PyNumber of the number of dimensions to initialize the identity matrix with.

    Outputs: An identity Matrix of the specified dimensions.
*/

    Matrix *identity;
    long dimensions;

    if (!PyArg_ParseTuple(args, "l", &dimensions))
        return NULL;

    if (dimensions < 0) {
        PyErr_SetString(PyExc_ValueError, "Dimensions cannot be less than 0.");
        return NULL;
    }

    if (PyErr_Occurred())
        return NULL;

    if ((identity = _matrixNew(dimensions, dimensions)) == NULL)
        return NULL;

    if (!_matrixInitIdentity(identity, 1)) {
        Py_DECREF(identity);
        return NULL;
    }

    return (PyObject *)identity;
}


PyObject *rotation2DMatrix(PyObject *self, PyObject *args) {
/*  Convenience function for constructing a rotation matrix to rotate a vector by a specified number of radians.

    Inputs: self - A pointer to the pytrix module.
            args - A PyTuple containing a PyNumber containing the radians to rotate the matrix.

    Outputs: A rotation Matrix with the specified rotation properties.
*/

    Matrix *rotation;
    VECTOR_TYPE radians;

    if (!PyArg_ParseTuple(args, "d", &radians))
        return NULL;

    if ((rotation = _matrixNew(2, 2)) == NULL)
        return NULL;

    Matrix_SetValue(rotation, 0, 0, cos(radians));
    Matrix_SetValue(rotation, 0, 1, -sin(radians));
    Matrix_SetValue(rotation, 1, 0, sin(radians));
    Matrix_SetValue(rotation, 1, 1, cos(radians));

    return (PyObject *)rotation;
}


PyObject *rotation3DMatrix(PyObject *self, PyObject *args) {
/*  Convenience function for constructing a rotation matrix to rotate a vector by a specified number of radians.

    Inputs: self - A pointer to the pytrix module.
            args - A PyTuple containing three PyNumbers for the x, y, and z rotation radians.

    Outputs: A rotation Matrix with the specified rotation properties.
*/

    Matrix *xRotation = NULL,
           *yRotation = NULL,
           *zRotation = NULL,
           *rotation;
    VECTOR_TYPE xRadians,
                yRadians,
                zRadians;

    if (!PyArg_ParseTuple(args, "ddd", &xRadians, &yRadians, &zRadians))
        return NULL;

    if ((rotation = _matrixNew(3, 3)) == NULL)
        return NULL;
    if (!_matrixInitIdentity(rotation, 1)) {
        Py_DECREF(rotation);
        return NULL;
    }

    if (xRadians != 0) {
        if ((xRotation = _matrixNew(3, 3)) == NULL) {
            Py_DECREF(rotation);
            return NULL;
        }
        Matrix_SetValue(xRotation, 0, 0, 1);
        Matrix_SetValue(xRotation, 0, 1, 0);
        Matrix_SetValue(xRotation, 0, 2, 0);
        Matrix_SetValue(xRotation, 1, 0, 0);
        Matrix_SetValue(xRotation, 1, 1, cos(xRadians));
        Matrix_SetValue(xRotation, 1, 2, -sin(xRadians));
        Matrix_SetValue(xRotation, 2, 0, 0);
        Matrix_SetValue(xRotation, 2, 1, sin(xRadians));
        Matrix_SetValue(xRotation, 2, 2, cos(xRadians));
        if (!_inplaceMatrixMatrixMul(xRotation, rotation, rotation)) {
            Py_DECREF(rotation);
            Py_DECREF(xRotation);
            return NULL;
        }
        Py_DECREF(xRotation);
    }
    if (yRadians != 0) {
        if ((yRotation = _matrixNew(3, 3)) == NULL) {
            Py_DECREF(rotation);
            Py_XDECREF(xRotation);
            return NULL;
        }
        Matrix_SetValue(yRotation, 0, 0, cos(yRadians));
        Matrix_SetValue(yRotation, 0, 1, 0);
        Matrix_SetValue(yRotation, 0, 2, sin(yRadians));
        Matrix_SetValue(yRotation, 1, 0, 0);
        Matrix_SetValue(yRotation, 1, 1, 1);
        Matrix_SetValue(yRotation, 1, 2, 0);
        Matrix_SetValue(yRotation, 2, 0, -sin(yRadians));
        Matrix_SetValue(yRotation, 2, 1, 0);
        Matrix_SetValue(yRotation, 2, 2, cos(yRadians));
        if (!_inplaceMatrixMatrixMul(yRotation, rotation, rotation)) {
            Py_DECREF(rotation);
            Py_DECREF(yRotation);
            return NULL;
        }
        Py_DECREF(yRotation);
    }
    if (zRadians != 0) {
        if ((zRotation = _matrixNew(3, 3)) == NULL) {
            Py_DECREF(rotation);
            Py_XDECREF(xRotation);
            Py_XDECREF(yRotation);
            return NULL;
        }
        Matrix_SetValue(zRotation, 0, 0, cos(zRadians));
        Matrix_SetValue(zRotation, 0, 1, -sin(zRadians));
        Matrix_SetValue(zRotation, 0, 2, 0);
        Matrix_SetValue(zRotation, 1, 0, sin(zRadians));
        Matrix_SetValue(zRotation, 1, 1, cos(zRadians));
        Matrix_SetValue(zRotation, 1, 2, 0);
        Matrix_SetValue(zRotation, 2, 0, 0);
        Matrix_SetValue(zRotation, 2, 1, 0);
        Matrix_SetValue(zRotation, 2, 2, 1);
        if (!_inplaceMatrixMatrixMul(zRotation, rotation, rotation)) {
            Py_DECREF(rotation);
            Py_DECREF(zRotation);
            return NULL;
        }
        Py_DECREF(zRotation);
    }

    return (PyObject *)rotation;
}


PyObject *permutationMatrix(PyObject *self, PyObject *args) {
/*  Convenience function for constructing a permutation matrix of a given dimension for swapping particular rows.

    Inputs: self - A pointer to the pytrix module.
            args - A PyTuple containing (dimensions, row1, row2)

    Outputs: A permutation Matrix of the specified dimensions for swapping the specified rows.
*/

    long row1,
         row2,
         dimensions;
    Matrix *permutation;
    Vector *tempRow;

    if (!PyArg_ParseTuple(args, "lll", &dimensions, &row1, &row2))
        return NULL;
    if (row1 < 0 || row2 < 0) {
        PyErr_SetString(PyExc_ValueError, "pytrix.permutation row input cannot be less than 0.");
        return NULL;
    }
    if (dimensions < 2) {
        PyErr_SetString(PyExc_ValueError, "pytrix.permutation dimensions input cannot be less than 2.");
        return NULL;
    }

    if ((permutation = _matrixNew(dimensions, dimensions)) == NULL)
        return NULL;

    if (!_matrixInitIdentity(permutation, 1)) {
        Py_DECREF(permutation);
        return NULL;
    }

    // Permute the matrix as requested
    tempRow = Matrix_GetVector(permutation, row1);
    Matrix_SetVector(permutation, row1, Matrix_GetVector(permutation, row2));
    Matrix_SetVector(permutation, row2, tempRow);

    return (PyObject *)permutation;
}
