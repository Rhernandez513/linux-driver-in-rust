//! Rust HW9 driver module
//! 
//! This module is a Rust implementation of the HW9 driver module written in C.

use kernel::{
    ioctl::{_IOR, _IOW}, prelude::*, Module
};

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

struct LkpEnc {
    devmem: u64,
}

// declare ioctl operations

impl Module for LkpEnc {
    fn init(_module: &'static ThisModule) -> Result<Self> {
        pr_info!("lkp_enc module init\n");
        
        // obtain device memory
        // register device
        let devmem = 0;

        pr_info!("\n{:?}\n{:?}\n{:?}\n{:?}", LKP_ENC_READ_SEED, LKP_ENC_WRITE_SEED, LKP_ENC_READ_STRING, LKP_ENC_WRITE_STRING);

        pr_info!("{:?}", devmem);

        Ok(LkpEnc { devmem })
    }
}

impl Drop for LkpEnc {
    fn drop(&mut self) {
        pr_info!("lkp_enc module exit\n");

        // unregister device

        pr_info!("{:?}", self.devmem);
    }
}