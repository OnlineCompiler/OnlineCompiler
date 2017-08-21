#!/bin/bash
rm -rf log/ bin/ tmp/ wwwroot/cgi-bin; mkdir log bin tmp wwwroot/cgi-bin
cd src; make clean; make release;cd -
cd cgi; make clean; make; mv compiler ../wwwroot/cgi-bin
