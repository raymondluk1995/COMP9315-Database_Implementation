#! /usr/bin/env bash
rm -f R.bsig R.data R.info R.psig R.tsig


echo "./create R 10000 6 1000"
./create R 10000 6 1000

echo "./gendata 10000 6 1234567 13 > R.in"
./gendata 10000 6 1234567 13 > R.in

echo "./insert R < R.in"
./insert R < R.in

echo "./stats R"
./stats R

echo "./select R '1234999,?,?,?,?,?'"
./select R '1234999,?,?,?,?,?'

echo "./select R '1234999,?,?,?,?,?' t"
./select R '1234999,?,?,?,?,?' t

echo "./select R '1234999,?,?,?,?,?' p"
./select R '1234999,?,?,?,?,?' p

echo "./select R '1234999,?,?,?,?,?' b"
./select R '1234999,?,?,?,?,?' b
