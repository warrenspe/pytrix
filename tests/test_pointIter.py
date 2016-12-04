# Standard imports
import gc

# Project imports
import pytrix
import tests

class TestPointIter(tests.PytrixTestCase):

    def testRefCounts(self):
        v = pytrix.Point([1, 2, 3, 4, 5, 6, 7, 8, 9, 0])

        self.assertEqual(len(gc.get_referrers(v)), 1)

        # Test that the ref count of a point is incremented when we create an iterator to it
        i = iter(v)
        self.assertEqual(len(gc.get_referrers(i)), 1)
        self.assertEqual(len(gc.get_referrers(v)), 2)

        # Test that when the iterator is dealloc'd the points refcount is decremented
        del i
        self.assertEqual(len(gc.get_referrers(v)), 1)

        # Test that when the iterator finishes iteration the points refcount is decremented
        i = iter(v)
        self.assertEqual(len(gc.get_referrers(v)), 2)
        try:
            while True:
                next(i)
        except StopIteration:
            self.assertEqual(len(gc.get_referrers(v)), 1)

        # Test that when the iterator is dealloc'd after finishing iteration the refcount of the point is unchanged
        del i
        self.assertEqual(len(gc.get_referrers(v)), 1)

        # Test that when the point is deleted, iteration can still continue until completion
        i = iter(v)
        del v
        self.assertEqual(list(i), [1, 2, 3, 4, 5, 6, 7, 8, 9, 0])
        self.assertRaises(StopIteration, next, i)

    def testIteration(self):
        v = pytrix.Point([1, 2, 3, 4, 5])
        self.assertEqual(list(iter(v)), [1, 2, 3, 4, 5])
        self.assertEqual(list(v), [1, 2, 3, 4, 5])
        self.assertEqual(tuple(iter(v)), (1, 2, 3, 4, 5))
        self.assertEqual(tuple(v), (1, 2, 3, 4, 5))
        i = iter(v)
        curr = 1
        try:
            while True:
                self.assertEqual(curr, next(i))
                curr += 1
        except StopIteration:
            self.assertEqual(curr, 6)

        self.assertEqual(list(iter(iter(v))), [1, 2, 3, 4, 5])
        self.assertEqual(tuple(iter(iter(iter(v)))), (1, 2, 3, 4, 5))
        i = iter(iter(iter(iter(v))))
        curr = 1
        try:
            while True:
                self.assertEqual(curr, next(i))
                curr += 1
        except StopIteration:
            self.assertEqual(curr, 6)

        self.assertEqual(len(gc.get_referrers(v)), 1)
