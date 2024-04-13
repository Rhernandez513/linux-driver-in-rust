#!/bin/sh

wget https://download.qemu.org/qemu-8.2.1.tar.xz -O /tmp/qemu-8.2.1.tar.xz
tar xf /tmp/qemu-8.2.1.tar.xz -C /tmp/

diff -urN --no-dereference -x prefix -x scripts -x tests -x build -x GNUmakefile /tmp/qemu-8.2.1 /users/rherna70/code/qemu-8.2.1 > qemu-8.2.1.patch

