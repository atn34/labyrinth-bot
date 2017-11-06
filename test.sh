#!/bin/bash

set -euxo pipefail

diff -u Makefile <(./gen_makefile.py)

make -j4 test
