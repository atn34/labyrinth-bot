#!/bin/bash

set -euxo pipefail

diff -u Makefile <(./gen_makefile.py)

make test
