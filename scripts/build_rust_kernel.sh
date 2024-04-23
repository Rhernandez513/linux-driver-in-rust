#!/bin/sh -x

##########################
# build_rust_kernel.sh #
##########################
#
# This script is used to build the Linux Kernel with Rust enabled
#


# run this script at the root of your cloned linux kernel
# this script will install the rust toolchain on your system
# and then build linux kernel with rust enabled the rust toolchain

PREFIX=/home/ubuntu/code

sudo apt install clang llvm lld

curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

. ${PREFIX}/.cargo/env

rustup override set $(scripts/min-tool-version.sh rustc)

cargo install --locked --version $(scripts/min-tool-version.sh bindgen) bindgen-cli

rustup component add rust-src

make defconfig rust.config

echo "Next, you will need to enable the LKP_ENC device driver in the menuconfig Device Drivers -> Misc devices -> LKP_ENC"

make menuconfig

make LLVM=1 -j16 CLIPPY=1

make LLVM=1 -j16 rust-analyzer

make LLVM=1 -j16 rustfmtcheck

make LLVM=1 -j16 rustfmt

make LLVM=1 -j16 rustdoc
