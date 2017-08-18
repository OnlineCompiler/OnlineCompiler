#!/bin/bash
rm -rf log bin; mkdir log bin
cd src; make clean; make release
