// SPDX-License-Identifier: GPL-2.0

//! ioctl() number definitions
//!
//! C header: [`include/asm-generic/ioctl.h`](../../../../include/asm-generic/ioctl.h)

#![allow(non_snake_case)]

use crate::build_assert;

// Values copied from include/uapi/asm-generic/ioctl.h
// TODO: find a way to use bindgen to generate these values

#[allow(dead_code)]
const _IOC_NRBITS: u32 = 8;
const _IOC_TYPEBITS: u32 = 8;
const _IOC_SIZEBITS: u32 = 14;
const _IOC_DIRBITS: u32 = 2;
const _IOC_NRMASK: u32 = 255;
const _IOC_TYPEMASK: u32 = 255;
#[allow(missing_docs)]
pub const _IOC_SIZEMASK: u32 = 16383;
const _IOC_DIRMASK: u32 = 3;
const _IOC_NRSHIFT: u32 = 0;
const _IOC_TYPESHIFT: u32 = 8;
#[allow(missing_docs)]
pub const _IOC_SIZESHIFT: u32 = 16;
const _IOC_DIRSHIFT: u32 = 30;
const _IOC_NONE: u32 = 0;
const _IOC_WRITE: u32 = 1;
const _IOC_READ: u32 = 2;
#[allow(dead_code)]
const IOC_IN: u32 = 1073741824;
#[allow(dead_code)]
const IOC_OUT: u32 = 2147483648;
#[allow(dead_code)]
const IOC_INOUT: u32 = 3221225472;
#[allow(dead_code)]
const IOCSIZE_MASK: u32 = 1073676288;
#[allow(dead_code)]
const IOCSIZE_SHIFT: u32 = 16;

/// Build an ioctl number, analogous to the C macro of the same name.
#[inline(always)]
const fn _IOC(dir: u32, ty: u32, nr: u32, size: usize) -> u32 {
    build_assert!(dir <= _IOC_DIRMASK);
    build_assert!(ty <= _IOC_TYPEMASK);
    build_assert!(nr <= _IOC_NRMASK);
    build_assert!(size <= (_IOC_SIZEMASK as usize));

    (dir << _IOC_DIRSHIFT)
        | (ty << _IOC_TYPESHIFT)
        | (nr << _IOC_NRSHIFT)
        | ((size as u32) << _IOC_SIZESHIFT)
}

/// Build an ioctl number for an argumentless ioctl.
#[inline(always)]
pub const fn _IO(ty: u32, nr: u32) -> u32 {
    _IOC(_IOC_NONE, ty, nr, 0)
}

/// Build an ioctl number for an read-only ioctl.
#[inline(always)]
pub const fn _IOR<T>(ty: u32, nr: u32) -> u32 {
    _IOC(_IOC_READ, ty, nr, core::mem::size_of::<T>())
}

/// Build an ioctl number for an write-only ioctl.
#[inline(always)]
pub const fn _IOW<T>(ty: u32, nr: u32) -> u32 {
    _IOC(_IOC_WRITE, ty, nr, core::mem::size_of::<T>())
}

/// Build an ioctl number for a read-write ioctl.
#[inline(always)]
pub const fn _IOWR<T>(ty: u32, nr: u32) -> u32 {
    _IOC(
        _IOC_READ | _IOC_WRITE,
        ty,
        nr,
        core::mem::size_of::<T>(),
    )
}

/// Get the ioctl direction from an ioctl number.
pub const fn _IOC_DIR(nr: u32) -> u32 {
    (nr >> _IOC_DIRSHIFT) & _IOC_DIRMASK
}

/// Get the ioctl type from an ioctl number.
pub const fn _IOC_TYPE(nr: u32) -> u32 {
    (nr >> _IOC_TYPESHIFT) & _IOC_TYPEMASK
}

/// Get the ioctl number from an ioctl number.
pub const fn _IOC_NR(nr: u32) -> u32 {
    (nr >> _IOC_NRSHIFT) & _IOC_NRMASK
}

/// Get the ioctl size from an ioctl number.
pub const fn _IOC_SIZE(nr: u32) -> usize {
    ((nr >> _IOC_SIZESHIFT) & _IOC_SIZEMASK) as usize
}
