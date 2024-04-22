#!/bin/sh -x

# For use inside the rust-enabled kernel running the rust driver

mknod /dev/lkp_enc c 250 0
ls -l /dev/lkp_enc

apt install build-essential zsh
