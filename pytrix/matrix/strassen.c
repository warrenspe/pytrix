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



/*
    Set of utilities necessary to implement the Strassen-Winograd Matrix Multiplication algorithm.

    The matrix operations in these utilities differ from the regular utilities in that they:
        * Accept a third argument, which is an initialized output matrix to be populated
        * Accept SWMatrixPart * instead of Matrix *, see below
        * Accept start and stop values for both the rows and columns of the input matrices.
              This is so that we can operate on subportions of the input matrices without having to reallocate them.
        * Require that both inputs be square matrices of the same size
        * Return a 0 on success and 1 on failure

    Inspiration for the implementation: https://arxiv.org/pdf/0707.2347v5.pdf
*/


// The Strassen-Winograd algorithm involves recursively splitting matrices down into subcomponents, which would require
// quite a lot of system level memory calls.  However, since we don't ever actually `change` the matrix, there's no
// harm in re-using the initial matrix we're given.  This requires us to be able to record which portion of the matrix
// we're interested in.  To this end, we will use this struct to record the initial input matrix and the portion a
// given part of the code is interested in.
typedef struct {
    Matrix *m;
    unsigned int mRowStart;
    unsigned int mRowEnd;
    unsigned int mColStart;
    unsigned int mColEnd;

} SWMatrixPart;

// Convenience Macros
#define SWM_M(swm) (swm->m)
#define SWM_RS(swm) (swm->mRowStart)
#define SWM_RE(swm) (swm->mRowEnd)
#define SWM_CS(swm) (swm->mColStart)
#define SWM_CE(swm) (swm->mColEnd)

// Function Prototypes
static void _swMatrixPartInit(SWMatrixPart *, Matrix *, unsigned int, unsigned int, unsigned int, unsigned int);
static unsigned int _getMinPadding(unsigned int, unsigned int);
static Matrix *_padMatrix(Matrix *, unsigned int);
static unsigned int _strassenMatrixAdd(SWMatrixPart *, SWMatrixPart *, SWMatrixPart *);
static unsigned int _strassenMatrixSub(SWMatrixPart *, SWMatrixPart *, SWMatrixPart *);
static unsigned int _strassenNaiveMatrixMul(SWMatrixPart *, SWMatrixPart *, SWMatrixPart *);
static unsigned int _strassenMatrixMul(SWMatrixPart *, SWMatrixPart *, SWMatrixPart *, SWMatrixPart *, unsigned int);

// Function Declarations
static void _swMatrixPartInit(SWMatrixPart *s, Matrix *m, unsigned int rs, unsigned int re, unsigned int cs, unsigned int ce) {
/* Initializes the values of a SWMatrixPart struct. */

    s->m = m;
    s->mRowStart = rs;
    s->mRowEnd = re;
    s->mColStart = cs;
    s->mColEnd = ce;
}

static unsigned int _getMinPadding(unsigned int matrixSize, unsigned int cutoff) {
/*  Determines the minimum amount of padding necessary to add to a given matrix in order to perform the
    Strassen-Winograd algorithm on it without requiring further pad operations.

    Inputs: matrixSize - The number of rows & columns in the matrix.
            cutoff     - The Strassen Cutoff.  See the API function for details.

    Outputs: The size of the matrix we should pad to.
*/

    unsigned int cnt = 0;
    while(matrixSize > cutoff) {
        matrixSize++;
        matrixSize >>= 1;
        cnt++;
    }

    return matrixSize << cnt;
}


static Matrix *_padMatrix(Matrix *m, unsigned int newSize) {
/*  Creates a new matrix with a padded number of rows/columns so that we are able to perform the algorithm on
    matrices with an odd number of rows/columns.  This is necessary because Strassen's algorithm is a divide and
    conquor algorithm; which requires the input size to be divisible by 2.

    Inputs: m - The Matrix to be padded.
            newSize - The size the matrix should be padded to.

    Outputs: A new Matrix if successful, NULL if an error occurred.
*/

    unsigned int row,
                 column;
    VECTOR_TYPE val;
    Matrix *new;

    if ((new = _matrixNew(newSize, newSize)) == NULL)
        return NULL;

    // Copy the existing matrix over
    for (row = 0; row < newSize; row++) {
        for (column = 0; column < newSize; column++) {
            // If this is a padding byte
            if (row >= m->rows || column >= m->columns) {
                val = 0;
            } else {
                val = Matrix_GetValue(m, row, column);
            }
            Matrix_SetValue(new, row, column, val);
        }
    }

    return new;
}


