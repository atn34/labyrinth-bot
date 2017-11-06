#!/usr/bin/env python3
"""
`$ ./gen_makefile.py` generates a Makefile for building executables and tests.
"""
from itertools import chain
import os
import re
import subprocess
import sys

INCLUDE_RE = re.compile(r'^#include\s+"([^"]*)')

def is_cc(file_name):
    return file_name.endswith('.cc') or file_name.endswith('.cpp')

def strip_extension(file_name):
    return os.path.splitext(file_name)[0]

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
    for line in subprocess.check_output(
            ['cpp', '-fpreprocessed', file_name]).decode('utf-8').splitlines():
        if line.startswith('#') or line == '':
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
        if self.includes_from_cc and is_cc(file_name):
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
                    basename = strip_extension(include)
                    if os.path.isfile(basename + '.cc'):
                        yield from self._read_includes(basename + '.cc')
                    elif os.path.isfile(basename + '.cpp'):
                        yield from self._read_includes(basename + '.cpp')

OPENCV_LIBS = '''-lopencv_dnn -lopencv_ml -lopencv_objdetect -lopencv_shape
-lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d
-lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs
-lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core'''.split()

def object_deps(file_name):
    """
    Reports all the object deps needed to compile the
    executable corresponding to the given cc file.
    """
    assert is_cc(file_name)
    for include in TransitiveIncludes(file_name, includes_from_cc=True).find():
        basename = strip_extension(include)
        if os.path.isfile(basename + '.cc'):
            yield basename + '.o'
        elif os.path.isfile(basename + '.cpp'):
            yield basename + '.o'

def linker_args(file_name):
    """
    Reports all the linker args needed to compile the
    executable corresponding to the given cc file.
    """
    for include in TransitiveIncludes(file_name, includes_from_cc=True).find():
        if 'gtest' in include:
            yield from ['-lgmock', '-lgmock_main', '-lpthread']
        elif include.startswith('opencv2'):
            yield from ['-Wl,-rpath opencv-3.3.0-prebuilt/lib', '-Lopencv-3.3.0-prebuilt/lib'] 
            yield from OPENCV_LIBS
        elif include == 'libsocket/inetclientstream.hpp':
            yield from ['-Wl,-rpath /usr/lib', '-L/usr/lib', '-lsocket++']

def header_deps(file_name):
    assert is_cc(file_name)
    yield from TransitiveIncludes(file_name, includes_from_cc=False).find()

def cc_args(h_deps):
    for include in h_deps:
        if include.startswith('opencv2'):
            yield '-Iopencv-3.3.0-prebuilt/include'

def object_file_rule(cc_file):
    assert is_cc(cc_file)
    h_deps = sorted(set(header_deps(cc_file)))
    o_args = sorted(set(cc_args(h_deps)))
    depends = os.path.splitext(cc_file)[0]+'.o: ' + cc_file + ' ' + ' '.join(filter(os.path.isfile, h_deps))
    build_command = '\t$(CXX) -c ' + cc_file + ' $(CXXFLAGS) ' + ' '.join(o_args) + ' -o $@\n'
    return '\n'.join((depends, build_command))

def binary_file_rule(main_cc_file):
    assert main_cc_file.endswith('_main.cc') or main_cc_file.endswith('_test.cc')
    o_deps = sorted(set(object_deps(main_cc_file)))
    o_args = sorted(set(linker_args(main_cc_file)))
    main_o_file = strip_extension(main_cc_file) + '.o'
    depends = strip_extension(main_cc_file) + ': ' + main_o_file + ' ' + ' '.join(o_deps)
    build_command = ('\t$(CXX) $(LDFLAGS) ' + main_o_file + ' ' + ' '.join(chain(o_deps, o_args)) + ' -o $@\n')
    return '\n'.join((depends, build_command))

def main():
    test_cc_files = set()
    main_cc_files = set()
    cc_files = set()
    for f in subprocess.check_output(
            r'find . -regextype posix-egrep -regex ".*\.(cc|cpp)$" | sed "s,^\./,," | sort',
            shell=True).decode('utf-8').splitlines():
        if is_cc(f):
            cc_files.add(f)
        if f.endswith('_test.cc'):
            test_cc_files.add(f)
        elif f.endswith('_main.cc'):
            main_cc_files.add(f)

    # To make diffing build.py outputs easier
    cc_files = sorted(cc_files)
    test_cc_files = sorted(test_cc_files)
    main_cc_files = sorted(main_cc_files)

    print('CXXFLAGS := -std=c++11 -Wall -Werror')
    print()

    print('.PHONY: all')
    print('all: ' + ' '.join(
        strip_extension(main_cc_file) for main_cc_file in chain(
            main_cc_files, test_cc_files)))
    print()

    for cc_file in cc_files:
        print(object_file_rule(cc_file))

    for main_cc_file in chain(main_cc_files, test_cc_files):
        print(binary_file_rule(main_cc_file))

    print('.PHONY: test')
    print('test: ' + ' '.join(
        strip_extension(test_cc_file) for test_cc_file in test_cc_files))
    for test_cc_file in test_cc_files:
        print('\t./' + strip_extension(test_cc_file))
    print()

    print('.PHONY: clean')
    print('clean:')
    print("\tfind . -type f -name '*.o' -delete")
    print('\trm -f ' + ' '.join(map(strip_extension, chain(main_cc_files, test_cc_files))))

if __name__ == '__main__':
    main()
