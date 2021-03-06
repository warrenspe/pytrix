# pytrix
Python utility which allows oeprations to be performed on Matrices, Vectors and Points.

## Installation
pytrix can be installed using pip:

`pip install pytrix`

Alternatively, pytrix can be installed manually by cloning this repo and running the following command in the main directory:

`python setup.py install`

## Usage
The pytrix module contains 3 classes, Matrix, Vector, and Point.  These objects are immutable.  It also contains a few convenience functions for constructing certain special types of matrices.

### Matrix

#### Matrix.\_\_init__(...)
Matrices can be instantiated by passing either a list containing the rows of the matrix, or by passing the rows of the matrix to the constructor itself.
```
>>> print(pytrix.Matrix([[1, 2, 3], [4, 5, 6]]))
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
>>> print(pytrix.Matrix([1, 2, 3], [4, 5, 6]))
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
```

#### Matrix.\_\_getitem__(Int)
Matrices can be indexed by their row to return a Vector.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> m[1]
<pytrix.Vector object at 0x6ffffda0df0>
>>> print(m[1])
(4.0, 5.0, 6.0)
```

#### Matrix.\_\_add__(Matrix)
Matrices can be added to other matrices having the same number of rows and columns.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m + m)
[2.0, 4.0, 6.0]
[8.0, 10.0, 12.0]
>>> print(m + pytrix.Matrix([1]))
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Matrices must be of the same dimensions.
```

#### Matrix.\_\_sub__(Matrix)
Matrices can be subtracted from other matrices having the same number of rows and columns.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m - m)
[0.0, 0.0, 0.0]
[0.0, 0.0, 0.0]
>>> print(m - pytrix.Matrix([1]))
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Matrices must be of the same dimensions.
```

#### Matrix.\_\_mul__(MatrixOrVector)
Matrices can be multiplied with other matrices and vectors to produce new matrices or vectors respectively.  If a large matrix is multiplied with another large matrix instead of using the Naive O(n^3) matrix multiplication, the application will use an implementation of the Strassen-Winograd matrix multiplication algorithm.  If this behaviour is either desired/undesired use of one algorithm or the other can be forced by using either the \_strassenmul function, or \_naiveMul function.  Note that Strassen-Winograd multiplication can only be applied to square matrices of the same size.
```
>>> m1 = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> m2 = pytrix.Matrix([1, 2], [3, 4], [5, 6])
>>> v1 = pytrix.Vector(5, 5, 5)
>>> print(v1)
(5.0, 5.0, 5.0)
>>> print(m1)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
>>> print(m2)
[1.0, 2.0]
[3.0, 4.0]
[5.0, 6.0]

>>> print(m1 * v1)
(30.0, 75.0, 1.6e-322)
>>> print(m1 * m2)
[22.0, 28.0]
[49.0, 64.0]
>>> print(m1._naiveMul(m2))
[22.0, 28.0]
[49.0, 64.0]
>>> print(m1._strassenMul(m2, 2))
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Cannot perform Strassen-Winograd on non-square matrices.
```
Finally, the \_strassenMul function requires a second parameter, which indicates the level recursion should halt at and naive matrix multiplication should be used to finish the computation.  This is because the Strassen-Winograd algorithm is a Divide-and-Conquor algorithm, and for matrices below a certain size it is faster to use the naive matrix multiplication algorithm.
```
>>> m1 = pytrix.Matrix([9, 8], [7, 6])
>>> print(m1)
[9.0, 8.0]
[7.0, 6.0]
>>> m2 = pytrix.Matrix([5, 4], [3, 2])
>>> print(m2)
[5.0, 4.0]
[3.0, 2.0]
>>> print(m1._strassenMul(m2, 2))
[69.0, 52.0]
[53.0, 40.0]
```

#### Matrix.__neg__()
Matrices can be component-wise negated.
```
>>> m1 = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
>>> print(-m1)
[-1.0, -2.0, -3.0]
[-4.0, -5.0, -6.0]
```

#### Matrix.__bool__()
Matrices are considered True unless all its components are 0.
```
>>> print(bool(pytrix.Matrix([0, 0, 1])))
True
>>> print(bool(pytrix.Matrix([0, 0, 0])))
False
```

#### Matrix.copy()
Returns a new copy of this Matrix.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> print(id(m))
7696580423856
>>> print(id(m.copy()))
7696580424016
```