static unsigned int _strassenMatrixAdd(SWMatrixPart *a, SWMatrixPart *b, SWMatrixPart *out) {
    unsigned int rowDelta,
                 colDelta,
                 minRowA = SWM_RS(a),
                 minRowB = SWM_RS(b),
                 minRowOut = SWM_RS(out),
                 minColA = SWM_CS(a),
                 minColB = SWM_CS(b),
                 minColOut = SWM_CS(out),
                 maxDelta = SWM_RE(a) - SWM_RS(a);

    for (rowDelta = 0; rowDelta < maxDelta; rowDelta++)
        for (colDelta = 0; colDelta < maxDelta; colDelta++)
            Matrix_SetValue(SWM_M(out), minRowOut + rowDelta, minColOut + colDelta,
                                Matrix_GetValue(SWM_M(a), minRowA + rowDelta, minColA + colDelta) +
                                Matrix_GetValue(SWM_M(b), minRowB + rowDelta, minColB + colDelta));

    return 0;
}


static unsigned int _strassenMatrixSub(SWMatrixPart *a, SWMatrixPart *b, SWMatrixPart *out) {
    unsigned int rowDelta,
                 colDelta,
                 minRowA = SWM_RS(a),
                 minRowB = SWM_RS(b),
                 minRowOut = SWM_RS(out),
                 minColA = SWM_CS(a),
                 minColB = SWM_CS(b),
                 minColOut = SWM_CS(out),
                 maxDelta = SWM_RE(a) - SWM_RS(a);

    for (rowDelta = 0; rowDelta < maxDelta; rowDelta++)
        for (colDelta = 0; colDelta < maxDelta; colDelta++)
            Matrix_SetValue(SWM_M(out), minRowOut + rowDelta, minColOut + colDelta,
                                Matrix_GetValue(SWM_M(a), minRowA + rowDelta, minColA + colDelta) -
                                Matrix_GetValue(SWM_M(b), minRowB + rowDelta, minColB + colDelta));

    return 0;
}


static unsigned int _strassenNaiveMatrixMul(SWMatrixPart *a, SWMatrixPart *b, SWMatrixPart *out) {
    unsigned int rowDelta,
                 colDelta,
                 k,
                 minRowA = SWM_RS(a),
                 minRowB = SWM_RS(b),
                 minRowOut = SWM_RS(out),
                 minColA = SWM_CS(a),
                 minColB = SWM_CS(b),
                 minColOut = SWM_CS(out),
                 maxDelta = SWM_RE(a) - SWM_RS(a);
    VECTOR_TYPE val;

    for (rowDelta = 0; rowDelta < maxDelta; rowDelta++)
        for (colDelta = 0; colDelta < maxDelta; colDelta++) {
            val = 0;
            for (k = 0; k < maxDelta; k++)
                val += Matrix_GetValue(SWM_M(a), minRowA + rowDelta, minColA + k) * 
                       Matrix_GetValue(SWM_M(b), minRowB + k, minColB + colDelta);
            Matrix_SetValue(SWM_M(out), minRowOut + rowDelta, minColOut + colDelta, val);
        }

    return 0;
}


