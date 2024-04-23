#!/bin/sh -x

##########################
# run-bookworm.sh #
##########################
#
# this script is for launching the bookworm image in QEMU
#

PREFIX=/home/ubuntu/code

QEMU_BIN=${PREFIX}/qemu/build/qemu-system-x86_64
NCPU=4
MEMSIZE=8G

KNL_SRC=${PREFIX}/linux
BZIMAGE=${KNL_SRC}/arch/x86/boot/bzImage

BOOKWORM_IMAGE=${PREFIX}/image/bookworm.img

sudo ${QEMU_BIN} \
        -m ${MEMSIZE} \
        -smp ${NCPU} \
        -kernel ${BZIMAGE} \
        -append "console=ttyS0 root=/dev/sda net.ifnames=0" \
        -drive file=${BOOKWORM_IMAGE},format=raw \
        -net user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:2222-:22 \
        -net nic,model=e1000 \
        -nographic \
        -device lkp_enc

