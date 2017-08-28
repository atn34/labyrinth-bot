#!/usr/bin/env python3
"""
`$ ./build.py` generates a Makefile for building the executable and tests.
"""
from itertools import chain
import os
import re
import subprocess
import sys

INCLUDE_RE = re.compile(r'^#include\s+"([^"]*)')

class memoize:
    def __init__(self, fn):
        self.fn = fn
        self.memo = {}
    def __call__(self, *args):
        if args not in self.memo:
            self.memo[args] = self.fn(*args)
        return self.memo[args]

@memoize
def includes_from_file(file_name):
    result = []
    with open(file_name, 'r') as fil:
        for line in fil:
            if line.startswith('#') or line.startswith('//') or line == '\n':
                match = INCLUDE_RE.match(line)
                if match:
                    include = match.groups()[0]
                    if os.path.isfile(include):
                        result.append(include)
            else:
                break
    return result

class TransitiveIncludes(object):
    """
    Computes the transitive includes of the given file.

    Only reports files that are accessible from the current directory.
    """

    def __init__(self, cc_file, includes_from_cc=False):
        self.files_processed = set()
        self.includes_from_cc = includes_from_cc
        self.cc_file = cc_file

    def find(self):
        """
        Yield the transitive includes.
        """
        yield from self._read_includes(self.cc_file)

    def _read_includes(self, file_name):
        if self.includes_from_cc:
            if file_name not in self.files_processed:
                self.files_processed.add(file_name)
            else:
                return
        for include in includes_from_file(file_name):
            if include not in self.files_processed:
                self.files_processed.add(include)
                yield include
                yield from self._read_includes(include)
                if self.includes_from_cc:
                    basename = os.path.splitext(include)[0]
                    if os.path.isfile(basename + '.cc'):
                        yield from self._read_includes(basename + '.cc')


def object_deps(file_name):
    """
    Reports all the object deps in the local source tree needed to compile the
    executable corresponding to the given cc file.
    """
    assert file_name.endswith('.cc')
    for include in TransitiveIncludes(file_name, includes_from_cc=True).find():
        basename = os.path.splitext(include)[0]
        if os.path.isfile(basename + '.cc'):
            yield basename + '.o'

def header_deps(file_name):
    assert file_name.endswith('.cc')
    yield from TransitiveIncludes(file_name).find()

def main():
    cc_files = set()
    test_cc_files = set()
    main_cc_files = set()
    for f in subprocess.check_output(
            r'find . -type f -name "*.cc" | sort', shell=True).decode('utf-8').splitlines():
        if f.endswith('_test.cc'):
            test_cc_files.add(f)
        elif f.endswith('_main.cc'):
            main_cc_files.add(f)
        elif f.endswith('.cc'):
            cc_files.add(f)
        else:
            assert False

     # To make diffing build.py outputs easier
    cc_files = sorted(cc_files)
    test_cc_files = sorted(test_cc_files)
    main_cc_files = sorted(main_cc_files)

    cflags = subprocess.check_output(
        'pkg-config --cflags opencv', shell=True).decode(
            'utf-8')[:-1] + ' ' + ' '.join(['-std=c++11', '-Wall', '-Werror', '-g'])
    lflags = subprocess.check_output('pkg-config --libs opencv', shell=True).decode('utf-8')[:-1] + ' -Wl,-rpath /usr/lib -L/usr/lib -lsocket++'

    for cc_file in cc_files:
        sys.stdout.write(
            os.path.splitext(cc_file)[0]+'.o: ' + cc_file + ' ' + ' '.join(
                header_deps(cc_file)) + '\n')
        sys.stdout.write('\tg++ -c %s $(CFLAGS) %s -o $@\n' % (cc_file, cflags))

    for test_cc_file in test_cc_files:
        o_deps = list(object_deps(test_cc_file))
        sys.stdout.write(test_cc_file + '.exe: ' + test_cc_file + ' ' + ' '.join(chain(o_deps, header_deps(test_cc_file))) + '\n')
        sys.stdout.write('	g++ ' + test_cc_file + (' $(CFLAGS) %s ' % cflags) + ' '.join(o_deps) +
                         ' %s -lgtest -lgmock -lpthread -lgtest_main -o $@\n' % lflags)

    for main_cc_file in main_cc_files:
        o_deps = list(object_deps(main_cc_file))
        sys.stdout.write(main_cc_file + '.exe: ' + main_cc_file + ' ' + ' '.join(chain(o_deps, header_deps(main_cc_file))) + '\n')
        sys.stdout.write('	g++ ' + main_cc_file + (' $(CFLAGS) %s ' % cflags) + ' '.join(o_deps) +
                         ' %s -o $@\n' % lflags)

    sys.stdout.write('.PHONY: test\n')
    sys.stdout.write('test: ' + ' '.join(
        test_cc_file + '.exe' for test_cc_file in test_cc_files) + '\n')
    for test_cc_file in test_cc_files:
        sys.stdout.write('\t./' + test_cc_file + '.exe\n')

    sys.stdout.write('.PHONY: build\n')
    sys.stdout.write('build: ' + ' '.join(
        main_cc_file + '.exe' for main_cc_file in chain(
            main_cc_files, test_cc_files)) + '\n')

if __name__ == '__main__':
    main()
