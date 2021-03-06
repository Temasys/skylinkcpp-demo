#!/bin/bash

if [ ! "$BASH_VERSION" ] ; then
  echo "Please do not use sh to run this script ($0), use bash." 1>&2
  exit 1
fi
source $(dirname $0)/common.sh

# Download SkylinkCPP latest
SKYLINKCPP_VERSION="2.0.2"
SKYLINKCPP_URL="https://temasys-cdn.s3.amazonaws.com/skylink/skylinksdk/cpp/${SKYLINKCPP_VERSION}/skylinkcpp-${SKYLINKCPP_VERSION}.tar.gz"
SKYLINKCPP_ARCHIVE=$(basename $SKYLINKCPP_URL)
cd $PROJECT_ROOT_DIR
mkdir skylinkcpp
cd skylinkcpp
wget $SKYLINKCPP_URL
tar xfz $SKYLINKCPP_ARCHIVE
rm $SKYLINKCPP_ARCHIVE
