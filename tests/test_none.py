# Project imports
import kimchi
import tests

class TestNone(tests.KimchiTestCase):

    def testNone(self):
        toTest = [
            None
        ]

        self._test(toTest)
