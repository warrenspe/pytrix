# Project imports
import kimchi
import tests

class TestBool(tests.KimchiTestCase):

    def testBool(self):
        toTest = [
            False,
            True
        ]

        self._test(toTest)
