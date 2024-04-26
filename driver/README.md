## Guide for applying the patch

The patch was created against commit with hash 18b7491480025420896e0c8b73c98475c3806c6f of Rust-for-linux/linux branch: rust
The patch will had the configuration to enable the device driver, the device driver written in rust and it will also include a file added to the rust directory to enable the use of ioctl number generation directly.
Finally, it edit also the main.c file with a test run to see whether the device is properly working using c