static unsigned int _strassenMatrixMul(SWMatrixPart *a, SWMatrixPart *b, SWMatrixPart *out, SWMatrixPart *x,
                                       unsigned int cutoff) {
    unsigned int halfSize,
                 regularSize,
                 errored = 0;

    SWMatrixPart a11,
                 a12,
                 a21,
                 a22,
                 b11,
                 b12,
                 b21,
                 b22,
                 u1p,
                 u5p,
                 u6p,
                 u7p,
                 x11,
                 x12,
                 x21,
                 x22;

    regularSize = SWM_RE(a) - SWM_RS(a);
    halfSize = regularSize >> 1;

    // If there's fewer than `cutoff` elements in our matrices just use regular multiplication
    if (regularSize <= cutoff)
        return _strassenNaiveMatrixMul(a, b, out);

    // Initialize our MatrixParts
    _swMatrixPartInit(&a11, SWM_M(a), SWM_RS(a), SWM_RS(a) + halfSize, SWM_CS(a), SWM_CS(a) + halfSize);
    _swMatrixPartInit(&a12, SWM_M(a), SWM_RS(a), SWM_RS(a) + halfSize, SWM_CS(a) + halfSize, SWM_CE(a));
    _swMatrixPartInit(&a21, SWM_M(a), SWM_RS(a) + halfSize, SWM_RE(a), SWM_CS(a), SWM_CS(a) + halfSize);
    _swMatrixPartInit(&a22, SWM_M(a), SWM_RS(a) + halfSize, SWM_RE(a), SWM_CS(a) + halfSize, SWM_CE(a));

    _swMatrixPartInit(&b11, SWM_M(b), SWM_RS(b), SWM_RS(b) + halfSize, SWM_CS(b), SWM_CS(b) + halfSize);
    _swMatrixPartInit(&b12, SWM_M(b), SWM_RS(b), SWM_RS(b) + halfSize, SWM_CS(b) + halfSize, SWM_CE(b));
    _swMatrixPartInit(&b21, SWM_M(b), SWM_RS(b) + halfSize, SWM_RE(b), SWM_CS(b), SWM_CS(b) + halfSize);
    _swMatrixPartInit(&b22, SWM_M(b), SWM_RS(b) + halfSize, SWM_RE(b), SWM_CS(b) + halfSize, SWM_CE(b));

    _swMatrixPartInit(&u1p, SWM_M(out), SWM_RS(out), SWM_RS(out) + halfSize, SWM_CS(out), SWM_CS(out) + halfSize);
    _swMatrixPartInit(&u5p, SWM_M(out), SWM_RS(out), SWM_RS(out) + halfSize, SWM_CS(out) + halfSize, SWM_CE(out));
    _swMatrixPartInit(&u6p, SWM_M(out), SWM_RS(out) + halfSize, SWM_RE(out), SWM_CS(out), SWM_CS(out) + halfSize);
    _swMatrixPartInit(&u7p, SWM_M(out), SWM_RS(out) + halfSize, SWM_RE(out), SWM_CS(out) + halfSize, SWM_CE(out));

    _swMatrixPartInit(&x11, SWM_M(x), SWM_RS(x), SWM_RS(x) + halfSize, SWM_CS(x), SWM_CS(x) + halfSize);
    _swMatrixPartInit(&x12, SWM_M(x), SWM_RS(x), SWM_RS(x) + halfSize, SWM_CS(x) + halfSize, SWM_CE(x));
    _swMatrixPartInit(&x21, SWM_M(x), SWM_RS(x) + halfSize, SWM_RE(x), SWM_CS(x), SWM_CS(x) + halfSize);
    _swMatrixPartInit(&x22, SWM_M(x), SWM_RS(x) + halfSize, SWM_RE(x), SWM_CS(x) + halfSize, SWM_CE(x));

    // Perform Strassen-Winograd
    if (_strassenMatrixSub(&b22, &b12, &u5p)) // T3
        return 1;
    if (!errored && _strassenMatrixSub(&a11, &a21, &u1p)) // S3
        return 1;
    if (!errored && _strassenMatrixSub(&b12, &b11, &u7p)) // T1
        return 1;
    if (!errored && _strassenMatrixAdd(&a21, &a22, &x11)) // S1
        return 1;
    if (!errored && _strassenMatrixMul(&u1p, &u5p, &u6p, &x22, cutoff)) // P7
        return 1;
    if (!errored && _strassenMatrixMul(&x11, &u7p, &u5p, &x22, cutoff)) // P5
        return 1;
    if (!errored && _strassenMatrixSub(&b22, &u7p, &u1p)) // T2
        return 1;
    if (!errored && _strassenMatrixSub(&x11, &a11, &u7p)) // S2
        return 1;
    if (!errored && _strassenMatrixMul(&u7p, &u1p, &x11, &x22, cutoff)) // P6
        return 1;
    if (!errored && _strassenMatrixMul(&a11, &b11, &x12, &x22, cutoff)) // P1
        return 1;
    if (!errored && _strassenMatrixAdd(&x12, &x11, &x21)) // U2
        return 1;
    if (!errored && _strassenMatrixAdd(&x21, &u6p, &x11)) // U3
        return 1;
    if (!errored && _strassenMatrixAdd(&x21, &u5p, &u6p)) // U4
        return 1;
    if (!errored && _strassenMatrixSub(&a12, &u7p, &x21)) // S4
        return 1;
    if (!errored && _strassenMatrixSub(&u1p, &b21, &u7p)) // T4
        return 1;
    if (!errored && _strassenMatrixMul(&x21, &b22, &u1p, &x22, cutoff)) // P3
        return 1;
    if (!errored && _strassenMatrixMul(&a22, &u7p, &x21, &x22, cutoff)) // P4
        return 1;
    if (!errored && _strassenMatrixAdd(&x11, &u5p, &u7p)) // U7
        return 1;
    if (!errored && _strassenMatrixAdd(&u6p, &u1p, &u5p)) // U5
        return 1;
    if (!errored && _strassenMatrixSub(&x11, &x21, &u6p)) // U6
        return 1;
    if (!errored && _strassenMatrixMul(&a12, &b21, &x21, &x22, cutoff)) // P2
        return 1;
    if (!errored && _strassenMatrixAdd(&x12, &x21, &u1p)) // U1
        return 1;

    return 0;
}

