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
static void _freeMatrixData(Matrix *);
int matrixInit(Matrix *, PyObject *);
void matrixDeInit(Matrix *);

// iter.c
PyObject *matrixIter(Matrix *);

// operations.c
PyObject *matrixStr(Matrix *);
PyObject *matrixAdd(PyObject *, PyObject *);
PyObject *matrixSub(PyObject *, PyObject *);
PyObject *matrixMul(PyObject *, PyObject *);
PyObject *matrixNeg(PyObject *);
int matrixTrue(Matrix *);
PyObject *matrixRichCmp(PyObject *, PyObject *, int);

// utils.c
Matrix *_matrixNewBlank(unsigned int, unsigned int);
Matrix *_matrixNew(unsigned int, unsigned int);
Matrix *_matrixCopy(Matrix *);
unsigned int _assertMatrix(PyObject *);
unsigned int _assertMatrixDimensionsEqual(Matrix *, Matrix *);
Matrix *_matrixAdd(Matrix *, Matrix *);
Matrix *_matrixSub(Matrix *, Matrix *);
Matrix *_scalarMatrixMul(Matrix *, VECTOR_TYPE);
Vector *_vectorMatrixMul(PyObject *, PyObject *);
Matrix *_matrixMatrixMul(Matrix *, Matrix *);
Matrix *_matrixNeg(Matrix *);
unsigned char _matricesEqual(Matrix *, Matrix *);

// strassen.c
Matrix *strassenWinogradMatrixMatrixMul(Matrix *, Matrix *);
