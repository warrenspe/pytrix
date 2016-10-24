# Project imports
import kimchi
import tests

class TestSet(tests.KimchiTestCase):

    def testSet(self):
        toTest = [
            set(),
            set(['']),
            set([0]),
            set([1]),
            set([u'\u2208']),
            set([1, 2, 3]),
            set([long(-1)]),
            set(range(900000)),
            set([(1, 2), (4, 5), ((6, 7), (8,), tuple())]),
            set((((((((((((((None,),),),),),),),),),),),),))
        ]

        self._test(toTest)


    def testFrozenSet(self):
        toTest = [
            frozenset(),
            frozenset(['']),
            frozenset([0]),
            frozenset([1]),
            frozenset([u'\u2208']),
            frozenset([1, 2, 3]),
            frozenset([long(-1)]),
            frozenset(range(900000)),
            frozenset([(1, 2), (4, 5), ((6, 7), (8,), tuple())]),
            frozenset((((((((((((((None,),),),),),),),),),),),),))
        ]

        self._test(toTest)