#### Matrix.row(Int)
Matrix.row returns the ith row of the Matrix as a Vector.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> m.row(1)
<pytrix.Vector object at 0x6fffff8af50>
>>> print(m.row(1))
(4.0, 5.0, 6.0)
```

#### Matrix.column(Int)
Matrix.column returns the ith column of the Matrix as a Vector.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> m.column(1)
<pytrix.Vector object at 0x6ffffda0f30>
>>> print(m.column(1))
(2.0, 5.0)
```

#### Matrix.transpose()
Matrix.transpose creates a new matrix from the transpose of the matrix it is called on.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
>>> print(m.transpose())
[1.0, 4.0]
[2.0, 5.0]
[3.0, 6.0]
```

#### Matrix.permute(row1, row2)
Matrix.permute creates a new Matrix identical to the one having the function run on it, with its ith and jth rows swapped.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
>>> print(m.permute(0, 1))
[4.0, 5.0, 6.0]
[1.0, 2.0, 3.0]
```

#### Matrix.isSymmetrical()
Determines whether or not this matrix is symmetrical or not.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
>>> print(m.isSymmetrical())
False
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
>>> print(m.isSymmetrical())
False
>>> m = pytrix.Matrix([1, 0, 1], [0, 1, 0], [1, 0, 1])
>>> print(m)
[1.0, 0.0, 1.0]
[0.0, 1.0, 0.0]
[1.0, 0.0, 1.0]
>>> print(m.isSymmetrical())
True
```

#### Matrix.isIdentity()
Determines whether or not this matrix is an identity matrix.
```
>>> pytrix.identityMatrix(50).isIdentity()
True
>>> pytrix.permutationMatrix(50, 0, 1).isIdentity()
False
```

#### Matrix.isInvertible()
Determines whether or not this matrix is invertible.  Note that this function has nearly the same cost as actually creating the inverse, so if an inverse will be taken at some point it would be wiser to actually try to construct the inverse instead of using this function.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> m.isInvertible()
False
>>> m.inverse()
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Cannot take inverse of non-invertible matrix.
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 10])
>>> m.isInvertible()
True
>>> m.inverse()
<pytrix.Matrix object at 0x6ffffda0d30>
```

#### Matrix.inverse()
Constructs the inverse of the Matrix if possible, else raises a ValueError.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> m.inverse()
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Cannot take inverse of non-invertible matrix.
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 10])
>>> m.isInvertible()
True
>>> print(m.inverse())
[-0.6666666666666665, -1.3333333333333335, 1.0]
[-0.6666666666666667, 3.6666666666666665, -2.0]
[1.0, -2.0, 1.0]
>>> print(m.inverse() * m)
[1.0, 0.0, 0.0]
[0.0, 1.0, 0.0]
[0.0, 0.0, 1.0]
```

#### Matrix.rank()
Determines the rank of the matrix's reduced row echelon form.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 10])
>>> print(m.rank())
3
>>> m = pytrix.Matrix([1, 0, 0], [0, 1, 0], [0, 0, 0])
>>> print(m.rank())
2
```

#### Matrix.trace()
Determines the trace of the matrix.  Note that this is only a valid operation for square matrices.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
>>> print(m.trace())
15.0
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> m.trace()
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Matrix.trace can only computed on square matrices.
```

#### Matrix.determinant()
Determines the determinant of the matrix.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
>>> print(m.determinant())
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 10])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 10.0]
>>> print(m.determinant())
-3.0
```

#### Matrix.gaussianElim()
Creates a new matrix from this matrix, by performing gaussian elimination on the matrix, to convert it into a reduced row echelon form.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 10])
>>> print(m.gaussianElim())
[1.0, 2.0, 3.0]
[0.0, -3.0, -6.0]
[0.0, 0.0, 1.0]
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m.gaussianElim())
[1.0, 2.0, 3.0]
[0.0, -3.0, -6.0]
```

#### Matrix.factorLU()
Factors a matrix into two new matrices L and U, having M = L * U, where L is lower triangular, and U is upper triangular.  Note that m must be invertible.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 10])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 10.0]
>>> l, u = m.factorLU()
>>> print(l)
[1.0, 0.0, 0.0]
[4.0, 1.0, 0.0]
[7.0, 2.0, 1.0]
>>> print(u)
[1.0, 2.0, 3.0]
[0.0, -3.0, -6.0]
[0.0, 0.0, 1.0]
>>> print(l * u)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 10.0]
>>> m = pytrix.Matrix([0, 0, 1], [0, 1, 0], [1, 0, 0])
>>> print(m)
[0.0, 0.0, 1.0]
[0.0, 1.0, 0.0]
[1.0, 0.0, 0.0]
>>> m.factorLU()
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Cannot factor non-invertible Matrix.
```

