# Standard imports
import gc

# Project imports
import pytrix
import tests

class TestMatrixIter(tests.PytrixTestCase):

    def testRefCounts(self):
        m = pytrix.Matrix([1], [2], [3], [4], [5], [6], [7], [8], [9], [0])

        self.assertEqual(len(gc.get_referrers(m)), 1)

        # Test that the ref count of a matrix is incremented when we create an iterator to it
        i = iter(m)
        self.assertEqual(len(gc.get_referrers(i)), 1)
        self.assertEqual(len(gc.get_referrers(m)), 2)

        # Test that when the iterator is dealloc'd the matrices' refcount is decremented
        del i
        self.assertEqual(len(gc.get_referrers(m)), 1)

        # Test that when the iterator finishes iteration the matrices' refcount is decremented
        i = iter(m)
        self.assertEqual(len(gc.get_referrers(m)), 2)
        try:
            while True:
                next(i)
        except StopIteration:
            self.assertEqual(len(gc.get_referrers(m)), 1)

        # Test that when the iterator is dealloc'd after finishing iteration the refcount of the matrix is unchanged
        del i
        self.assertEqual(len(gc.get_referrers(m)), 1)

        # Test that when the matrix is deleted, iteration can still continue until completion
        i = iter(m)
        del m
        self.assertEqual([list(q) for q in i], [[1], [2], [3], [4], [5], [6], [7], [8], [9], [0]])
        self.assertRaises(StopIteration, next, i)

    def testIteration(self):
        m = pytrix.Matrix([1, 2, 3, 4, 5])
        self.assertEqual(list(iter(m)), [pytrix.Vector(1, 2, 3, 4, 5)])
        self.assertEqual(list(m), [pytrix.Vector(1, 2, 3, 4, 5)])
        self.assertEqual(tuple(iter(m)), (pytrix.Vector(1, 2, 3, 4, 5),))
        self.assertEqual(tuple(m), (pytrix.Vector(1, 2, 3, 4, 5),))
        i = iter(m)
        curr = 1
        try:
            while True:
                next(i)
                curr += 1
        except StopIteration:
            self.assertEqual(curr, 2)

        self.assertEqual(list(iter(iter(m))), [pytrix.Vector(1, 2, 3, 4, 5)])
        self.assertEqual(tuple(iter(iter(iter(m)))), (pytrix.Vector(1, 2, 3, 4, 5),))
        i = iter(iter(iter(iter(m))))
        curr = 1
        try:
            while True:
                next(i)
                curr += 1
        except StopIteration:
            self.assertEqual(curr, 2)

        self.assertEqual(len(gc.get_referrers(m)), 1)
