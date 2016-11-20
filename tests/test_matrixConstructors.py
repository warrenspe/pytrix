# Standard imports
import math

# Project imports
import pytrix
import tests

class TestMatrixConstructors(tests.PytrixTestCase):

    def testIdentityMatrix(self):
        for d in range(50):
            i = pytrix.identityMatrix(d)
            self.assertTrue(i.isIdentity())
            self.assertEqual(i.rows, i.columns)
            self.assertEqual(i.rows, d)

        self.assertRaises(TypeError, pytrix.identityMatrix, [])
        self.assertRaises(ValueError, pytrix.identityMatrix, -1)
        self.assertRaises(TypeError, pytrix.identityMatrix, "2")
        self.assertRaises(TypeError, pytrix.identityMatrix, 4.5)

    def testRotation2DMatrix(self):
        xp = pytrix.Vector(2, 0)
        yp = pytrix.Vector(0, 2)
        xn = pytrix.Vector(-2, 0)
        yn = pytrix.Vector(0, -2)

        for v in (xp, yp, xn, yn):
            for r in (0, math.pi/2, math.pi, (2.0 / 3) * math.pi, 1, 2, 3):
                for sign in (1, -1):
                    vr = pytrix.rotation2DMatrix(sign * r) * v
                    self.assertAlmostEqual(v.angleBetween(vr), r, delta=.01)

        self.assertRaises(TypeError, pytrix.rotation2DMatrix, "1")
        self.assertRaises(TypeError, pytrix.rotation2DMatrix, [])

    def testRotation3DMatrix(self):
        v = pytrix.Vector(1, 0, 0)
        r = pytrix.rotation3DMatrix(math.pi, 0, 0)
        self.assertEqual(v, r * v)
        r = pytrix.rotation3DMatrix(0, math.pi, 0)
        self.assertAlmostEqual((r * v).angleBetween(v), math.pi, delta=.01)
        r = pytrix.rotation3DMatrix(0, 0, math.pi)
        self.assertAlmostEqual((r * v).angleBetween(v), math.pi, delta=.01)

        v = pytrix.Vector(0, 1, 0)
        r = pytrix.rotation3DMatrix(0, math.pi, 0)
        self.assertEqual(v, r * v)
        r = pytrix.rotation3DMatrix(0, 0, math.pi)
        self.assertAlmostEqual((r * v).angleBetween(v), math.pi, delta=.01)
        r = pytrix.rotation3DMatrix(math.pi, 0, 0)
        self.assertAlmostEqual((r * v).angleBetween(v), math.pi, delta=.01)

        v = pytrix.Vector(0, 0, 1)
        r = pytrix.rotation3DMatrix(0, 0, math.pi)
        self.assertEqual(v, r * v)
        r = pytrix.rotation3DMatrix(0, math.pi, 0)
        self.assertAlmostEqual((r * v).angleBetween(v), math.pi, delta=.01)
        r = pytrix.rotation3DMatrix(math.pi, 0, 0)
        self.assertAlmostEqual((r * v).angleBetween(v), math.pi, delta=.01)

        matrices = [
            pytrix.Vector(2, 0, 0),
            pytrix.Vector(2, 2, 0),
            pytrix.Vector(2, 0, 2),
            pytrix.Vector(2, 2, 2),
            pytrix.Vector(0, 0, 0),
            pytrix.Vector(0, 2, 0),
            pytrix.Vector(0, 0, 2),
            pytrix.Vector(0, 2, 2),
            pytrix.Vector(2, 0, 0),
            pytrix.Vector(2, 2, 0),
            pytrix.Vector(2, 0, 2),
            pytrix.Vector(2, 2, 2),
            pytrix.Vector(0, 0, 0),
            pytrix.Vector(0, 2, 0),
            pytrix.Vector(0, 0, 2),
            pytrix.Vector(0, 2, 2)
        ]

        r = pytrix.rotation3DMatrix(math.pi, math.pi, math.pi)
        for m in matrices:
            m2 = r * m
            self.assertAlmostEqual(m[0], m2[0], delta=.01)
            self.assertAlmostEqual(m[1], m2[1], delta=.01)
            self.assertAlmostEqual(m[2], m2[2], delta=.01)

    def testPermutationMatrix(self):
        for d in range(2, 10):
            for p in range(d):
                for p2 in range(d):
                    iMatrix = pytrix.identityMatrix(d)
                    pMatrix = pytrix.permutationMatrix(d, p, p2)
                    self.assertEqual(iMatrix.permute(p, p2), pMatrix)

        self.assertRaises(TypeError, pytrix.permutationMatrix, 1)
        self.assertRaises(TypeError, pytrix.permutationMatrix, 1, 2)
        self.assertRaises(ValueError, pytrix.permutationMatrix, 1, 2, 3)
        self.assertRaises(ValueError, pytrix.permutationMatrix, 0, 2, 3)
        self.assertRaises(ValueError, pytrix.permutationMatrix, -1, 2, 3)
        self.assertRaises(TypeError, pytrix.permutationMatrix, 0, [], 3)
        self.assertRaises(TypeError, pytrix.permutationMatrix, 0, 2, "3")