Matrix *strassenWinogradMatrixMatrixMul(Matrix *a, Matrix *b, unsigned int cutoff) {
/*  API function for the Strassen Winograd matrix multiplication algorithm.

    Inputs: a      - The first matrix to multiply.
            b      - The second matrix to multiply.
            cutoff - A value, which when the recursively calculated matrice's drops below, will be calculated using the
                     naive O(n^3) algorithm.

    Outputs: A new Matrix initialized by performing a * b using the Strassen-Winograd algorithm.
             NULL if an exception occurrs.
*/

    unsigned int padSize,
                 prePadSize,
                 padded = 0,
                 result,
                 i;
    Matrix *out,
           *x;
    SWMatrixPart aPart,
                 bPart,
                 outPart,
                 xPart;

    // Validate that our input matrices are the same size and square.  This is the only case when we can apply
    // Strassen-Winograd
    if (a->rows != a->columns || b->rows != b->columns) {
        PyErr_SetString(PyExc_ValueError, "Cannot perform Strassen-Winograd on non-square matrices.");
        return NULL;
    } else if (b->rows != a->rows) {
        PyErr_SetString(PyExc_ValueError, "Cannot perform Strassen-Winograd on matrices of different sizes");
        return NULL;
    }
    // Assert we are at least 2 rows/columns wide
    if (a->rows == 0 || a->rows == 1) {
        PyErr_Format(PyExc_ValueError, "Cannot perform Strassen-Winograd on a %u dimensional matrix", a->rows);
        return NULL;
    }

    // Check if our matrices need padding.  This is necessary because if, say, we were given a matrix of odd length we
    // would first need to add a row and column of 0's at the end so that it can be divided into two chunks.
    // Additionally, we want to ensure that we will only need to pad the matrix once, and be able to repeatedly divide
    // it into smaller matrices until eventually we result with a matrix with a size smaller than `cutoff`,
    // (as naive O(n^3)matrix multiplication is faster for small matrices).  For example, if we stop strassen on
    // matrices of size 10 or less, and we're given a matrix of size 42, we want to pad it to 48.  This is because if
    // we were to work on a matrix of size 42, after 1 step we would have matrices of size 21, which would require
    // re-padding to 22, then we'd have matrices of size 11, which would have to be padded to 12, then finally after
    // two paddings we would have matrices small enough to perform O(n^3) multiplication.  By padding to 48 once we
    // divide to 24, then 12, then 6 without having to re-pad.
    padSize = _getMinPadding(a->rows, cutoff);
    prePadSize = a->rows;
    if (a->rows != padSize) {
        padded = 1;
        if ((a = _padMatrix(a, padSize)) == NULL)
            return NULL;
        if ((b = _padMatrix(b, padSize)) == NULL) {
            Py_DECREF(a);
            return NULL;
        }
    }

    if ((out = _matrixNew(a->rows, a->columns)) == NULL) {
        if (padded) {
            Py_DECREF(a);
            Py_DECREF(b);
        }
        return NULL;
    }

    // Initialize our temp matrix
    if ((x = _matrixNew(out->rows, out->columns)) == NULL) {
        Py_DECREF(a);
        Py_DECREF(b);
        Py_DECREF(out);
        return NULL;
    }

    _swMatrixPartInit(&aPart, a, 0, a->rows, 0, a->columns);
    _swMatrixPartInit(&bPart, b, 0, b->rows, 0, b->columns);
    _swMatrixPartInit(&outPart, out, 0, out->rows, 0, out->columns);
    _swMatrixPartInit(&xPart, x, 0, x->rows, 0, x->columns);

    result = _strassenMatrixMul(&aPart, &bPart, &outPart, &xPart, cutoff);

    // If we created padded matrices, release them
    if (padded) {
        Py_DECREF(a);
        Py_DECREF(b);
    }

    // Release our temp matrix
    Py_DECREF(x);

    // If our algorithm was unsuccessful release the return matrix and return NULL
    if (result) {
        Py_DECREF(out);
        return NULL;
    }

    // Otherwise, update our matrix to remove any unnecessary padding
    // Free any unnecessary padding rows.  Don't worry about columns because those will be free'd with their row
    for (i = prePadSize; i < out->rows; i++)
        Py_DECREF(Matrix_GetVector(out, i));
    // Update the size of the matrix
    out->rows = prePadSize;
    out->columns = prePadSize;
    // Update the size of each of the vectors in the matrix
    for (i = 0; i < out->rows; i++)
        Matrix_GetVector(out, i)->dimensions = out->columns;

    return out;
}
