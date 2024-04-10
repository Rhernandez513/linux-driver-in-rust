//! Rust HW9 driver module
//! 
//! This module is a Rust implementation of the HW9 driver module written in C.

use core::result::Result::Ok;
use core::ffi::c_void;
use kernel::sync::Mutex;
use kernel::bindings;
use kernel::{
    ioctl::{_IOR, _IOW}, prelude::*, Module
};
use kernel::file;
use kernel::chrdev;

module! {
    type: LkpEnc,
    name: "lkp_enc",
    author: "Riccardo Strina",
    description: "HW9... but in Rust!",
    license: "GPL",
}

const LKP_ENC_WRITE_SEED: u32 = _IOW::<u64>('q' as u32, 1);
const LKP_ENC_WRITE_STRING: u32 = _IOW::<&str>('q' as u32, 2);
const LKP_ENC_READ_STRING: u32 = _IOR::<&str>('q' as u32, 3);

static DEVMEM: Mutex<usize> = unsafe {
    Mutex::new(0)
};

#[allow(dead_code)]
const DEVICE_BASE_PHYS_ADDR: u64 = 0xfebd1000;

struct LkpEnc {
    _dev: Pin<Box<chrdev::Registration<1>>>,
    // _dev: Pin<Box<miscdev::Registration<LkpEnc>>>,
}

#[vtable]
impl file::Operations for LkpEnc {
    fn open(_context: &Self::OpenData,_file: &file::File) -> Result<Self::Data> {
        pr_info!("qemu file opened\n");
        Ok(())
    }

    fn write(_data: Self::Data,_file: &file::File,reader: &mut impl kernel::io_buffer::IoBufferReader,_offset:u64,) -> Result<usize> {
        pr_info!("qemu file written\n");
        Ok(reader.len())
    }

    fn read(_data: Self::Data,_file: &file::File,writer: &mut impl kernel::io_buffer::IoBufferWriter,offset:u64,) -> Result<usize> {
        pr_info!("qemu file read\n");
    
        // Message to write only once
        let message = b"Hello, World!\n";
        // If the offset is 0, it means we're starting to read from the beginning.
        // If the offset is greater than 0, in this simple case, we assume the message was already read,
        // and thus we return Ok(0) to indicate no more data is to be read.
        if offset == 0 {
            let _ = writer.write_slice(message);
            Ok(message.len())
        } else {
            // No more data to read, indicate this by returning 0 bytes read.
            Ok(0)
        }
    }

    fn ioctl(_data: <Self::Data as kernel::ForeignOwnable>::Borrowed<'_>,_file: &file::File,cmd: &mut file::IoctlCommand,) -> Result<i32> {
        pr_info!("qemu file ioctl\n");
        pr_info!("devmem {:?}", *DEVMEM.lock());
        
        match cmd.raw().0 {
            LKP_ENC_WRITE_SEED => {
                let seed = cmd.raw().1;
                pr_info!("LKP_ENC_WRITE_SEED : {:?}", seed);
            },
            LKP_ENC_READ_STRING => {
                pr_info!("LKP_ENC_READ_STRING: {}", "Hello, World!");
            },
            LKP_ENC_WRITE_STRING => {
                let string = cmd.raw().1;
                pr_info!("LKP_ENC_WRITE_STRING: {:?}", string);
            },
            _ => {
                pr_info!("Unknown IOCTL command");
            }
        }
        
        Ok(0)
    }
}

impl Module for LkpEnc {
    fn init(name: &'static CStr, module: &'static ThisModule) -> Result<Self> {
        pr_info!("{} module init", name);
        
        let addr = unsafe { bindings::ioremap(DEVICE_BASE_PHYS_ADDR, 4096) };
        
        // devmem know contains the address mapped in the CPU of the device

        let mut ptr = DEVMEM.lock();
        *ptr = if addr.is_null() {
            Err(ENOMEM)
        } else {
            // INVARIANT: `addr` is non-null and was returned by `ioremap`, so it is valid. It is
            // also 8-byte aligned because we checked it above.
            Ok(addr as usize)
        }?;

        pr_info!("devmem: {:?}", *ptr);

        let mut chardev_reg = chrdev::Registration::new_pinned(name, 0, module)?;
        chardev_reg.as_mut().register::<LkpEnc>()?;

        pr_info!("LKP_ENC_WRITE_SEED: {:?}", LKP_ENC_WRITE_SEED);
        pr_info!("LKP_ENC_READ_STRING: {:?}", LKP_ENC_READ_STRING);
        pr_info!("LKP_ENC_WRITE_STRING: {:?}", LKP_ENC_WRITE_STRING);

        Ok(LkpEnc { _dev: chardev_reg })
    }
}

impl Drop for LkpEnc {
    fn drop(&mut self) {
        pr_info!("lkp_enc module exit\n");
        unsafe { bindings::iounmap(*DEVMEM.lock() as *mut c_void ) };
    }
}