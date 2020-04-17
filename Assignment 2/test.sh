#! /usr/bin/env bash
rm -f R.bsig R.data R.info R.psig R.tsig

echo "./create R 5000 4 1000"
./create R 5000 4 1000

echo "./stats R"
./stats R

echo "./gendata 5 4 | ./insert -v R"
./gendata 5 4 | ./insert -v R
