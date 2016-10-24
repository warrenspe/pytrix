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

#include "vector/init.c"
#include "vector/iter.c"

static PyMemberDef VectorMembers[] = {
    {"height", T_UINT, offsetof(Vector, height), READONLY, "Height of the vector."},
    {"width", T_UINT, offsetof(Vector, width), READONLY, "Width of the vector."},
    {"dimensions", T_UINT, offsetof(Vector, dimensions), READONLY, "Number of dimensions of the vector."},
    {NULL} // Sentinel
};

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
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence TODO?*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro TODO?*/
    0,                         /*tp_setattro TODO?*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Object representing a Vector.",
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    (getiterfunc)vectorIter,                     /* tp_iter */
    0,                     /* tp_iternext */
    0,             /* tp_methods */
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