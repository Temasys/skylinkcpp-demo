#!/bin/bash

source $(dirname $0)/common.sh

bash $SCRIPT_DIR/cloneBoost.sh
bash $SCRIPT_DIR/clonePortAudio.sh
bash $SCRIPT_DIR/cloneSDL.sh
bash $SCRIPT_DIR/cloneSkylinkcpp.sh
