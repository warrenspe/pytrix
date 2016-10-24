#!/usr/bin/python

# Standard imports
import unittest, argparse, sys, os, time
try:
    import cPickle as pickle
except ImportError:
    import pickle

sys.path.append('.')
sys.path.append('..')

# Projet imports
import kimchi

# Python 2 & 3 compatibility
if sys.version_info.major >= 3:
    if type(__builtins__) == dict:
        __builtins__['long'] = int
    else:
        __builtins__.long = int

# Globals
TEST_DIR = os.path.split(os.path.dirname(os.path.realpath(__file__)))[-1]
PICKLE_CMP = 0

class KimchiTestCase(unittest.TestCase):

    def __test(self, obj, ser, deser):
            serName = ser.__name__
            deserName = deser.__name__

            try:
                before = time.time()
                dumped = ser(obj)
                serElapsed = time.time() - before
            except Exception as e:
                self.fail("Error in (%s) serializing %s:\n%s" % (serName, repr(obj), str(e)))

            try:
                before = time.time()
                loaded = deser(dumped)
                deserElapsed = time.time() - before
            except Exception as e:
                self.fail("Error in (%s) deserializing %s (serialized: %r):\n%s"
                          % (deserName, repr(obj), repr(dumped), str(e)))
            if obj != loaded:
                self.fail("%s != %s; using %s, %s" % (repr(obj), repr(loaded), serName, deserName))

            return serElapsed, deserElapsed, len(dumped)


    def _test(self, toTest):
        for obj in toTest:
            kimSerElapsed, kimDeserElapsed, kimSerializedSize = self.__test(obj, kimchi.dumps, kimchi.loads)
            if PICKLE_CMP:
                pickleSerElapsed, pickleDeserElapsed, pickleSerializedSize = self.__test(obj, pickle.dumps, pickle.loads)
                print("\n".join(("",
                    "%s:" % repr(obj)[:125],
                    "Time to serialize:",
                    "    kimchi: %s" % kimSerElapsed,
                    "    pickle: %s" % pickleSerElapsed,
                    "Time to deserialize:",
                    "    kimchi: %s" % kimDeserElapsed,
                    "    pickle: %s" % pickleDeserElapsed,
                    "Serialization length:",
                    "    kimchi: %s" % kimSerializedSize,
                    "    pickle: %s" % pickleSerializedSize
                )))
                      


    @classmethod
    def setUpClass(cls):
        pass

    @classmethod
    def tearDownClass(cls):
        pass

def run(verbosity=1):
    loader = unittest.TestLoader()
    suite = loader.discover(TEST_DIR)
    _runSuite(suite, verbosity)

def runSelective(testFiles, verbosity):
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromNames(["%s.test_%s" % (TEST_DIR, testFile) for testFile in testFiles])
    _runSuite(suite, verbosity)

def _runSuite(testSuite, verbosity):
    unittest.TextTestRunner(verbosity=verbosity).run(testSuite)
    sys.stdout.write("\nPress enter to close.")
    sys.stdin.readline()

def main():
    global PICKLE_CMP

    parser = argparse.ArgumentParser(description="Execute kimchi Unit Tests")
    parser.add_argument("testFiles", nargs="*")
    parser.add_argument("--verbosity", nargs="?", choices=['1', '2'], default=1)
    args = parser.parse_args()

    if len(args.testFiles):
        runSelective(args.testFiles, int(args.verbosity))
    else:
        run(int(args.verbosity))

if __name__ == '__main__':
    main()