#### Matrix.factorLDU()
Factors a matrix into three new matrices, L, D, and U, having M = L * D * U, where L is lower triangular, D is diagonal, and U is upper triangular.  Note that m must be invertible.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 10])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 10.0]
>>> l, d, u = m.factorLDU()
>>> print(l)
[1.0, 0.0, 0.0]
[4.0, 1.0, 0.0]
[7.0, 2.0, 1.0]
>>> print(d)
[1.0, 0.0, 0.0]
[0.0, -3.0, 0.0]
[0.0, 0.0, 1.0]
>>> print(u)
[1.0, 2.0, 3.0]
[0.0, 1.0, 2.0]
[0.0, 0.0, 1.0]
>>> print(l * d * u)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 10.0]
>>> m = pytrix.Matrix([0, 0, 1], [0, 1, 0], [1, 0, 0])
>>> print(m)
[0.0, 0.0, 1.0]
[0.0, 1.0, 0.0]
[1.0, 0.0, 0.0]
>>> m.factorLDU()
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Cannot factor non-invertible Matrix.
```

#### Matrix.factorPLU()
Factors a matrix into three new matrices, P, L, and U, having P * M = L * U, where P is a permutation matrix, L is lower triangular, and U is upper triangular.  Note that m need not be invertible.
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
>>> p, l, u = m.factorPLU()
>>> print(p)
[1.0, 0.0, 0.0]
[0.0, 1.0, 0.0]
[0.0, 0.0, 1.0]
>>> print(l)
[1.0, 0.0, 0.0]
[4.0, 1.0, 0.0]
[7.0, 2.0, 1.0]
>>> print(u)
[1.0, 2.0, 3.0]
[0.0, -3.0, -6.0]
[0.0, 0.0, 0.0]
>>> print(p * m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
>>> print (l * u)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
```

#### Matrix.factorPLDU()
Factors a matrix into four new matrices, P, L, D, and U, having P * M = L * D * U, where P is a permutation matrix, L is lower triangular, D is diagonal, and U is upper triangular.  Note that m need not be invertible.
```
>>> m = pytrix.Matrix([0, 2, 3], [4, 5, 6], [7, 8, 9])
>>> print(m)
[0.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
>>> p, l, d, u = m.factorPLDU()
>>> print(p)
[0.0, 1.0, 0.0]
[1.0, 0.0, 0.0]
[0.0, 0.0, 1.0]
>>> print(l)
[1.0, 0.0, 0.0]
[0.0, 1.0, 0.0]
[1.75, -0.375, 1.0]
>>> print(d)
[4.0, 0.0, 0.0]
[0.0, 2.0, 0.0]
[0.0, 0.0, -0.375]
>>> print(u)
[1.0, 1.25, 1.5]
[0.0, 1.0, 1.5]
[0.0, 0.0, 1.0]
>>> print(p * m)
[4.0, 5.0, 6.0]
[0.0, 2.0, 3.0]
[7.0, 8.0, 9.0]
>>> print(l * d * u)
[4.0, 5.0, 6.0]
[0.0, 2.0, 3.0]
[7.0, 8.0, 9.0]
```

#### Matrix Attributes
Matrices have two attributes, `rows` and `columns` which are integer values corresponding to the matrices' number of rows and columns
```
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(m.rows)
2
>>> print(m.columns)
3
```

### Vector

#### Vector.\_\_init__(...)
Vectors can be instantiated by passing either a list containing the values of the vector, or by passing the values of the vector to the constructor itself.
```
>>> v = pytrix.Vector([1, 2, 3])
>>> print(v)
(1.0, 2.0, 3.0)
>>> v = pytrix.Vector(1, 2, 3)
>>> print(v)
(1.0, 2.0, 3.0)
```

#### Vector.\_\_getitem__(Int)
It is possible to extract the components from a Vector using dictionary-style indexing.
```
>>> v = pytrix.Vector(1, 2, 3)
>>> print(v[0])
1.0
>>> print(v[1])
2.0
>>> print(v[2])
3.0
>>> print(v[3])
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
IndexError: Cannot return item in index 3 of Vector with 3 dimensions.
```

#### Vector.\_\_add__(Vector)
A vector can be component-wise added with another vector to produce a new third vector.
```
>>> v1 = pytrix.Vector(1, 2, 3)
>>> v2 = pytrix.Vector(4, 5, 6)
>>> print(v1 + v2)
(5.0, 7.0, 9.0)
>>> v2 = pytrix.Vector(4, 5)
>>> print(v1 + v2)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Vectors must be of the same dimensions.
```

