# Standard imports
import math

# Project imports
import pytrix
import tests

class TestPoint(tests.PytrixTestCase):

    def setUp(self):
        self.p0 = pytrix.Point([])
        self.p1 = pytrix.Point(1)
        self.p2 = pytrix.Point(1, 2)
        self.p3 = pytrix.Point(1, 2, 3)
        self.v0 = pytrix.Vector([])
        self.v1 = pytrix.Vector([1])
        self.v2 = pytrix.Vector([1, 2])
        self.v3 = pytrix.Vector([1, 2, 3])


    def testPointInit(self):
        self.assertRaises(TypeError, pytrix.Point)
        self.assertRaises(TypeError, pytrix.Point, None)
        self.assertRaises(TypeError, pytrix.Point, '1')
        self.assertRaises(TypeError, pytrix.Point, [[1, 2, 3]])
        self.assertRaises(TypeError, pytrix.Point, pytrix.Point(1, 2, 3))
        self.assertEqual(len(list(pytrix.Point([1, 2, 3]))), 3)
        self.assertEqual(len(list(pytrix.Point(1, 2, 3))), 3)
        self.assertEqual(list(pytrix.Point(1, 2, 3, 4, 5, 6)), [1, 2, 3, 4, 5, 6])


    def testPointAdd(self):
        # Point / Vector additions tested in test_vector.py

        # Test erroneous point additions
        self.assertRaises(TypeError, lambda *x: self.p0 + 1)
        self.assertRaises(TypeError, lambda *x: 1 + self.p1)
        self.assertRaises(TypeError, lambda *x: [] + self.p1)
        self.assertRaises(TypeError, lambda *x: self.p1 + [])

        self.assertRaises(ValueError, lambda *x: self.p1 + self.v2)
        self.assertRaises(ValueError, lambda *x: self.v2 + self.p0)
        self.assertRaises(ValueError, lambda *x: self.v3 + self.p2)

    def testPointSub(self):
        # Test typical point subtractions
        self.assertEqual(self.p0 - self.v0, self.p0)
        self.assertEqual(self.p1 - self.v1, pytrix.Point(0))
        self.assertEqual(self.p2 - self.v2, pytrix.Point(0, 0))
        self.assertEqual(self.p3 - self.v3, pytrix.Point(0, 0, 0))

        # Test erroneous point subtractions
        self.assertRaises(TypeError, lambda *x: self.v0 - self.p0)
        self.assertRaises(TypeError, lambda *x: self.v1 - self.p1)
        self.assertRaises(TypeError, lambda *x: 1 - self.p1)
        self.assertRaises(TypeError, lambda *x: self.p1 - 1)
        self.assertRaises(TypeError, lambda *x: [] - self.p1)
        self.assertRaises(TypeError, lambda *x: self.p1 - [])

    def testPointMul(self):
        # Assert that multiplication is not a valid operation on points
        self.assertRaises(TypeError, lambda *x: self.p1 * self.p1)
        self.assertRaises(TypeError, lambda *x: self.p1 * self.v1)
        self.assertRaises(TypeError, lambda *x: self.p1 * pytrix.Matrix([0]))
        self.assertRaises(TypeError, lambda *x: self.p1 * 1)
        self.assertRaises(TypeError, lambda *x: self.p0 * 0)

    def testPointDiv(self):
        # Assert that division is not a valid operation on points
        self.assertRaises(TypeError, lambda *x: self.p1 / self.p1)
        self.assertRaises(TypeError, lambda *x: self.p1 / self.v1)
        self.assertRaises(TypeError, lambda *x: self.p1 / pytrix.Matrix([0]))
        self.assertRaises(TypeError, lambda *x: self.p1 / 1)
        self.assertRaises(TypeError, lambda *x: self.p0 / 0)

    def testPointNeg(self):
        self.assertEqual(self.p0, --self.p0)
        self.assertEqual(self.p1, --self.p1)
        self.assertEqual(self.p2, --self.p2)
        self.assertEqual(self.p3, --self.p3)
        self.assertEqual(list(-self.p3), [-1, -2, -3])

    def testPointTruth(self):
        self.assertFalse(self.p0)
        self.assertFalse(pytrix.Point(0))
        self.assertTrue(self.p1)
        self.assertTrue(self.p2)
        self.assertTrue(self.p3)

    def testPointDimensions(self):
        self.assertEqual(self.p0.dimensions, 0)
        self.assertEqual(self.p1.dimensions, 1)
        self.assertEqual(self.p2.dimensions, 2)
        self.assertEqual(self.p3.dimensions, 3)

    def testPointCopy(self):
        self.assertEqual(self.p0, self.p0.copy())
        self.assertEqual(self.p1, self.p1.copy())
        self.assertEqual(self.p2, self.p2.copy())
        self.assertEqual(self.p3, self.p3.copy())
        self.assertEqual(id(self.p1), id(self.p1))
        self.assertNotEqual(id(self.p1), id(self.p1.copy()))

    def testPointGetItem(self):
        self.assertEqual(self.p1[0], 1)
        self.assertEqual(self.p2[0], 1)
        self.assertEqual(self.p2[1], 2)
        self.assertEqual(self.p3[0], 1)
        self.assertEqual(self.p3[1], 2)
        self.assertEqual(self.p3[2], 3)
        self.assertRaises(IndexError, self.p1.__getitem__, 11)
        self.assertRaises(IndexError, self.p1.__getitem__, -1)
        self.assertRaises(IndexError, self.p0.__getitem__, 0)
        self.assertRaises(TypeError, self.v1.__getitem__, self.p3)
