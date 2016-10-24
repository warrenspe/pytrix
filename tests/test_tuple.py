# Project imports
import kimchi
import tests

class TestTuple(tests.KimchiTestCase):

    def testTuple(self):
        toTest = [
            tuple(),
            ('',),
            (0,),
            (1,),
            (u'\u2208',),
            (1, 2, 3),
            (long(-1),),
            tuple(range(900000)),
            ((1, 2), (4, 5), ((6, 7), (8,), tuple())),
            ((((((tuple(),),),),),),),
            ([], [], [], None, [tuple()])
        ]

        self._test(toTest)
