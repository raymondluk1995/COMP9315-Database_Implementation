make clean
make -B all
./sanity_check.sh > my.out
diff -s R.out my.out