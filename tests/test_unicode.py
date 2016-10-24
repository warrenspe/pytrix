# Project imports
import kimchi
import tests

class TestUnicode(tests.KimchiTestCase):

    def testUnicode(self):
        toTest = [
            u'',
            u'\u2208',
            u'adsjgfasjrqkwerkqjwekrhqwerjkw',
            u'\x00',
            u'\xFF',
            u'\u2208\u9208\u5208\u0208\ua208\u6208\u5208\u4408',
        ]

        self._test(toTest)
