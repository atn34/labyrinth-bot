#!/usr/bin/env python3
import glob
import itertools
import os
import subprocess
import sys

cc_files = [cc_file for cc_file in glob.iglob('**/*.cc', recursive=True) if not cc_file.endswith('_test.cc')]
test_cc_files = [cc_file for cc_file in glob.iglob('**/*.cc', recursive=True) if cc_file.endswith('_test.cc')]

for cc_file in itertools.chain(cc_files, test_cc_files):
    sys.stdout.write(subprocess.check_output(
        'g++ -M -std=c++11 %s `pkg-config --cflags opencv`' % (
            cc_file), shell=True).decode('utf-8'))
    sys.stdout.write('	g++ -std=c++11 -c %s `pkg-config --cflags opencv` -o $@\n' % (cc_file))

object_files = [os.path.splitext(cc_file)[0]+'.o' for cc_file in cc_files]
test_object_files = [os.path.splitext(test_cc_file)[0]+'.o' for test_cc_file in test_cc_files]

sys.stdout.write('a.out: ' + ' '.join(object_files) + '\n')
sys.stdout.write('	g++ -std=c++11 ' +
                 ' '.join(object_files) +
                 ' `pkg-config --cflags --libs opencv` -o $@\n')

sys.stdout.write('test.out: ' + ' '.join(test_object_files) + '\n')
sys.stdout.write('	g++ -std=c++11 ' +
                 ' '.join(test_object_files) +
                 ' `pkg-config --cflags --libs opencv` -lgtest -lpthread -lgtest_main -o $@\n')
