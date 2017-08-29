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

OPENCV_LIBS = set('''-lopencv_dnn -lopencv_ml -lopencv_objdetect -lopencv_shape
-lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d
-lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs
-lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core'''.split())

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
    if not os.path.isfile(file_name):
        return result
    with open(file_name, 'r') as fil:
        for line in fil:
            if line.startswith('#') or line.startswith('//') or line == '\n':
                match = INCLUDE_RE.match(line)
                if match:
                    include = match.groups()[0]
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
        if self.includes_from_cc and file_name.endswith('.cc'):
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
    Reports all the object deps needed to compile the
    executable corresponding to the given cc file.
    """
    assert file_name.endswith('.cc')
    for include in TransitiveIncludes(file_name, includes_from_cc=True).find():
        basename = os.path.splitext(include)[0]
        if os.path.isfile(basename + '.cc'):
            yield basename + '.o'

def header_deps(file_name, includes_from_cc=False):
    assert file_name.endswith('.cc')
    yield from TransitiveIncludes(file_name, includes_from_cc=includes_from_cc).find()

def headers_to_cflags(headers):
    @memoize
    def header_to_cflags(header_file):
        if header_file.startswith('opencv2'):
            return set(['-Iopencv-3.3.0-prebuilt/include'])
        return set()
    cflags = set()
    for h_dep in headers:
        cflags |= header_to_cflags(h_dep)
    return ' '.join(sorted(cflags) + ['-std=c++14', '-Wall', '-Werror', '-g'])

def headers_to_lflags(headers):
    @memoize
    def header_to_lflags(header_file):
        special_cases = {
            'libsocket/inetclientstream.hpp': {'-Wl,-rpath /usr/lib', '-L/usr/lib', '-lsocket++'},
        }
        if header_file.startswith('opencv2'):
            return {'-Wl,-rpath opencv-3.3.0-prebuilt/lib', '-Lopencv-3.3.0-prebuilt/lib'} | OPENCV_LIBS
        return special_cases.get(header_file, set())
    lflags = set()
    for h_dep in headers:
        lflags |= header_to_lflags(h_dep)
    return ' '.join(sorted(lflags))

def object_file_rule(cc_file):
    assert cc_file.endswith('.cc')
    h_deps = list(header_deps(cc_file))
    cflags = headers_to_cflags(h_deps)
    depends = os.path.splitext(cc_file)[0]+'.o: ' + cc_file + ' ' + ' '.join(filter(os.path.isfile, h_deps))
    build_command = '\tg++ -c %s $(CFLAGS) %s -o $@\n' % (cc_file, cflags)
    return '\n'.join((depends, build_command))

def test_file_rule(test_cc_file):
    assert test_cc_file.endswith('_test.cc')
    o_deps = list(object_deps(test_cc_file))
    h_deps = list(header_deps(test_cc_file))
    cflags = headers_to_cflags(h_deps)
    lflags = headers_to_lflags(header_deps(test_cc_file, includes_from_cc=True))
    depends = test_cc_file + '.exe: ' + test_cc_file + ' ' + ' '.join(chain(o_deps, filter(os.path.isfile, h_deps)))
    build_command = ('\tg++ ' + test_cc_file + (' $(CFLAGS) %s ' % cflags) + ' '.join(o_deps) +
                      ' %s -lgmock -lpthread -lgmock_main -o $@\n' % lflags)
    return '\n'.join((depends, build_command))

def main_file_rule(main_cc_file):
    assert main_cc_file.endswith('_main.cc')
    o_deps = list(object_deps(main_cc_file))
    h_deps = list(header_deps(main_cc_file))
    cflags = headers_to_cflags(h_deps)
    lflags = headers_to_lflags(header_deps(main_cc_file, includes_from_cc=True))
    depends = main_cc_file + '.exe: ' + main_cc_file + ' ' + ' '.join(chain(o_deps, filter(os.path.isfile, h_deps)))
    build_command = ('\tg++ ' + main_cc_file + (' $(CFLAGS) %s ' % cflags) + ' '.join(o_deps) +
                      ' %s -o $@\n' % lflags)
    return '\n'.join((depends, build_command))

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

    for cc_file in cc_files:
        print(object_file_rule(cc_file))

    for test_cc_file in test_cc_files:
        print(test_file_rule(test_cc_file))

    for main_cc_file in main_cc_files:
        print(main_file_rule(main_cc_file))

    print('.PHONY: test')
    print('test: ' + ' '.join(
        test_cc_file + '.exe' for test_cc_file in test_cc_files))
    for test_cc_file in test_cc_files:
        print('\t' + test_cc_file + '.exe')

    print('.PHONY: build')
    print('build: ' + ' '.join(
        main_cc_file + '.exe' for main_cc_file in chain(
            main_cc_files, test_cc_files)))

if __name__ == '__main__':
    main()
