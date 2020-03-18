#!/bin/bash

if [ ! "$BASH_VERSION" ] ; then
  echo "Please do not use sh to run this script ($0), use bash." 1>&2
  exit 1
fi
source $(dirname $0)/common.sh

echo "Cloning portaudio into $THIRD_PARTY_DIR"

cd $THIRD_PARTY_DIR

curl -O http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz
tar xfz pa_stable_v190600_20161030.tgz
cd portaudio
THIRD_BUILD_DIR="$THIRD_PARTY_DIR/BUILD"
./configure --prefix="$THIRD_BUILD_DIR/portaudio"
make && make install

