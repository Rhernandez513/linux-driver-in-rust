#!/bin/sh

mknod /dev/lkp_enc_driver c 250 0
ls -l /dev/lkp_enc_driver

apt install build-essential zsh
