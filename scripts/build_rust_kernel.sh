#!/bin/sh

# run this script at the root of your cloned linux kernel

PREFIX=/home/ubuntu/code

sudo apt install clang llvm lld

curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

. ${PREFIX}/.cargo/env

rustup override set $(scripts/min-tool-version.sh rustc)

cargo install --locked --version $(scripts/min-tool-version.sh bindgen) bindgen-cli

rustup component add rust-src

make defconfig rust.config

make LLVM=1 -j8 CLIPPY=1

make LLVM=1 -j8 rust-analyzer

make LLVM=1 -j8 rustfmtcheck

make LLVM=1 -j8 rustfmt

make LLVM=1 -j8 rustdoc
