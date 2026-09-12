#!/bin/bash

additional_args="$@"
hatari --configfile ./.hatari/hatari.cfg --window --zoom 1.85 $additional_args 
