#!/bin/bash

if [ ! "$BASH_VERSION" ] ; then
  echo "Please do not use sh to run this script ($0), use bash." 1>&2
  exit 1
fi
source $(dirname $0)/common.sh

echo "Cloning SDL2 into $THIRD_PARTY_DIR"

cd $THIRD_PARTY_DIR

if [[ $OSTYPE =~ ^[Dd][Aa][Rr][Ww][Ii][Nn] ]] ; then
  # Mac OSX
  # replace HG with wget ?
  hg clone https://hg.libsdl.org/SDL SDL

  # I don't know whether those flags are actually useful
  if [[ "$OSTYPE" == "darwin"* ]]; then
    SDKDIR="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk"
    export CFLAGS="-isysroot ${SDKDIR} -I${SDKDIR}/usr/include -mmacosx-version-min=10.9.0"
    export LDFLAGS="-isysroot ${SDKDIR}"
  fi

  mkdir SDL/build
  cd SDL/build
  ../configure --enable-static=yes --prefix="$THIRD_PARTY_DIR/BUILD/SDL"
  make && make install

else
  # Linux
  sudo apt-get install libsdl2-dev
fi


