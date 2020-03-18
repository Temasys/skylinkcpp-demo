#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PROJECT_ROOT_DIR="$(dirname "$SCRIPT_DIR")"
THIRD_PARTY_DIR="$(dirname "$SCRIPT_DIR")"/third_party
BUILD_DIR="$(dirname "$SCRIPT_DIR")"/build

mkdir -p $THIRD_PARTY_DIR
mkdir -p $BUILD_DIR
