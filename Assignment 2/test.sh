#! /usr/bin/env bash
rm -f R.bsig R.data R.info R.psig R.tsig

echo "./create R 5000 4 1000"
./create R 5000 4 1000

echo "./stats R"
./stats R

echo "./gendata 5 4 | ./insert -v R"
./gendata 5 4 | ./insert -v R

echo "./stats R"
./stats R

echo "./dump R"
./dump R

./select R '1000001,?,?,?' t

./select R '1000001,?,a3-002,?' t

./select R '1000001,?,a3-002,?' 
