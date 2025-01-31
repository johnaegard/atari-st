#!/bin/bash

additional_args="$@"
nohup hatari --configfile ./.hatari/hatari.cfg $additional_args & disown
