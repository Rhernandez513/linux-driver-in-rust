# Virtual Device for encryption

## lkp_enc.c

The `lkp_enc.c` file is a source code file that contains the implementation of the encryption functionality for the virtual device.

### Purpose

The purpose of `lkp_enc.c` is to provide encryption capabilities for strings. The user or the linux kernel can provide an encryption key and the device will then shift every single character of a given string of the same amount of letters specified by the encryption key. The device works only on lowercase and uppercase letters.

### Features

- Encryption: `lkp_enc.c` implements encryption algorithms to protect data confidentiality.
- Decryption: It also provides decryption algorithms to reverse the encryption process and retrieve the original data.
- Key Management: The file includes functions for inserting and managing encryption keys.

### Usage

To use the encryption functionality provided by `lkp_enc.c`, you need to include the file in your qemu folder under /hw/misc/. Then you should compile qemu and enclude the device when you start your virtual machine. After this, the device should be visible by your kernel.

If you don't want to do it manually, you can apply the given patches. To do so, copy the `qemu.patch` file in the root file of the project and run the command `patch -p0 < qemu.patch`. After that, route inside the qemu folder and run `make`. Do the same with `linux.patch`.

### Dependencies

`lkp_enc.c` has been developed using qemu v8.2.1 and linux v6.1.1. Other qemu & linux versions could work but are not guaranteed to be compatible.
