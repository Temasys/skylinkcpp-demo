#!/bin/bash

if [ ! "$BASH_VERSION" ] ; then
  echo "Please do not use sh to run this script ($0), use bash." 1>&2
  exit 1
fi
source $(dirname $0)/common.sh

echo "Cloning Boost into $THIRD_PARTY_DIR"

cd $THIRD_PARTY_DIR
wget https://dl.bintray.com/boostorg/release/1.65.0/source/boost_1_65_0.tar.gz
tar xfz boost_1_65_0.tar.gz
cd boost_1_65_0/
./bootstrap.sh
./bjam install --prefix=./build --with-system --with-date_time --with-random link=static runtime-link=shared threading=multi

