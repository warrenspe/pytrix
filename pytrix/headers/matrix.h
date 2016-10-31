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

static PyMemberDef MatrixMembers[] = {
    {"rows", T_UINT, offsetof(Matrix, rows), READONLY, "Number of rows in the matrix."},
    {"columns", T_UINT, offsetof(Matrix, columns), READONLY, "Number of columns in the matrix."},
    {NULL} // Sentinel
};

// Matrix Functions
#if PY_MAJOR_VERSION >= 3
static PyNumberMethods MatrixNumberMethods = {
    (binaryfunc)matrixAdd,
    (binaryfunc)matrixSub,
    (binaryfunc)matrixMul,
    0,
    0,
    0,
    (unaryfunc)matrixNeg,
    0,
    0,
    (inquiry)matrixTrue,
};
#else
/*static PyNumberMethods MatrixNumberMethods = {
    (binaryfunc)matrixAdd,
    (binaryfunc)matrixSub,
    (binaryfunc)matrixMul,
    (binaryfunc)matrixDiv,
    0,
    0,
    0,
    (unaryfunc)matrixNeg,
    0,
    0,
    (inquiry)matrixTrue,
};*/
#endif
static PyMethodDef MatrixMethods[] = {
    {NULL}  /* Sentinel */
};


/* NOTES:
    * Matrices are immutable; when operations are performed on them new matrices are created.
*/
static PyTypeObject MatrixType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pytrix.Matrix",
    sizeof(Matrix),
    0,                         /*tp_itemsize*/
    (destructor)matrixDeInit,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,//&MatrixNumberMethods,                         /*tp_as_number*/ // TODO
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)matrixStr,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /*tp_flags*/
    "Object representing a Matrix.",
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,//matrixRichCmp,                     /* tp_richcompare */ // TODO
    0,                     /* tp_weaklistoffset */
    (getiterfunc)matrixIter,                     /* tp_iter */
    0,                     /* tp_iternext */
    MatrixMethods,             /* tp_methods */
    MatrixMembers,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)matrixInit,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

