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
