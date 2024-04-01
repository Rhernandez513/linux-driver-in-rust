# User space app to exploit a virtual device for encryption

## user_space_app

The `user_space_app.c` file is a source code file that contains the implementation of the encryption functionality for the virtual device.

### Purpose

The purpose of `user_space_app.c` is to provide encryption capabilities for strings. The user or the linux kernel can provide an encryption key and the device will then shift every single character of a given string of the same amount of letters specified by the encryption key. The device works only on lowercase and uppercase letters.

### Features

- Encryption: `user_space_app.c` implements encryption algorithms to protect data confidentiality.
- Decryption: It also provides decryption algorithms to reverse the encryption process and retrieve the original data.
- Key Management: The file includes functions for inserting and managing encryption keys.

### Dependencies

`user_space_app.c` has been developed using qemu v8.2.1 and linux v6.1.1. Other qemu & linux versions could work but are not guaranteed to be compatible.

### Usage

./user_space_app -s Hello,World! -k 7
