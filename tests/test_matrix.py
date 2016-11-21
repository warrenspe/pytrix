# Project imports
import pytrix
import tests

class TestMatrix(tests.PytrixTestCase):

    def setUp(self):
        self.e1 = pytrix.Matrix([])
        self.zero1 = pytrix.Matrix([[0]])
        self.zero2 = pytrix.Matrix([[0, 0], [0, 0]])
        self.zero3 = pytrix.Matrix([[0, 0, 0], [0, 0, 0], [0, 0, 0]])
        self.m1 = pytrix.Matrix([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
        self.m2 = pytrix.Matrix([[9, 8, 7], [6, 5, 4], [3, 2, 1]])


    def _assertMatrixEqual(self, matrix, *lists):
        self.assertEqual(matrix.rows, len(lists))
        if len(lists):
            self.assertEqual(matrix.columns, len(lists[0]))
        for i, v in enumerate(matrix):
            self.assertEqual(list(v), lists[i])

    def _assertMatrixEqualWithDelta(self, m1, m2):
        self.assertEqual(m1.rows, m2.rows)
        self.assertEqual(m1.columns, m2.columns)
        for i in range(m1.rows):
            for j in range(m1.columns):
                self.assertAlmostEqual(m1[i][j], m2[i][j], delta=.0001)

    def testMatrixRowsColumns(self):
        self.assertEqual(self.e1.rows, 0)
        self.assertEqual(self.e1.columns, 0)
        self.assertEqual(self.zero1.rows, 1)
        self.assertEqual(self.zero1.columns, 1)
        self.assertEqual(self.zero2.rows, 2)
        self.assertEqual(self.zero2.columns, 2)
        self.assertEqual(self.m1.rows, 3)
        self.assertEqual(self.m1.columns, 3)

    def testMatrixInit(self):
        self.assertRaises(TypeError, pytrix.Matrix, None)
        self.assertRaises(TypeError, pytrix.Matrix, [None])
        self.assertRaises(TypeError, pytrix.Matrix, [[1, 2], None])
        self.assertRaises(TypeError, pytrix.Matrix, 1)
        self.assertRaises(TypeError, pytrix.Matrix, "abc")
        self.assertRaises(TypeError, pytrix.Matrix, [[1, 2], [1]])
        self.assertEqual(pytrix.Matrix([[1, 2], [3, 4], [5, 6]]).rows, 3)
        self.assertEqual(pytrix.Matrix([[1, 2, 3], [4, 5, 6]]).columns, 3)
        self.assertEqual(pytrix.Matrix([]).columns, 0)
        self.assertEqual(pytrix.Matrix([]).rows, 0)
        self._assertMatrixEqual(pytrix.Matrix([1]), [1])
        self._assertMatrixEqual(pytrix.Matrix([1, 2], [3, 4], [5, 6]), [1, 2], [3, 4], [5, 6])
        self._assertMatrixEqual(pytrix.Matrix([[1, 2], [3, 4], [5, 6]]), [1, 2], [3, 4], [5, 6])
        self._assertMatrixEqual(pytrix.Matrix([1, 2, 3]), [1, 2, 3])
        self._assertMatrixEqual(pytrix.Matrix([[1, 2, 3]]), [1, 2, 3])
        self.assertRaises(TypeError, pytrix.Matrix, [], [])
        self.assertRaises(TypeError, pytrix.Matrix, [[], []])
        self.assertRaises(TypeError, pytrix.Matrix, [[[]]])
        self.assertRaises(TypeError, pytrix.Matrix, 1, 2, 3)
        self.assertRaises(TypeError, pytrix.Matrix, [[1, 2, 3]], [])
        self.assertRaises(TypeError, pytrix.Matrix, [1, 2, 3], [[1, 2, 3]])

    def testMatrixAdd(self):
        # Test typical matrix additions
        self._assertMatrixEqual(self.m1 + self.m2, [10, 10, 10], [10, 10, 10], [10, 10, 10])
        self._assertMatrixEqual(self.m2 + self.m1, [10, 10, 10], [10, 10, 10], [10, 10, 10])
        self._assertMatrixEqual(self.m1 + self.zero3, [1, 2, 3], [4, 5, 6], [7, 8, 9])
        self._assertMatrixEqual(self.zero3 + self.m2, [9, 8, 7], [6, 5, 4], [3, 2, 1])
        self._assertMatrixEqual(self.zero3 + self.zero3, [0, 0, 0], [0, 0, 0], [0, 0, 0])
        self._assertMatrixEqual(self.zero2 + self.zero2, [0, 0], [0, 0])
        self._assertMatrixEqual(self.zero1 + self.zero1, [0])
        self.assertEqual((self.e1 + self.e1).columns, 0)

        # Test erroneous matrix additions
        self.assertRaises(TypeError, lambda *x: self.m1 + 1)
        self.assertRaises(TypeError, lambda *x: 1 + self.m1)
        self.assertRaises(TypeError, lambda *x: [] + self.m1)
        self.assertRaises(TypeError, lambda *x: self.m1 + [])
        self.assertRaises(ValueError, lambda *x: self.m1 + self.zero1)
        self.assertRaises(ValueError, lambda *x: self.m1 + self.zero2)
        self.assertRaises(ValueError, lambda *x: self.e1 + self.zero1)
        self.assertRaises(ValueError, lambda *x: self.zero2 + self.e1)
        self.assertRaises(TypeError, lambda *x: self.m1 + pytrix.Vector([1, 2, 3]))

    def testMatrixSub(self):
        # Test typical matrix subtractions
        self._assertMatrixEqual(self.m1 - self.m2, [-8, -6, -4], [-2, 0, 2], [4, 6, 8])
        self._assertMatrixEqual(self.m2 - self.m1, [8, 6, 4], [2, 0, -2], [-4, -6, -8])
        self._assertMatrixEqual(self.zero1 - self.zero1, [0])
        self._assertMatrixEqual(self.zero2 - self.zero2, [0, 0], [0, 0])
        self._assertMatrixEqual(self.m1 - self.zero3, [1, 2, 3], [4, 5, 6], [7, 8, 9])
        self._assertMatrixEqual(self.zero3 - self.m1, [-1, -2, -3], [-4, -5, -6], [-7, -8, -9])
        self._assertMatrixEqual(self.m1 - (self.m2 - self.m1), [-7, -4, -1], [2, 5, 8], [11, 14, 17])
        self._assertMatrixEqual(self.zero2 - self.zero2 - self.zero2, [0, 0], [0, 0])
        self.assertEqual((self.e1 - self.e1 - self.e1).columns, 0)

        # Test erroneous matrix subtractions
        self.assertRaises(TypeError, lambda *x: self.m1 - 1)
        self.assertRaises(TypeError, lambda *x: 1 - self.m1)
        self.assertRaises(TypeError, lambda *x: [] - self.m1)
        self.assertRaises(TypeError, lambda *x: self.m1 - [])
        self.assertRaises(ValueError, lambda *x: self.m1 - self.zero2)
        self.assertRaises(ValueError, lambda *x: self.m1 - self.e1)
        self.assertRaises(ValueError, lambda *x: self.zero1 - self.e1)

    def testMatrixMul(self):
        # Test scalar matrix multiplications
        self.assertEqual(self.m1, self.m1 * 1)
        self.assertEqual(self.m1, 1 * self.m1)
        self.assertEqual(self.zero3, 0 * self.m1)
        self.assertEqual(self.zero3, self.m1 * 0)
        self._assertMatrixEqual(2 * self.m1, [2, 4, 6], [8, 10, 12], [14, 16, 18])
        self._assertMatrixEqual(self.m1 * 2, [2, 4, 6], [8, 10, 12], [14, 16, 18])
        self.assertEqual(self.zero3 * 5, self.zero3)
        self.assertEqual(5 * self.zero3, self.zero3)
        self.assertEqual(self.zero2 * 5, self.zero2)
        self.assertEqual(5 * self.zero2, self.zero2)
        self.assertEqual(self.zero1 * 5, self.zero1)
        self.assertEqual(5 * self.zero1, self.zero1)
        self.assertEqual(2 * self.e1, self.e1)
        self.assertEqual(self.e1 * 2, self.e1)

        # Test vector matrix multiplications
        v = pytrix.Vector([1, 2, 3])
        self.assertRaises(ValueError, lambda *x: v * self.zero2)
        self.assertRaises(ValueError, lambda *x: self.zero2 * v)
        self.assertRaises(ValueError, lambda *x: self.e1 * v)
        self.assertRaises(ValueError, lambda *x: v * self.e1)
        self.assertRaises(ValueError, lambda *x: pytrix.Vector([]) * self.m1)
        self.assertRaises(ValueError, lambda *x: self.m1 * pytrix.Vector([]))
        self.assertEqual(list(v * self.m1), list(pytrix.Vector([30, 36, 42])))
        self.assertEqual(v * self.zero3, pytrix.Vector([0, 0, 0]))
        self.assertEqual(list(self.m1 * v), list(pytrix.Vector([14, 32, 50])))
        self.assertEqual(self.zero3 * v, pytrix.Vector([0, 0, 0]))

        # Test matrix matrix multiplications
        self.assertEqual(self.m1 * self.zero3, self.zero3)
        self.assertEqual(self.zero3 * self.m1, self.zero3)
        self.assertNotEqual(self.m1 * self.m2, self.m2 * self.m1)
        self._assertMatrixEqual(self.m1 * self.m2, [30, 24, 18], [84, 69, 54], [138, 114, 90])
        self.assertEqual(list(self.e1 * self.e1), [])

        # Test erroneous matrix multiplications
        self.assertRaises(TypeError, lambda *x: [] * self.m1)
        self.assertRaises(TypeError, lambda *x: self.m1 * [])
        self.assertRaises(TypeError, lambda *x: self.m1 * "0")
        self.assertRaises(ValueError, lambda *x: self.m1 * self.zero2)

    def testMatrixNeg(self):
        self.assertEqual(self.e1, -self.e1)
        self.assertEqual(self.zero1, -self.zero1)
        self.assertEqual(self.zero2, -self.zero2)
        self.assertEqual(self.zero3, -self.zero3)
        self._assertMatrixEqual(-self.m1, [-1, -2, -3], [-4, -5, -6], [-7, -8, -9])


    def testMatrixTruth(self):
        self.assertTrue(self.m1)
        self.assertFalse(self.zero1)
        self.assertFalse(self.zero2)
        self.assertFalse(self.zero3)
        self.assertFalse(self.e1)

    def testMatrixGetItem(self):
        self.assertIsInstance(self.m1[0], pytrix.Vector)
        self.assertEqual(list(self.m1[0]), [1, 2, 3])
        self.assertEqual(list(self.m1[1]), [4, 5, 6])
        self.assertEqual(list(self.m1[2]), [7, 8, 9])
        self.assertEqual(self.m1[0][0], 1)
        self.assertEqual(self.m1[2][2], 9)
        self.assertRaises(IndexError, self.m1.__getitem__, 3)
        self.assertRaises(IndexError, self.e1.__getitem__, 0)

    def testMatrixTranspose(self):
        self.assertEqual(self.m1, self.m1.transpose().transpose())
        self._assertMatrixEqual(self.m1.transpose(), [1, 4, 7], [2, 5, 8], [3, 6, 9])
        self.assertEqual(self.e1.transpose(), self.e1)

    def testMatrixRow(self):
        self.assertRaises(IndexError, self.e1.row, 0)
        self.assertRaises(IndexError, self.m1.row, -1)
        self.assertRaises(IndexError, self.m1.row, 10)
        self.assertEqual(list(self.m1.row(0)), [1, 2, 3])
        self.assertEqual(list(self.m1.row(1)), [4, 5, 6])
        self.assertEqual(list(self.m1.row(2)), [7, 8, 9])
        self.assertEqual(list(self.zero1.row(0)), [0])

    def testMatrixColumn(self):
        self.assertRaises(IndexError, self.e1.column, 0)
        self.assertRaises(IndexError, self.m1.column, -1)
        self.assertRaises(IndexError, self.m1.column, 10)
        self.assertEqual(list(self.m1.column(0)), [1, 4, 7])
        self.assertEqual(list(self.m1.column(1)), [2, 5, 8])
        self.assertEqual(list(self.m1.column(2)), [3, 6, 9])
        self.assertEqual(list(self.zero1.row(0)), [0])

    def testMatrixGaussianElim(self):
        self._assertMatrixEqual(pytrix.Matrix(
                [0, 0, 0, 0],
                [0, 0, 0, 0],
                [0, 0, 0, 0],
            ).gaussianElim(),
                [0, 0, 0, 0],
                [0, 0, 0, 0],
                [0, 0, 0, 0],
        )
        self._assertMatrixEqual(pytrix.Matrix(
                [1, 0, 0, 1],
                [0, 1, 0, 1],
                [0, 0, 1, 1],
            ).gaussianElim(),
                [1, 0, 0, 1],
                [0, 1, 0, 1],
                [0, 0, 1, 1],
        )
        self._assertMatrixEqual(pytrix.Matrix(
                [1, 0, 0, 0],
                [0, 0, 0, 0],
                [0, 0, 1, 0],
            ).gaussianElim(),
                [1, 0, 0, 0],
                [0, 0, 1, 0],
                [0, 0, 0, 0],
        )
        self._assertMatrixEqual(pytrix.Matrix(
                [1, 0, 0, 0],
                [0, 0, 0, 0],
                [0, 0, 1, 0],
            ).gaussianElim(),
                [1, 0, 0, 0],
                [0, 0, 1, 0],
                [0, 0, 0, 0],
        )
        self._assertMatrixEqual(pytrix.Matrix(
                [1, 2, 3, 5],
                [0, 1, 4, 6],
                [0, 0, 1, 7],
            ).gaussianElim(),
                [1, 2, 3, 5],
                [0, 1, 4, 6],
                [0, 0, 1, 7],
        )
        self._assertMatrixEqual(pytrix.Matrix(
                [2, 2, 4, 6],
                [0, 3, 6, 9],
                [0, 0, 4, 8],
            ).gaussianElim(),
                [2, 2, 4, 6],
                [0, 3, 6, 9],
                [0, 0, 4, 8],
        )
        self._assertMatrixEqual(pytrix.Matrix(
                [2, 2, 4, 6],
                [4, 4, 8,12],
                [6, 6,12,18],
            ).gaussianElim(),
                [2, 2, 4, 6],
                [0, 0, 0, 0],
                [0, 0, 0, 0],
        )
        self._assertMatrixEqual(pytrix.Matrix(
                [1, 0, 0, 0],
                [0, 0, 1, 0],
                [1, 0, 1, 0],
            ).gaussianElim(),
                [1, 0, 0, 0],
                [0, 0, 1, 0],
                [0, 0, 0, 0],
        )
        self._assertMatrixEqual(self.m1.gaussianElim(),
                [1, 2, 3],
                [0, -3, -6],
                [0, 0, 0]
        )
        self.assertEqual(self.e1.gaussianElim(), self.e1)
        self.assertEqual(self.zero1.gaussianElim(), self.zero1)
        self.assertEqual(self.zero2.gaussianElim(), self.zero2)
        self.assertEqual(self.zero3.gaussianElim(), self.zero3)

    def testMatrixPermute(self):
        self.assertRaises(ValueError, self.e1.permute, 0, 0)
        self.assertRaises(ValueError, self.m1.permute, -1, 2)
        self.assertRaises(ValueError, self.m1.permute, -1, 5)
        self.assertRaises(ValueError, self.m1.permute, 0, 5)
        self.assertRaises(TypeError, self.m1.permute, 0, 5.5)
        self.assertRaises(TypeError, self.m1.permute, 0, "5")
        self.assertRaises(TypeError, self.m1.permute, 0, [])

        self.assertEqual(self.zero1, self.zero1.permute(0, 0))
        self.assertEqual(self.m1, self.m1.permute(0, 0))
        self.assertEqual(self.m1, self.m2.transpose().permute(0, 2).transpose().permute(0, 2))
        self._assertMatrixEqual(self.m1.permute(0, 2), [7, 8, 9], [4, 5, 6], [1, 2, 3])

    def testMatrixTrace(self):
        self.assertEqual(self.e1.trace(), 0)
        self.assertEqual(self.m1.trace(), 15)
        self.assertEqual(self.m2.trace(), 15)
        self.assertEqual(self.zero1.trace(), 0)
        self.assertEqual(self.zero2.trace(), 0)
        self.assertEqual(self.zero3.trace(), 0)
        self.assertEqual(pytrix.Matrix([[1, 1, 1], [2, 3, 5], [4, 6, 8]]).trace(), 12)
        self.assertEqual(pytrix.Matrix([[1, 1, 1], [2, 2, 5], [4, 6, 8]]).trace(), 11)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 3, 4], [5, 6, 7, 8]]).trace),
        self.assertEqual(pytrix.Matrix([[0, 0, 0], [1, 0, 0], [0, 0, 0]]).trace(), 0)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]]).trace),
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]]).trace),
        self.assertEqual(pytrix.Matrix([[2, -1, 3], [4, 2, 1], [-6, -1, 2]]).trace(), 6)
        self.assertRaises(ValueError, pytrix.Matrix([[0, 2, -6, -2, 4], [0, -1, 3, 3, 2], [0, -1, 3, 7, 10]]).trace),
        self.assertEqual(pytrix.Matrix([[1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]]).trace(), 10)
        self.assertRaises(ValueError, pytrix.Matrix([[1], [2], [3], [4]]).trace)

    def testMatrixSymmetrical(self):
        self.assertTrue(self.e1.isSymmetrical())
        self.assertTrue(self.zero1.isSymmetrical())
        self.assertTrue(self.zero2.isSymmetrical())
        self.assertTrue(self.zero3.isSymmetrical())
        self.assertFalse(self.m1.isSymmetrical())
        self.assertFalse(self.m2.isSymmetrical())

        self.assertFalse(pytrix.Matrix([1, 2, 3]).isSymmetrical())
        self.assertFalse(pytrix.Matrix([1, 2, 3], [4, 5, 6]).isSymmetrical())

    def testMatrixRank(self):
        matrices = [
            (self.e1, 0),
            (self.m1, 2),
            (self.m2, 2),
            (self.zero1, 0),
            (self.zero2, 0),
            (self.zero3, 0),
            ([[1, 1, 1], [2, 3, 5], [4, 6, 8]], 3),
            ([[1, 2, 3, 4], [5, 6, 7, 8]], 2),
            ([[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]], 2),
            ([[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]], 2),
            ([[2, -1, 3], [4, 2, 1], [ -6, -1, 2]], 3),
            ([[0, 2, -6, -2, 4], [0, -1, 3, 3, 2], [0, -1, 3, 7, 10]], 2),
            ([[1], [2], [3], [4]], 1)
        ]

        for m, rank in matrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            self.assertEqual(m.rank(), rank)

    def testMatrixDeterminant(self):
        matrices = [
            (self.e1, 1),
            (self.m1, 0),
            (self.m2, 0),
            (self.zero1, 0),
            (self.zero2, 0),
            (self.zero3, 0),
            ([[0]], 0),
            ([[2]], 2),
            ([[1, 1], [2, 3]], 1),
            ([[1, 1], [2, 2]], 0),
            ([[2, 0], [7, 0]], 0),
            ([[1, 1, 1], [2, 2, 5], [4, 6, 8]], -6),
            ([[1, 1, 1], [2, 3, 5], [4, 6, 8]], -2),
            ([[0, 0, 0], [1, 0, 0], [0, 0, 0]], 0),
            ([[2, -1, 3], [4, 2, 1], [ -6, -1, 2]], 48),
            ([[1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]], -4),
            ([[1, 4, 0, 3], [1, 2, 0, 0], [2, 6, 0, 1], [0, 0, 0, 4]], 0),
        ]

        for m, determinant in matrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)

            self.assertEqual(m.determinant(), determinant)

        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 3, 4], [5, 6, 7, 8]]).determinant)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]]).determinant)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]]).determinant)
        self.assertRaises(ValueError, pytrix.Matrix([[1], [2], [3], [4]]).determinant)
        self.assertRaises(ValueError, pytrix.Matrix([[0, 2, -6, -2, 4], [0, -1, 3, 3, 2], [0, -1, 3, 7, 10]]).determinant)

    def testMatrixInverse(self):
        matrices = [
            self.e1,
            [[1, 0], [0, 2]],
            [[1, 1, 1], [2, 3, 5], [4, 6, 8]],
            [[1, 1, 1], [2, 2, 5], [4, 6, 8]],
            [[2, -1, 3], [4, 2, 1], [ -6, -1, 2]],
            [[1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]],
        ]

        for m in matrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            i = m.inverse()
            self._assertMatrixEqualWithDelta(i.inverse(), m)

        self.assertRaises(ValueError, self.zero1.inverse)
        self.assertRaises(ValueError, self.zero2.inverse)
        self.assertRaises(ValueError, self.zero3.inverse)
        self.assertRaises(ValueError, self.m1.inverse)
        self.assertRaises(ValueError, self.m2.inverse)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 0], [2, 0]]).inverse)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 3, 4], [5, 6, 7, 8]]).inverse)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]]).inverse)
        self.assertRaises(ValueError, pytrix.Matrix([[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]]).inverse)
        self.assertRaises(ValueError, pytrix.Matrix([[1], [2], [3], [4]]).inverse)
        self.assertRaises(ValueError, pytrix.Matrix([[0, 0, 0], [1, 0, 0], [0, 0, 0]]).inverse)

    def testMatrixInvertible(self):
        invertibleMatrices = [
            self.e1,
            [[1]],
            [[1, 0], [0, 2]],
            [[1, 1, 1], [2, 3, 5], [4, 6, 8]],
            [[1, 1, 1], [2, 2, 5], [4, 6, 8]],
            [[2, -1, 3], [4, 2, 1], [ -6, -1, 2]],
            [[1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]],
        ]
        nonInvertibleMatrices = [
            self.zero1,
            self.zero2,
            self.zero3,
            self.m1,
            self.m2,
            [[0]],
            [[1, 0], [2, 0]],
            [[1, 2, 3, 4], [5, 6, 7, 8]],
            [[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]],
            [[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]],
            [[1], [2], [3], [4]],
            [[0, 0, 0], [1, 0, 0], [0, 0, 0]]
        ]

        for m in invertibleMatrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            self.assertTrue(m.isInvertible())
        for m in nonInvertibleMatrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            self.assertFalse(m.isInvertible())

    def testMatrixFactorLU(self):
        matrices = [
            self.e1,
            self.m1,
            self.m2,
            self.zero1,
            self.zero2,
            self.zero3,
            [[1, 1, 1], [2, 3, 5], [4, 6, 8]],
            [[1, 2, 3, 4], [5, 6, 7, 8]],
            [[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]],
            [[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]],
            [[2, -1, 3], [4, 2, 1], [ -6, -1, 2]],
            [[0, 2, -6, -2, 4], [0, -1, 3, 3, 2], [0, -1, 3, 7, 10]],
            [[1], [2], [3], [4]]
        ]
        for m in matrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            l, u = m.factorLU()
            self._assertMatrixEqualWithDelta(m, l * u)

        self.assertRaises(ValueError, pytrix.Matrix([1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]).factorLU)
        self.assertRaises(ValueError, pytrix.Matrix([1, 1, 1], [2, 2, 5], [4, 6, 8]).factorLU)
        self.assertRaises(ValueError, pytrix.Matrix([0, 0, 0], [1, 0, 0], [0, 0, 0]).factorLU)

    def testMatrixFactorLDU(self):
        matrices = [
            self.e1,
            self.m1,
            self.m2,
            self.zero1,
            self.zero2,
            self.zero3,
            [[1, 1, 1], [2, 3, 5], [4, 6, 8]],
            [[1, 2, 3, 4], [5, 6, 7, 8]],
            [[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]],
            [[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]],
            [[2, -1, 3], [4, 2, 1], [ -6, -1, 2]],
            [[0, 2, -6, -2, 4], [0, -1, 3, 3, 2], [0, -1, 3, 7, 10]],
            [[1], [2], [3], [4]]
        ]
        for m in matrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            l, d, u = m.factorLDU()
            self._assertMatrixEqualWithDelta(m, (l * d) * u)
            self._assertMatrixEqualWithDelta(m, l * (d * u))
            for i in range(d.rows):
                for j in range(d.columns):
                    if i != j:
                        self.assertEqual(d[i][j], 0)
            for i in range(min(m.rows, m.columns)):
                self.assertEqual(l[i][i], 1)
                self.assertIn(u[i][i], (0, 1))

        self.assertRaises(ValueError, pytrix.Matrix([1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]).factorLDU)
        self.assertRaises(ValueError, pytrix.Matrix([1, 1, 1], [2, 2, 5], [4, 6, 8]).factorLDU)
        self.assertRaises(ValueError, pytrix.Matrix([0, 0, 0], [1, 0, 0], [0, 0, 0]).factorLDU)

    def testMatrixFactorPLU(self):
        matrices = [
            self.e1,
            self.m1,
            self.m2,
            self.zero1,
            self.zero2,
            self.zero3,
            [[1, 1, 1], [2, 3, 5], [4, 6, 8]],
            [[1, 1, 1], [2, 2, 5], [4, 6, 8]],
            [[1, 2, 3, 4], [5, 6, 7, 8]],
            [[0, 0, 0], [1, 0, 0], [0, 0, 0]],
            [[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]],
            [[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]],
            [[2, -1, 3], [4, 2, 1], [ -6, -1, 2]],
            [[0, 2, -6, -2, 4], [0, -1, 3, 3, 2], [0, -1, 3, 7, 10]],
            [[1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]],
            [[1], [2], [3], [4]]
        ]
        for m in matrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            p, l, u = m.factorPLU()
            self._assertMatrixEqualWithDelta(m, (p * l) * u)
            self._assertMatrixEqualWithDelta(m, p * (l * u))
            # Assert that for matrices which can be LU factored, p is the identity matrix
            try:
                m.factorLU()
                p, _, _ = m.factorPLU()
                self.assertTrue(p.isIdentity())

            except ValueError:
                # Otherwise it is NOT the identity matrix
                p, _, _ = m.factorPLU()
                self.assertFalse(p.isIdentity())


    def testMatrixFactorPLDU(self):
        matrices = [
            self.e1,
            self.m1,
            self.m2,
            self.zero1,
            self.zero2,
            self.zero3,
            [[1, 1, 1], [2, 3, 5], [4, 6, 8]],
            [[1, 1, 1], [2, 2, 5], [4, 6, 8]],
            [[1, 2, 3, 4], [5, 6, 7, 8]],
            [[0, 0, 0], [1, 0, 0], [0, 0, 0]],
            [[1, 2, 0, 2], [3, 6, -1, 8], [1, 2, 1, 0]],
            [[1, 2, 3], [4, 5, 6], [7, 8, 9], [20, 25, 30]],
            [[2, -1, 3], [4, 2, 1], [ -6, -1, 2]],
            [[0, 2, -6, -2, 4], [0, -1, 3, 3, 2], [0, -1, 3, 7, 10]],
            [[1, 4, 2, 3], [1, 2, 1, 0], [2, 6, 3, 1], [0, 0, 1, 4]],
            [[1], [2], [3], [4]]
        ]
        for m in matrices:
            if not isinstance(m, pytrix.Matrix):
                m = pytrix.Matrix(m)
            p, l, d, u = m.factorPLDU()
            self._assertMatrixEqualWithDelta(m, (p * l) * (d * u))
            self._assertMatrixEqualWithDelta(m, p * ((l * d) * u))
            self._assertMatrixEqualWithDelta(m, (p * (l * d)) * u)
            self._assertMatrixEqualWithDelta(m, p * (l * (d * u)))
            for i in range(d.rows):
                for j in range(d.columns):
                    if i != j:
                        self.assertEqual(d[i][j], 0)
            for i in range(min(m.rows, m.columns)):
                self.assertEqual(l[i][i], 1)
                self.assertIn(u[i][i], (0, 1))
            # Assert that for matrices which can be LU factored, p is the identity matrix
            try:
                m.factorLU()
                p, _, _, _ = m.factorPLDU()
                self.assertTrue(p.isIdentity())

            except ValueError:
                # Otherwise it is NOT the identity matrix
                p, _, _, _ = m.factorPLDU()
                self.assertFalse(p.isIdentity())
