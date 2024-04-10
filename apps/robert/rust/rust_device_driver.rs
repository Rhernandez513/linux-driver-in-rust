use linux_kernel_module::{kernel::file_operations::FileOp, prelude::*};
use spin::RwLock;

const LKP_ENC_IOCTL_RKEY: u32 = ior!('q', 1, std::mem::size_of::<u32>()) as u32;
const LKP_ENC_IOCTL_WKEY: u32 = iow!('q', 2, std::mem::size_of::<u32>()) as u32;
const LKP_ENC_IOCTL_RCH: u32 = ior!('q', 3, std::mem::size_of::<u32>()) as u32;
const LKP_ENC_IOCTL_WCH: u32 = iow!('q', 4, std::mem::size_of::<u32>()) as u32;

const DEVICE_BASE_PHYS_ADDR: usize = 0xfebf1000;
const MAX_LKP_ENC_BUFFER_SIZE: usize = 128;

static MY_LOCK: RwLock<()> = RwLock::const_new(());

struct LkpEncDriver;

impl KernelModule for LkpEncDriver {
    fn init() -> KernelResult<Self> {
        MY_LOCK.write().unwrap();

        let lkp_enc_devmem = unsafe { io::ioremap(DEVICE_BASE_PHYS_ADDR, 4096) };
        if lkp_enc_devmem.is_null() {
            pr_err!("Failed to map device registers in memory");
            return Err(KernelError::Generic);
        }

        let fops = FileOp::new().ioctl(Self::lkp_enc_ioctl);
        if register_chrdev(250, "lkp_enc_driver", fops) < 0 {
            pr_err!("Failed to register lkp_enc_driver");
            return Err(KernelError::Generic);
        }

        pr_info!(
            "lkp_enc_driver loaded, registered ioctls 0x{:x} (get enc key) and 0x{:x} (write enc key)",
            LKP_ENC_IOCTL_RKEY,
            LKP_ENC_IOCTL_WKEY
        );
        pr_info!(
            "lkp_enc_driver loaded, registered ioctls 0x{:x} (get chars) and 0x{:x} (write chars)",
            LKP_ENC_IOCTL_RCH,
            LKP_ENC_IOCTL_WCH
        );
        Ok(Self)
    }
}

impl LkpEncDriver {
    fn lkp_enc_ioctl(file: &File, cmd: u32, arg: u64) -> KernelResult<i64> {
        let flags = MY_LOCK.write();
        match cmd {
            LKP_ENC_IOCTL_RKEY => {
                pr_info!("IN LKP_ENC_IOCTL_RKEY, reading data from device");
                let mut lkp_enc_data: u32 = unsafe { io::ioread32(arg as *const u32) };
                pr_info!("copying data to userspace");
                let bytes_copied = file.write_object(&lkp_enc_data)?;
                if bytes_copied != std::mem::size_of::<u32>() {
                    pr_info!("Failed to copy data to user space");
                    return Err(KernelError::EFAULT);
                }
            }
            LKP_ENC_IOCTL_WKEY => {
                pr_info!("IN LKP_ENC_IOCTL_WKEY, writing data to device");
                let mut lkp_enc_data: u32 = 0;
                file.read_object(&mut lkp_enc_data)?;
                pr_info!("writing data to device");
                unsafe { io::iowrite32(lkp_enc_data, arg as *mut u32) };
            }
            LKP_ENC_IOCTL_RCH => {
                pr_info!("IN LKP_ENC_IOCTL_RCH, reading data from device");
                let mut i: usize = 1;
                let mut str_data = [0; MAX_LKP_ENC_BUFFER_SIZE];
                loop {
                    let lkp_enc_data: u32 = unsafe { io::ioread32(arg as *const u32).into() };
                    pr_info!("data returned from ioread32 as char: {}", lkp_enc_data as u8 as char);
                    str_data[i - 1] = lkp_enc_data as u8 as char;
                    i += 1;
                    if lkp_enc_data == 0 {
                        break;
                    }
                }
                pr_info!("copying data to userspace");
                let bytes_copied = file.write_slice(&str_data)?;
                if bytes_copied != MAX_LKP_ENC_BUFFER_SIZE {
                    pr_info!("Failed to copy data to user space");
                    return Err(KernelError::EFAULT);
                }
            }
            LKP_ENC_IOCTL_WCH => {
                pr_info!("IN LKP_ENC_IOCTL_WCH, writing data to device");
                let mut str_data = [0; MAX_LKP_ENC_BUFFER_SIZE];
                let bytes_read = file.read_slice(&mut str_data)?;
                let mut i = 0;
                while i < bytes_read && str_data[i] != 0 {
                    unsafe {
                        io::iowrite32(str_data[i] as u32, (arg as usize + i + 1) as *mut u32);
                    }
                    i += 1;
                }
            }
            _ => {
                return Err(KernelError::ENOTTY); // unknown command
            }
        }
        Ok(0)
    }
}

impl KernelModuleExit for LkpEncDriver {
    fn exit(&mut self) {
        unregister_chrdev(250, "lkp_enc_driver");
        unsafe {
            io::iounmap(DEVICE_BASE_PHYS_ADDR);
        }
        pr_info!("lkp_enc_driver unloaded");
    }
}

module! {
    type: LkpEncDriver,
    name: b"lkp_enc_driver",
    author: b"Author",
    description: b"LKP Enc Driver",
    license: b"GPL",
}
