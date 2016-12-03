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

// Point Macros
#define Point_Check(op) (Py_TYPE(op) == &PointType)
#define Point_GetValue(point, idx) (*((point->data) + idx))
#define Point_SetValue(point, idx, val) Point_GetValue(point, idx) = val


// Vector Macros
#define Vector_Check(op) (Py_TYPE(op) == &VectorType)
#define Vector_GetValue(vector, idx) (*((vector->data) + idx))
#define Vector_SetValue(vector, idx, val) Vector_GetValue(vector, idx) = val


// Matrix Macros
#define Matrix_Check(op) (Py_TYPE(op) == &MatrixType)
#define Matrix_GetVector(matrix, row) (*(matrix->data + row))
#define Matrix_SetVector(matrix, row, vector) Matrix_GetVector(matrix, row) = vector
#define Matrix_GetValue(matrix, row, col) (Vector_GetValue(Matrix_GetVector(matrix, row), col))
#define Matrix_SetValue(matrix, row, col, val) (Vector_SetValue(Matrix_GetVector(matrix, row), col, val))

// Other Macros
#define Compatible_Input_Sequence_Check(op) (PyList_CheckExact(op) || PyTuple_CheckExact(op))
