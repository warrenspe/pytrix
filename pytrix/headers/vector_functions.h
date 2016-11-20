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

// init.c
static int vectorInit(Vector *, PyObject *);
static void vectorDeInit(Vector *);

//operations.c
PyObject *vectorStr(Vector *);
PyObject *vectorDot(Vector *, PyObject *);
PyObject *vectorCross(Vector *, PyObject *);
PyObject *vectorLength(Vector *);
PyObject *vectorAngleBetween(Vector *, PyObject *);
PyObject *vectorUnit(Vector *);
PyObject *vectorIsUnit(Vector *);
PyObject *vectorCopy(Vector *);
PyObject *vectorAdd(PyObject *, PyObject *);
PyObject *vectorSub(PyObject *, PyObject *);
PyObject *vectorMul(PyObject *, PyObject *);
PyObject *vectorDiv(PyObject *, PyObject *);
PyObject *vectorNeg(Vector *);
PyObject *vectorOrthogonal(Vector *, PyObject *);
PyObject *vectorRichCmp(PyObject *, PyObject *, int);
int vectorTrue(Vector *);
PyObject *vectorItem(PyObject *, Py_ssize_t);

// utils.c
Vector *_vectorNew(unsigned int);
void _vectorCopyData(Vector *, Vector *);
Vector *_vectorCopy(Vector *);
unsigned char _assertVectorDimensionsEqual(Vector *, Vector *);
unsigned char _assertVector(PyObject *);

PyObject *_vectorToTuple(Vector *);
VECTOR_TYPE _vectorDot(Vector *, Vector *);
VECTOR_TYPE _vectorLength(Vector *);
Vector *_vectorAdd(Vector *, Vector *);
Vector *_vectorSub(Vector *, Vector *);
Vector *_vectorMul(Vector *, VECTOR_TYPE);
Vector *_vectorDiv(Vector *, VECTOR_TYPE);
Vector *_vectorNeg(Vector *);
unsigned char _vectorsEqual(Vector *, Vector *);

//iter.c
PyObject *vectorIter(Vector *);
