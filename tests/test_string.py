# Project imports
import kimchi
import tests

class TestString(tests.KimchiTestCase):

    def testString(self):
        toTest = [
            "",
            "a",
            "\u2208",
            "alsdkjfl;qwej roiwe rqj r",
            "\x00",
            "\x00\x01\x00",
        ]

        self._test(toTest)
