#!/bin/bash

additional_args="$@"
./bin/hatari --configfile ./.hatari/hatari.cfg --window --zoom 1.85 $additional_args 
