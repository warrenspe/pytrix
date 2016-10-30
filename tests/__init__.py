#!/usr/bin/python

# Standard imports
import unittest, argparse, sys, os, time

sys.path.append('.')
sys.path.append('..')

# Project imports
import pytrix


# Globals
TEST_DIR = os.path.split(os.path.dirname(os.path.realpath(__file__)))[-1]

class PytrixTestCase(unittest.TestCase):

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
    parser = argparse.ArgumentParser(description="Execute pytrix Unit Tests")
    parser.add_argument("testFiles", nargs="*")
    parser.add_argument("--verbosity", nargs="?", choices=['1', '2'], default=1)
    args = parser.parse_args()

    if len(args.testFiles):
        runSelective(args.testFiles, int(args.verbosity))
    else:
        run(int(args.verbosity))

if __name__ == '__main__':
    main()

