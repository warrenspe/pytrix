# Standard imports
import math

# Project imports
import pytrix
import tests

class TestVector(tests.PytrixTestCase):

    def setUp(self):
        self.e1 = pytrix.Vector([])
        self.zero0 = pytrix.Vector([0])
        self.zero1 = pytrix.Vector([0, 0])
        self.zero2 = pytrix.Vector([0, 0, 0])
        self.v1 = pytrix.Vector([1, 2, 3, 4, 5, 6, 7, 8, 9, 0])
        self.v2 = pytrix.Vector([0, 9, 8, 7, 6, 5, 4, 3, 2, 1])
        self.p0 = pytrix.Point([])
        self.p1 = pytrix.Point(1)
        self.p2 = pytrix.Point(1, 2)
        self.p3 = pytrix.Point(1, 2, 3)


    def testVectorInit(self):
        self.assertRaises(TypeError, pytrix.Vector)
        self.assertRaises(TypeError, pytrix.Vector, None)
        self.assertRaises(TypeError, pytrix.Vector, '1')
        self.assertRaises(TypeError, pytrix.Vector, [[1, 2, 3]])
        self.assertRaises(TypeError, pytrix.Vector, pytrix.Point(1, 2, 3))
        self.assertEqual(len(list(pytrix.Vector([1, 2, 3]))), 3)
        self.assertEqual(len(list(pytrix.Vector(1, 2, 3))), 3)
        self.assertEqual(list(pytrix.Vector(1, 2, 3, 4, 5, 6)), [1, 2, 3, 4, 5, 6])


    def testVectorAdd(self):
        # Test typical vector additions
        self.assertEqual(list(self.v1 + self.v2), [1, 11, 11, 11, 11, 11, 11, 11, 11, 1])
        self.assertEqual(list(self.v1 + self.v1 + self.v1), [3, 6, 9, 12, 15, 18, 21, 24, 27, 0])
        self.assertEqual(list(self.e1 + self.e1 + self.e1), [])
        self.assertEqual(list(self.zero0 + self.zero0 + self.zero0), [0])
        self.assertEqual(list(self.zero1 + self.zero1 + self.zero1), [0, 0])
        self.assertEqual(list(self.zero2 + self.zero2 + self.zero2), [0, 0, 0])
        self.assertEqual(self.v1 + self.v2, self.v2 + self.v1)

        # Test Vector + Point additions
        self.assertEqual(self.e1 + self.p0, self.p0)
        self.assertEqual(self.zero0 + self.p1, self.p1)
        self.assertEqual(self.zero1 + self.p2, self.p2)
        self.assertEqual(self.zero2 + self.p3, self.p3)
        self.assertEqual(self.zero2 + self.p3 + self.zero2, self.p3)
        self.assertRaises(ValueError, lambda *x: self.v1 + self.p3)
        self.assertRaises(ValueError, lambda *x: self.zero0 + self.p2)
        self.assertRaises(ValueError, lambda *x: self.zero1 + self.p3)
        self.assertRaises(ValueError, lambda *x: self.zero1 + self.p3)

        self.assertEqual(self.p0 + self.e1, self.p0)
        self.assertEqual(self.p1 + self.zero0, self.p1)
        self.assertEqual(self.p2 + self.zero1, self.p2)
        self.assertEqual(self.p3 + self.zero2, self.p3)
        self.assertRaises(ValueError, lambda *x: self.p3 + self.v1)
        self.assertRaises(ValueError, lambda *x: self.p2 + self.zero0)
        self.assertRaises(ValueError, lambda *x: self.p3 + self.zero1)
        self.assertRaises(ValueError, lambda *x: self.p3 + self.zero1)

        # Test erroneous vector additions
        self.assertRaises(TypeError, lambda *x: self.v1 + 1)
        self.assertRaises(TypeError, lambda *x: 1 + self.v1)
        self.assertRaises(TypeError, lambda *x: [] + self.v1)
        self.assertRaises(TypeError, lambda *x: self.v1 + [])

        self.assertRaises(ValueError, lambda *x: self.v1 + self.zero2)
        self.assertRaises(ValueError, lambda *x: self.v1 + self.e1)
        self.assertRaises(ValueError, lambda *x: self.zero0 + self.e1)

    def testVectorSub(self):
        # Test typical vector subtractions
        self.assertEqual(list(self.v1 - self.v2), [1, -7, -5, -3, -1, 1, 3, 5, 7, -1])
        self.assertEqual(self.v1 - (self.v1 - self.v1), self.v1)
        self.assertEqual(list(self.e1 - self.e1 - self.e1), [])
        self.assertEqual(list(self.zero0 - self.zero0 - self.zero0), [0])
        self.assertEqual(list(self.zero1 - self.zero1 - self.zero1), [0, 0])
        self.assertEqual(list(self.zero2 - self.zero2 - self.zero2), [0, 0, 0])
        self.assertEqual(self.v1 - self.v2, -(self.v2 - self.v1))

        # Test erroneous vector subtractions
        self.assertRaises(TypeError, lambda *x: self.zero2 - self.p3)
        self.assertRaises(TypeError, lambda *x: self.v1 - 1)
        self.assertRaises(TypeError, lambda *x: 1 - self.v1)
        self.assertRaises(TypeError, lambda *x: [] - self.v1)
        self.assertRaises(TypeError, lambda *x: self.v1 - [])

        self.assertRaises(ValueError, lambda *x: self.v1 - self.zero2)
        self.assertRaises(ValueError, lambda *x: self.v1 - self.e1)
        self.assertRaises(ValueError, lambda *x: self.zero0 - self.e1)

    def testVectorMul(self):
        # Test typical vector multiplications
        self.assertEqual(list(2 * self.v1), [2, 4, 6, 8, 10, 12, 14, 16, 18, 0])
        self.assertEqual(list(2 * (2 * self.v1)), [4, 8, 12, 16, 20, 24, 28, 32, 36, 0])
        self.assertEqual(list(0 * (2 * self.v1)), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
        self.assertEqual(list(5 * self.e1), [])
        self.assertEqual(list(5 * self.zero0), [0])
        self.assertEqual(5 * self.v2, self.v2 * 5)
        self.assertEqual(pytrix.Vector(1, 2, 3) * pytrix.Matrix([1, 2, 3], [4, 5, 6]), pytrix.Vector(14, 32))

        # Test erroneous vector multiplications
        self.assertRaises(TypeError, lambda *x: [] * self.v1)
        self.assertRaises(TypeError, lambda *x: self.v1 * [])
        self.assertRaises(TypeError, lambda *x: self.v1 * "0")
        self.assertRaises(TypeError, lambda *x: self.v1 * self.v2)
        self.assertRaises(TypeError, lambda *x: self.zero2 * self.p3)

    def testVectorDiv(self):
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
        self.assertRaises(TypeError, lambda *x: self.zero2 / self.p3)

    def testVectorNeg(self):
        self.assertEqual(self.e1, -self.e1)
        self.assertEqual(self.zero0, -self.zero0)
        self.assertEqual(self.zero1, -self.zero1)
        self.assertEqual(self.zero2, -self.zero2)
        self.assertEqual(list(-self.v1), [-1, -2, -3, -4, -5, -6, -7, -8, -9, 0])


    def testVectorTruth(self):
        self.assertTrue(self.v1)
        self.assertFalse(self.zero0)
        self.assertFalse(self.zero1)
        self.assertFalse(self.zero2)
        self.assertFalse(self.e1)

    def testVectorDimensions(self):
        self.assertEqual(self.e1.dimensions, 0)
        self.assertEqual(self.zero0.dimensions, 1)
        self.assertEqual(self.zero1.dimensions, 2)
        self.assertEqual(self.zero2.dimensions, 3)
        self.assertEqual(self.v1.dimensions, 10)
        self.assertEqual(self.v2.dimensions, 10)

    def testVectorCopy(self):
        self.assertEqual(self.v1, self.v1.copy())
        self.assertEqual(self.v2, self.v2.copy())
        self.assertEqual(self.e1, self.e1.copy())
        self.assertEqual(self.zero0, self.zero0.copy())
        self.assertEqual(self.zero1, self.zero1.copy())
        self.assertEqual(self.zero2, self.zero2.copy())
        self.assertEqual(id(self.v1), id(self.v1))
        self.assertNotEqual(id(self.v1), id(self.v1.copy()))

    def testVectorDot(self):
        self.assertEqual(self.v1.dot(self.v2), sum(map(lambda x, y: x * y, list(self.v1), list(self.v2))))

        self.assertEqual(self.v1.dot(self.v2), 200)
        self.assertEqual(self.e1.dot(self.e1), 0)
        self.assertEqual(self.zero0.dot(self.zero0), 0)
        self.assertEqual(self.zero1.dot(self.zero1), 0)
        self.assertEqual(self.zero2.dot(self.zero2), 0)

        self.assertRaises(ValueError, self.zero1.dot, self.zero2)
        self.assertRaises(TypeError, self.zero1.dot, 1)
        self.assertRaises(TypeError, self.zero1.dot, [])
        self.assertRaises(TypeError, self.zero1.dot, "")
        self.assertRaises(TypeError, self.zero1.dot, self.p1)

    def testVectorCross(self):
        c1 = pytrix.Vector([1, 2, 3])
        c2 = pytrix.Vector([3, 2, 1])

        self.assertEqual(list(c1.cross(c2)), [-4, 8, -4])

        self.assertRaises(ValueError, self.v1.cross, self.v2)
        self.assertRaises(ValueError, self.zero1.cross, self.zero2)
        self.assertRaises(TypeError, self.zero1.cross, 1)
        self.assertRaises(TypeError, self.zero1.cross, [])
        self.assertRaises(TypeError, self.zero1.cross, "")
        self.assertRaises(TypeError, self.zero1.cross, self.p1)

    def testVectorLength(self):
        self.assertAlmostEqual(self.v1.length(), 16.88, delta=.01)
        self.assertAlmostEqual(self.v2.length(), 16.88, delta=.01)
        self.assertEqual(self.e1.length(), 0)
        self.assertEqual(self.zero0.length(), 0)
        self.assertEqual(self.zero1.length(), 0)
        self.assertEqual(self.zero2.length(), 0)

    def testVectorUnit(self):
        self.assertAlmostEqual(self.v1.unit().length(), 1, delta=.00001)
        self.assertAlmostEqual(self.v2.unit().length(), 1, delta=.00001)
        self.assertRaises(ValueError, self.zero0.unit)
        self.assertRaises(ValueError, self.zero1.unit)
        self.assertRaises(ValueError, self.zero2.unit)
        self.assertRaises(ValueError, self.e1.unit)

    def testVectorIsUnit(self):
        self.assertFalse(self.v1.isUnit())
        self.assertFalse(self.v1.isUnit())
        self.assertTrue(self.v1.unit().isUnit())
        self.assertTrue(self.v2.unit().isUnit())
        self.assertFalse(self.zero0.isUnit())
        self.assertFalse(self.zero1.isUnit())
        self.assertFalse(self.zero2.isUnit())
        self.assertFalse(self.e1.isUnit())

    def testVectorAngleBetween(self):
        v1 = pytrix.Vector([1, 0])
        v2 = pytrix.Vector([0, -1])
        v3 = pytrix.Vector([-1, 0])
        v4 = pytrix.Vector([0, 1])

        self.assertAlmostEqual(v1.angleBetween(v3), math.pi, delta=.00001)
        self.assertAlmostEqual(v3.angleBetween(v1), math.pi, delta=.00001)
        self.assertAlmostEqual(v2.angleBetween(v4), math.pi, delta=.00001)
        self.assertAlmostEqual(v4.angleBetween(v2), math.pi, delta=.00001)
        self.assertAlmostEqual(v1.angleBetween(v2), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(v1.angleBetween(v4), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(v2.angleBetween(v1), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(v2.angleBetween(v3), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(v3.angleBetween(v2), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(v3.angleBetween(v4), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(v4.angleBetween(v1), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(v4.angleBetween(v3), math.pi / 2, delta=.00001)

        va = pytrix.Vector([1, 73, 129])
        vb = pytrix.Vector([124, 6932, 1259])
        self.assertAlmostEqual(va.cross(vb).angleBetween(va), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(va.cross(vb).angleBetween(vb), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(vb.cross(va).angleBetween(va), math.pi / 2, delta=.00001)
        self.assertAlmostEqual(vb.cross(va).angleBetween(vb), math.pi / 2, delta=.00001)

        self.assertRaises(TypeError, va.cross, 1)
        self.assertRaises(TypeError, va.cross, [])
        self.assertRaises(TypeError, va.cross, "")
        self.assertRaises(TypeError, va.cross, self.p2)

    def testVectorOrthogonal(self):
        v1 = pytrix.Vector([125, 6093, 125])
        v2 = pytrix.Vector([120, 1320, 2503])
        self.assertFalse(self.v1.isOrthogonal(self.v2))

        self.assertTrue(v1.isOrthogonal(v1.cross(v2)))
        self.assertTrue(v2.isOrthogonal(v2.cross(v1)))

        v1 = pytrix.Vector([1, 0])
        v2 = pytrix.Vector([0, -1])
        v3 = pytrix.Vector([-1, 0])
        v4 = pytrix.Vector([0, 1])

        self.assertTrue(v1.isOrthogonal(v2))
        self.assertTrue(v1.isOrthogonal(v4))
        self.assertTrue(v2.isOrthogonal(v1))
        self.assertTrue(v2.isOrthogonal(v3))
        self.assertTrue(v3.isOrthogonal(v2))
        self.assertTrue(v3.isOrthogonal(v4))
        self.assertTrue(v4.isOrthogonal(v1))
        self.assertTrue(v4.isOrthogonal(v3))

        self.assertRaises(TypeError, v1.isOrthogonal, 1)
        self.assertRaises(TypeError, v1.isOrthogonal, [])
        self.assertRaises(TypeError, v1.isOrthogonal, "")
        self.assertRaises(TypeError, v1.isOrthogonal, self.p3)

    def testVectorGetItem(self):
        self.assertEqual(self.v1[0], 1)
        self.assertEqual(self.v1[1], 2)
        self.assertEqual(self.v1[2], 3)
        self.assertEqual(self.v1[3], 4)
        self.assertEqual(self.v1[4], 5)
        self.assertEqual(self.v1[5], 6)
        self.assertRaises(IndexError, self.v1.__getitem__, 11)
        self.assertRaises(IndexError, self.e1.__getitem__, 0)
        self.assertRaises(TypeError, self.v1.__getitem__, self.p3)
