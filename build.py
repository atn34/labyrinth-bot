#!/usr/bin/env python3
"""
`$ ./build.py` generates a Makefile for building the executable and tests.
"""
import os
import re
import subprocess
import sys

INCLUDE_RE = re.compile(r'^#include\s+"([^"]*)')

class TransitiveIncludes(object):
    """
    Computes the transitive includes of the given file.

    Only reports files that are accessible from the current directory.
    """

    def __init__(self, cc_file):
        self.includes = set()
        self.cc_file = cc_file

    def find(self):
        """
        Yield the transitive includes.
        """
        yield from self._read_includes(self.cc_file)

    def _read_includes(self, file_name):
        with open(file_name, 'r') as fil:
            for line in fil:
                if line.startswith('#include') or line.startswith('//') or line == '\n':
                    match = INCLUDE_RE.match(line)
                    if match:
                        include = match.groups()[0]
                        if include not in self.includes:
                            self.includes.add(include)
                            if os.path.isfile(include):
                                yield include
                                yield from self._read_includes(include)
                else:
                    break

def object_deps(file_name):
    """
    Reports all the object deps in the local source tree needed to compile the
    executable corresponding to the given cc file.
    """
    assert file_name.endswith('.cc')
    for include in TransitiveIncludes(file_name).find():
        basename = os.path.splitext(include)[0]
        if os.path.isfile(basename + '.cc'):
            yield basename + '.o'

def main():
    cc_files = set()
    test_cc_files = set()
    for f in subprocess.check_output(
            r'find . -type f -name "*.cc"', shell=True).decode('utf-8').splitlines():
        if f.endswith('_test.cc'):
            test_cc_files.add(f)
        elif f.endswith('.cc'):
            cc_files.add(f)
        else:
            assert False

    cflags = subprocess.check_output(
        'pkg-config --cflags opencv', shell=True).decode(
            'utf-8')[:-1] + ' ' + ' '.join(['-std=c++11', '-Wall', '-Werror'])
    lflags = subprocess.check_output('pkg-config --libs opencv', shell=True).decode('utf-8')[:-1]

    for cc_file in cc_files:
        sys.stdout.write(
            os.path.splitext(cc_file)[0]+'.o: ' + cc_file + ' ' + ' '.join(
                TransitiveIncludes(cc_file).find()) + '\n')
        sys.stdout.write('\tg++ -c %s %s -o $@\n' % (cc_file, cflags))

    object_files = [os.path.splitext(cc_file)[0]+'.o' for cc_file in cc_files]

    sys.stdout.write('a.out: ' + ' '.join(object_files) + '\n')
    sys.stdout.write('\tg++ ' +
                     ' '.join(object_files) +
                     ' %s -o $@\n' % lflags)

    for test_cc_file in test_cc_files:
        deps = list(object_deps(test_cc_file))
        sys.stdout.write(test_cc_file + '.exe: ' + test_cc_file + ' ' + ' '.join(deps) + '\n')
        sys.stdout.write('	g++ ' + test_cc_file + ' '
                         ' '.join(deps) +
                         ' %s -lgtest -lpthread -lgtest_main -o $@\n' % lflags)

    sys.stdout.write('.PHONY: test\n')
    sys.stdout.write('test: ' + ' '.join(
        test_cc_file + '.exe' for test_cc_file in test_cc_files) + '\n')
    for test_cc_file in test_cc_files:
        sys.stdout.write('\t./' + test_cc_file + '.exe\n')

if __name__ == '__main__':
    main()
