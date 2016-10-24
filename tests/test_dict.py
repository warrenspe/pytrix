# Project imports
import kimchi
import tests

class TestDict(tests.KimchiTestCase):

    def testDict(self):
        toTest = [
            dict(),
            {1: 2},
            dict([(a, a + 1) for a in range(15000)]),
            {3: {2: {1: {0: None}}}}
        ]

        self._test(toTest)