#### Vector.\_\_sub__(Vector)
A vector can be component-wise subtracted from another veector to produce a new third vector.
```
>>> v1 = pytrix.Vector(1, 2, 3)
>>> v2 = pytrix.Vector(4, 5, 6)
>>> print(v1 - v2)
(-3.0, -3.0, -3.0)
>>> v2 = pytrix.Vector(4, 5)
>>> print(v1 - v2)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: Vectors must be of the same dimensions.
```

#### Vector.\_\_mul__(MatrixOrScalar)
A vector can be multiplied by either a matrix, or a scalar to produce a new vector. Note that if it is being multiplied by a matrix it will always take the form M\*V.  In other words, the matrix will always be the left operand and the vector the right.
```
>>> v = pytrix.Vector(1, 2, 3)
>>> m = pytrix.Matrix([1, 2, 3], [4, 5, 6])
>>> print(v)
(1.0, 2.0, 3.0)
>>> print(m)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
>>> print(v * m)
(14.0, 32.0)
>>> print(v * 5)
(5.0, 10.0, 15.0)
```

#### Vector.\_\_div__(Scalar)
Creates a new vector by dividing the components of a vector by a scalar.
```
>>> v = pytrix.Vector(1, 2, 3)
>>> print(v)
(1.0, 2.0, 3.0)
>>> print(v / 5)
(0.2, 0.4, 0.6)
```

#### Vector.\_\_neg__()
Creates a new vector by negating the components of a vector.
```
>>> v = pytrix.Vector(1, 2, 3)
>>> print(v)
(1.0, 2.0, 3.0)
>>> print(-v)
(-1.0, -2.0, -3.0)
```

#### Vector.\_\_bool__()
Returns True if the vector contains any non-zero components, else False.
```
>>> v = pytrix.Vector(0, 0, 0)
>>> print(bool(v))
False
```

#### Vector.copy()
Returns a new copy of this Vector.
```
>>> v = pytrix.Vector(1, 2, 3)
>>> print(id(v))
7696578907216
>>> print(id(v.copy()))
7696578907696
>>>
```

#### Vector.dot(Vector)
Calculates the dot product of this vector with another.
```
>>> v = pytrix.Vector(1, 2, 3)
>>> v.dot(v)
14.0
```

#### Vector.cross(Vector)
Calculates the cross product of this vector with another.
```
>>> v1 = pytrix.Vector(1, 2, 3)
>>> v2 = pytrix.Vector(4, 5, 6)
>>> print(v1.cross(v2))
(-3.0, 6.0, -3.0)
```

#### Vector.length()
Calculates the length of this vector.
```
>>> v = pytrix.Vector(1, 2, 3)
>>> print(v.length())
3.7416573867739413
>>> v = pytrix.Vector(0, 0, 0)
>>> print(v.length())
0.0
```

#### Vector.unit()
Returns a new vector with the same direction as this vector but with a length of 1
```
>>> v = pytrix.Vector(1, 2, 3)
>>> u = v.unit()
>>> print(u)
(0.2672612419124244, 0.5345224838248488, 0.8017837257372732)
>>> print(u.length())
1.0
```

#### Vector.isUnit()
Determines whether or not the vector is a unit vector (ie has length 1)
```
>>> v = pytrix.Vector(1, 2, 3)
>>> print(v.isUnit())
False
>>> v = pytrix.Vector(1, 0, 0)
>>> print(v.isUnit())
True
```

#### Vector.angleBetween(Vector)
Calculates the angle between two vectors in radians.
```
>>> v1 = pytrix.Vector(1, 0, 0)
>>> v1.angleBetween(v1)
0.0
>>> v2 = pytrix.Vector(0, 1, 0)
>>> v1.angleBetween(v2)
1.5707963267948966
```

#### Vector.isOrthogonal(Vector)
Determines whether two vectors are orthogonal to each other.
```
>>> v1 = pytrix.Vector(1, 0, 0)
>>> v1.isOrthogonal(v1)
False
>>> v2 = pytrix.Vector(0, 1, 0)
>>> v1.isOrthogonal(v2)
True
```

#### Vector Attributes
Vectors have a single attribute, `dimensions` which is an integer value corresponding to the vectors' number of components.
```
>>> v = pytrix.Vector(1, 2, 3, 4)
>>> print(v.dimensions)
4
```

