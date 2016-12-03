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


static PyMemberDef VectorMembers[] = {
    {"dimensions", T_UINT, offsetof(Vector, dimensions), READONLY, "Number of dimensions of the vector."},
    {NULL} // Sentinel
};

#if PY_MAJOR_VERSION >= 3
static PyNumberMethods VectorNumberMethods = {
    (binaryfunc)vectorAdd,
    (binaryfunc)vectorSub,
    (binaryfunc)vectorMul,
    0,
    0,
    0,
    (unaryfunc)vectorNeg,
    0,
    0,
    (inquiry)vectorTrue,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    vectorDiv
};
#else
static PyNumberMethods VectorNumberMethods = {
    (binaryfunc)vectorAdd,
    (binaryfunc)vectorSub,
    (binaryfunc)vectorMul,
    (binaryfunc)vectorDiv,
    0,
    0,
    0,
    (unaryfunc)vectorNeg,
    0,
    0,
    (inquiry)vectorTrue,
};
#endif

static PySequenceMethods VectorSequenceMethods = {
    0,                                          /* sq_length */
    0,                                          /* sq_concat */
    0,                                          /* sq_repeat */
    (ssizeargfunc)vectorItem,                   /* sq_item */
    0,                                          /* sq_slice */
    0,                                          /* sq_ass_item */
    0,                                          /* sq_ass_slice */
    0,                                          /* sq_contains */
};

static PyMethodDef VectorMethods[] = {
    {"copy", (PyCFunction)vectorCopy, METH_NOARGS,
        PyDoc_STR("Creates a copy of this vector.")},
    {"dot", (PyCFunction)vectorDot, METH_O,
        PyDoc_STR("Performs a dot product against another vector.")},
    {"cross", (PyCFunction)vectorCross, METH_O,
        PyDoc_STR("Performs a cross product against another vector.")},
    {"length", (PyCFunction)vectorLength, METH_NOARGS,
        PyDoc_STR("Calculates the length of the vector.")},
    {"unit", (PyCFunction)vectorUnit, METH_NOARGS,
        PyDoc_STR("Returns a new vector of length 1 going in the same direction as this vector.")},
    {"isUnit", (PyCFunction)vectorIsUnit, METH_NOARGS,
        PyDoc_STR("Determine if this vector is a unit vector.")},
    {"angleBetween", (PyCFunction)vectorAngleBetween, METH_O,
        PyDoc_STR("Calculate the angle between this vector and another vector.")},
    {"orthogonal", (PyCFunction)vectorOrthogonal, METH_O,
        PyDoc_STR("Determines whether or not another vector is orthogonal to this vector.")},
    {NULL}  // Sentinel
};


/* NOTES:
    * Vectors are immutable; when operations are performed on them new vectors are created.
*/
static PyTypeObject VectorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pytrix.Vector",
    sizeof(Vector),
    0,                         /*tp_itemsize*/
    (destructor)vectorDeInit,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    &VectorNumberMethods,                         /*tp_as_number*/
    &VectorSequenceMethods,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)vectorStr,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /*tp_flags*/
    "Object representing a Vector.",
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    vectorRichCmp,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    (getiterfunc)vectorIter,                     /* tp_iter */
    0,                     /* tp_iternext */
    VectorMethods,             /* tp_methods */
    VectorMembers,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)vectorInit,      /* tp_init */
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
