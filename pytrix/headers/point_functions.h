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
static int pointInit(Point *, PyObject *);
static void pointDeInit(Point *);

//operations.c
PyObject *pointStr(Point *);
PyObject *pointCopy(Point *);
PyObject *pointAdd(PyObject *, PyObject *);
PyObject *pointSub(PyObject *, PyObject *);
PyObject *pointNeg(Point *);
PyObject *pointRichCmp(PyObject *, PyObject *, int);
int pointTrue(Point *);
PyObject *pointItem(PyObject *, Py_ssize_t);

// utils.c
Point *_pointNew(unsigned int);
void _pointCopyData(Point *, Point *);
Point *_pointCopy(Point *);
unsigned char _assertPointDimensionsEqual(Point *, Point *);
unsigned char _assertPointVectorDimensionsEqual(Point *, Vector *);
unsigned char _assertPoint(PyObject *);

PyObject *_pointToTuple(Point *);
Point *_pointAdd(Point *, Vector *);
Point *_pointVectorSub(Point *, Vector *);
Vector *_pointPointSub(Point *, Point *);
Point *_pointNeg(Point *);
unsigned char _pointsEqual(Point *, Point *);

//iter.c
PyObject *pointIter(Point *);
