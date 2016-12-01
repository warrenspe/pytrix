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
PyObject *matrixItem(PyObject *, Py_ssize_t);
PyObject *matrixAdd(PyObject *, PyObject *);
PyObject *matrixSub(PyObject *, PyObject *);
PyObject *matrixMul(PyObject *, PyObject *);
PyObject *matrixStrassenMul(PyObject *, PyObject *);
PyObject *matrixNaiveMul(PyObject *, PyObject *);
PyObject *matrixNeg(PyObject *);
int matrixTrue(Matrix *);
PyObject *matrixRichCmp(PyObject *, PyObject *, int);
PyObject *matrixTranspose(PyObject *);
PyObject *matrixRow(PyObject *, PyObject *);
PyObject *matrixColumn(PyObject *, PyObject *);
PyObject *matrixPermute(PyObject *, PyObject *);
PyObject *matrixGaussianElim(PyObject *);
PyObject *matrixFactorLU(PyObject *);
PyObject *matrixFactorLDU(PyObject *);
PyObject *matrixFactorPLU(PyObject *);
PyObject *matrixFactorPLDU(PyObject *);
PyObject *matrixInverse(PyObject *);
PyObject *matrixRank(PyObject *);
PyObject *matrixDeterminant(PyObject *);
PyObject *matrixTrace(PyObject *);
PyObject *matrixIsSymmetrical(PyObject *);
PyObject *matrixIsIdentity(PyObject *);
PyObject *matrixIsInvertible(PyObject *);

// utils.c
Matrix *_matrixNewBlank(unsigned int, unsigned int);
Matrix *_matrixNew(unsigned int, unsigned int);
void _matrixCopyData(Matrix *, Matrix *);
Matrix *_matrixCopy(Matrix *);
void _matrixInitBlank(Matrix *);
unsigned char _matrixInitIdentity(Matrix *, unsigned char);
unsigned char _assertMatrix(PyObject *);
unsigned char _assertMatrixDimensionsEqual(Matrix *, Matrix *);
unsigned char _matricesEqual(Matrix *, Matrix *);
Matrix *_matrixAdd(Matrix *, Matrix *);
Matrix *_matrixSub(Matrix *, Matrix *);
Matrix *_scalarMatrixMul(Matrix *, VECTOR_TYPE);
Vector *_vectorMatrixMul(PyObject *, PyObject *);
unsigned char _inplaceMatrixMatrixMul(Matrix *, Matrix *, Matrix *);
Matrix *_matrixMatrixMul(Matrix *, Matrix *, unsigned char);
Matrix *_matrixNeg(Matrix *);
Matrix *_matrixTranspose(Matrix *);
Matrix *_matrixPermute(Matrix *, unsigned int, unsigned int);
unsigned char _matrixInvertible(Matrix *);
unsigned char _matrixSymmetrical(Matrix *);
unsigned int _matrixRank(Matrix *);
VECTOR_TYPE _matrixDeterminant(Matrix *);
unsigned char _matrixPALDU(Matrix *, Matrix *, Matrix *, Matrix *, Matrix *, unsigned char);
Matrix *_matrixInverse(Matrix *);

// strassen.c
Matrix *strassenWinogradMatrixMatrixMul(Matrix *, Matrix *, unsigned int);
