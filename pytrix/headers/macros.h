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

// Vector Macros
#define Vector_GetValue(vector, idx) (*((vector->data) + idx))
#define Vector_SetValue(vector, idx, val) Vector_GetValue(vector, idx) = val


// Matrix Macros
#define Matrix_Index(matrix, row, col) ((matrix->width) * row + col)
#define Matrix_GetValue(matrix, row, col) (*((matrix->data) + Matrix_Index(matrix, row, col)))
#define Matrix_SetValue(matrix, row, col, val) Matrix_GetValue(matrix, row, col) = val