### Point

#### Point.\_\_init__(...)
Points can be instantiated by passing either a list containing the coordinates of the point, or by passing the coordinates of the point to the constructor itself.
```
>>> p = pytrix.Point([1, 2, 3])
>>> print(p)
(1.0, 2.0, 3.0)
>>> p = pytrix.Point(1, 2, 3)
>>> print(p)
(1.0, 2.0, 3.0)
```

#### Point.\_\_getitem__(Int)
It is possible to extract the coordinates from a Point using dictionary-style indexing.
```
>>> p = pytrix.Point(1, 2, 3)
>>> print(p[0])
1.0
>>> print(p[1])
2.0
>>> print(p[2])
3.0
>>> print(p[3])
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
IndexError: Cannot return item in index 3 of Point with 3 dimensions.
```

#### Point.\_\_add__(Vector)
A point can be added with a vector to construct a new point object.
```
>>> p = pytrix.Point(1, 2, 3)
>>> v = pytrix.Vector(1, 2, 3)
>>> print(p + v)
(2.0, 4.0, 6.0)
```

#### Point.\_\_sub__(PointOrVector)
A point can be subtracted from either a vector or another point.  If it is subtracted from a vector, it returns a Point, similar to how Point.\_\_add__ works.  If it is subtracted from another point, it returns a vector, which maps from the second point to the first.
```
>>> p1 = pytrix.Point(2, 3, 4)
>>> p2 = pytrix.Point(1, 2, 3)
>>> p1 - p2
<pytrix.Vector object at 0x6ffffda0cd0>
>>> v = p1 - p2
>>> print(v)
(1.0, 1.0, 1.0)
>>> p1 - v
<pytrix.Point object at 0x6ffffda0e30>
>>> print(p1 - v)
(1.0, 2.0, 3.0)
```

#### Point.\_\_neg__()
Constructs a new point by negating the coordinates of this point.
```
>>> p = pytrix.Point(1, 2, 3)
>>> print(p)
(1.0, 2.0, 3.0)
>>> print(-p)
(-1.0, -2.0, -3.0)
```

#### Point.\_\_bool__()
Returns True if the point is not on the 0-coordinate, else False.
```
>>> p = pytrix.Point(1, 2, 3)
>>> print(bool(p))
True
>>> p = pytrix.Point(0, 0, 0)
>>> print(bool(p))
False
```

#### Point.copy()
Returns a new copy of this point.
```
>>> p = pytrix.Point(1, 2, 3)
>>> print(id(p))
7696578907664
>>> print(id(p.copy()))
7696580915024
```

### Convenience Matrix Functions

#### identityMatrix(dimensions)
Constructs an identity matrix of a specified number of dimensions
```
>>> print(pytrix.identityMatrix(1))
[1.0]
>>> print(pytrix.identityMatrix(2))
[1.0, 0.0]
[0.0, 1.0]
>>> print(pytrix.identityMatrix(3))
[1.0, 0.0, 0.0]
[0.0, 1.0, 0.0]
[0.0, 0.0, 1.0]
```

#### permutationMatrix(dimensions, row1, row2)
Constructs a permutation matrix of the specified number of dimensions which swaps the two given rows of a matrix it's multiplied with.
```
>>> a = pytrix.Matrix([1, 2, 3], [4, 5, 6], [7, 8, 9])
>>> print(a)
[1.0, 2.0, 3.0]
[4.0, 5.0, 6.0]
[7.0, 8.0, 9.0]
>>> p = pytrix.permutationMatrix(3, 0, 1)
>>> print(p)
[0.0, 1.0, 0.0]
[1.0, 0.0, 0.0]
[0.0, 0.0, 1.0]
>>> print(p * a)
[4.0, 5.0, 6.0]
[1.0, 2.0, 3.0]
[7.0, 8.0, 9.0]
```

#### rotation2DMatrix(radians)
Constructs a 2 dimensional rotation matrix which can be applied to a vector to rotate it by the specified number of radians.
```
>>> v = pytrix.Vector(1, 1)
>>> print(v)
(1.0, 1.0)
>>> r = pytrix.rotation2DMatrix(math.pi)
>>> print(r)
[-1.0, -1.2246467991473532e-16]
[1.2246467991473532e-16, -1.0]
>>> print(r * v)
(-1.0000000000000002, -0.9999999999999999)
```

#### rotation3DMatrix(radians)
Functions the same was as rotation2DMatrix, except creates a rotation matrix which can be applied to vectors with 3 dimensions.
