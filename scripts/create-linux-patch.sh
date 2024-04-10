#!/bin/sh

wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.72.tar.xz -O /tmp/linux-6.1.72.tar.xz
tar xf /tmp/linux-6.1.72.tar.xz -C /tmp/

diff -urN -x tools -x security  /tmp/linux-6.1.72 /users/rherna70/code/linux-6.1.72 > linux-6.1.72.patch

