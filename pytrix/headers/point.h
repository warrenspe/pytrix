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


static PyMemberDef PointMembers[] = {
    {"dimensions", T_UINT, offsetof(Point, dimensions), READONLY, "Number of dimensions of the point."},
    {NULL} // Sentinel
};

#if PY_MAJOR_VERSION >= 3
static PyNumberMethods PointNumberMethods = {
    (binaryfunc)pointAdd,
    (binaryfunc)pointSub,
    0,
    0,
    0,
    0,
    (unaryfunc)pointNeg,
    0,
    0,
    (inquiry)pointTrue,
};
#else
static PyNumberMethods PointNumberMethods = {
    (binaryfunc)pointAdd,
    (binaryfunc)pointSub,
    0,
    0,
    0,
    0,
    0,
    (unaryfunc)pointNeg,
    0,
    0,
    (inquiry)pointTrue,
};
#endif

static PySequenceMethods PointSequenceMethods = {
    0,                                          /* sq_length */
    0,                                          /* sq_concat */
    0,                                          /* sq_repeat */
    (ssizeargfunc)pointItem,                   /* sq_item */
    0,                                          /* sq_slice */
    0,                                          /* sq_ass_item */
    0,                                          /* sq_ass_slice */
    0,                                          /* sq_contains */
};

static PyMethodDef PointMethods[] = {
    {"copy", (PyCFunction)pointCopy, METH_NOARGS,
        PyDoc_STR("Creates a copy of this point.")},
    {NULL}  // Sentinel
};


/* NOTES:
    * Points are immutable; when operations are performed on them new points are created.
*/
static PyTypeObject PointType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pytrix.Point",
    sizeof(Point),
    0,                         /*tp_itemsize*/
    (destructor)pointDeInit,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    &PointNumberMethods,                         /*tp_as_number*/
    &PointSequenceMethods,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)pointStr,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /*tp_flags*/
    "Object representing a Point.",
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    pointRichCmp,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    (getiterfunc)pointIter,                     /* tp_iter */
    0,                     /* tp_iternext */
    PointMethods,             /* tp_methods */
    PointMembers,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)pointInit,      /* tp_init */
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
