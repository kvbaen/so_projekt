#!/bin/bash

mkdir -p ~/demon_test/dir1/dir/next/other
mkdir -p ~/demon_test/dir2
mkdir -p ~/demon_test/dir1/dir/next/again
mkdir -p ~/demon_test/dir1/dir/next/otherone
mkdir -p ~/demon_test/dir1/dir2/cat13
mkdir -p ~/demon_test/dir1/dir2/cat15
mkdir -p ~/demon_test/dir1/dir3/directory/directory


echo "text" > ~/demon_test/dir1/dir/file.txt
echo "welcome" > ~/demon_test/dir1/dir/file2.txt
echo "abcd" > ~/demon_test/dir1/dir/next/file.txt
echo "fdsa" > ~/demon_test/dir1/dir/next/again/file123.txt
echo "everything okey" > ~/demon_test/dir1/dir/next/otherone/iuy.txt
echo "best project" > ~/demon_test/dir1/dir/next/otherone/utyas.txt
echo "nowy" > ~/demon_test/dir1/dir2/file544.txt
echo "24/7" > ~/demon_test/dir1/dir2/cat13/intrax.txt
echo "helloworld" > ~/demon_test/dir1/dir2/cat15/filenew.txt
echo "studies" > ~/demon_test/dir1/dir3/file.txt
echo "demo" > ~/demon_test/dir1/dir3/directory/trial.txt

for i in {1..3000}; do echo "this is big one" >>  ~/demon_test/dir1/dir3/directory/directory/big_one.txt; done 

echo "The directory structure has been successfully created!"
echo "The largest file created is in size 48kB!"
