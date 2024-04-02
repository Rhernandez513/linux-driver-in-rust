# linux-driver-in-rust
Used for CS 594 Linux Kernel Programming at the University of Illinois at Chicago

![Build and Test Demo](/images/build_and_test_demo.jpg)

## Setup
Install Task for all local users
```sh
sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin
```

To build the Rust program

```sh
task build
```

To run the compiled Rust program

```sh
task run


To clean up and remove the compiled executable

```sh
task clean
```

To run the tests:

```sh
task test
```

## Rust-for-Linux env setup
1. Download the fork of linux from Rust-for-Linux: `git clone --depth=1 https://github.com/Rust-for-Linux/linux.git`
2. Download busybox: `git clone --depth=1 https://github.com/mirror/busybox.git`
3. Install clang and llvm: `sudo apt install clang llvm lld`
4. Install Rust: `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
5. Update the path with `source ~/.cargo/env` or `source $HOME/.cargo/env`
6. Set the required rust version used for the current kernel with `rustup override set $(scripts/min-tool-version.sh rustc)`
6. Download bindgen with `cargo install --locked --version $(scripts/min-tool-version.sh bindgen) bindgen-cli`
7. Run this command to install reaquired rust elements `rustup component add rust-src`
8. Go into linux folder and run the command `make LLVM=1 defconfig rust.config`
9. Proceed to compile the kernel with `make LLVM=1 -j4 CLIPPY=1`, where clippy is the rust linter
10. Enable support for rust-analyzer with `make LLVM=1 -j4 rust-analyzer`
11. To check whether the rust code written in the kernel is correctly formatted `make LLVM=1 -j4 rustfmtcheck`
12. To automatically format the rust code use `make LLVM=1 -j4 rustfmt`
13. To generate documentation run `make LLVM=1 -j4 rustdoc` for ease of development. To browse it go to rust/doc/kernel/index.html

1. Go to busybox `cd ../.busybox` assuming you are in the linux dir
2. Run `make defconfig`
3. Run `make menuconfig` and enable "Build static library" under Settings-->Build
4. Build busy box with `make -j4`
5. Run `make install`
6. Run `cd _install` and run `find . | cpio -H newc -o | gzip > ../ramdisk.img` to create the image
mkdir etc
cp ../examples/inittab ./etc
vim etc/inittab and comment tty2 to tty5
mkdir -p etc/init.d/
vim etc/init.d/rcS and write into it 
```
mkdir proc
mount -t proc none /proc
ifconfig lo up
udhcpc -i eth0
mkdir dev
mount -t devtmpfs none /dev
mkdir dev/pts
mount -t devpts nodev /dev/pts
telnetd -l /bin/sh
```
Make it executable with `chmod a+x etc/init.d/rcS`
mkdir -p usr/share/udhcpc
cp ../examples/udhcp/ simple.script usr/share/udhcpc/default.script


Boot the image moving to ../../linux and running `qemu-system-x86_64 -nographic -kernel vmlinux -initrd ../busybox/ramdisk.img -nic user,model=rtl8139,hostfwd=tcp::5555-:23,hostfwd=tcp::5556-:8080`

