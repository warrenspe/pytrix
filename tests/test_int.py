# Project imports
import kimchi
import tests

class TestInteger(tests.KimchiTestCase):

    def testInteger(self):
        toTest = [
            0,
            1,
            -1,
            1234567,
            -1234567,
            int(2**63 - 1)
            -int(2**63 - 1)
        ]

        self._test(toTest)
