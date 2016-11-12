# Standard imports
import math

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
        self.assertEqual(matrix.columns, len(lists))
        self.assertEqual(matrix.rows, len(lists[0]))
        for i, v in enumerate(matrix):
            self.assertEqual(list(v), lists[i])

    def testMatrixInit(self):
        self.assertRaises(TypeError, pytrix.Matrix, None)
        self.assertRaises(TypeError, pytrix.Matrix, [None])
        self.assertRaises(TypeError, pytrix.Matrix, [[1, 2], None])
        self.assertRaises(TypeError, pytrix.Matrix, 1)
        self.assertRaises(TypeError, pytrix.Matrix, [1])
        self.assertRaises(TypeError, pytrix.Matrix, "abc")
        self.assertRaises(TypeError, pytrix.Matrix, [[1, 2], [1]])
        self.assertEqual(pytrix.Matrix([[1, 2], [3, 4], [5, 6]]).rows, 3)
        self.assertEqual(pytrix.Matrix([[1, 2, 3], [4, 5, 6]]).columns, 3)

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

    def testVectorSub(self):
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

    def testVectorMul(self):
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


        # Test erroneous matrix multiplications
        self.assertRaises(TypeError, lambda *x: [] * self.m1)
        self.assertRaises(TypeError, lambda *x: self.m1 * [])
        self.assertRaises(TypeError, lambda *x: self.m1 * "0")
        self.assertRaises(ValueError, lambda *x: self.m1 * self.zero2)

    def _testVectorDiv(self):
        # Test typical vector divisions
        self.assertEqual(list(self.v1 / 2), [.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 0])
        self.assertEqual(list(self.e1 / 5), [])
        self.assertEqual(list(self.zero0 / 5), [0])

        # Test erroneous vector divisions
        self.assertRaises(TypeError, lambda *x: [] / self.v1)
        self.assertRaises(TypeError, lambda *x: self.v1 / [])
        self.assertRaises(TypeError, lambda *x: self.v1 / "0")
        self.assertRaises(TypeError, lambda *x: self.v1 / self.v2)
        self.assertRaises(TypeError, lambda *x: 5 / self.v1)
        self.assertRaises(ZeroDivisionError, lambda *x: self.zero0 / 0)

    def _testVectorNeg(self):
        self.assertEqual(self.e1, -self.e1)
        self.assertEqual(self.zero0, -self.zero0)
        self.assertEqual(self.zero1, -self.zero1)
        self.assertEqual(self.zero2, -self.zero2)
        self.assertEqual(list(-self.v1), [-1, -2, -3, -4, -5, -6, -7, -8, -9, 0])


    def _testVectorTruth(self):
        self.assertTrue(self.v1)
        self.assertFalse(self.zero0)
        self.assertFalse(self.zero1)
        self.assertFalse(self.zero2)
        self.assertFalse(self.e1)

    def _testVectorDimensions(self):
        self.assertEqual(self.e1.dimensions, 0)
        self.assertEqual(self.zero0.dimensions, 1)
        self.assertEqual(self.zero1.dimensions, 2)
        self.assertEqual(self.zero2.dimensions, 3)
        self.assertEqual(self.v1.dimensions, 10)
        self.assertEqual(self.v2.dimensions, 10)

    def _testVectorCopy(self):
        self.assertEqual(self.v1, self.v1.copy())
        self.assertEqual(self.v2, self.v2.copy())
        self.assertEqual(self.e1, self.e1.copy())
        self.assertEqual(self.zero0, self.zero0.copy())
        self.assertEqual(self.zero1, self.zero1.copy())
        self.assertEqual(self.zero2, self.zero2.copy())
        self.assertEqual(id(self.v1), id(self.v1))
        self.assertNotEqual(id(self.v1), id(self.v1.copy()))


