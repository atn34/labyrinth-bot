#!/bin/bash

nc -k -l 10000 | stdbuf -i0 -o0 -e0 python bigapple.py
