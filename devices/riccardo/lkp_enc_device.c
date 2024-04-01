#include "qemu/osdep.h"
#include "hw/pci/msi.h"
#include "hw/pci/pci.h"

#define TYPE_LKP_ENC "lkp_enc"
#define LKP_ENC(obj) OBJECT_CHECK(lkp_enc, (obj), TYPE_LKP_ENC)

typedef struct {
    PCIDevice parent_obj;
    uint32_t seed_register;
    char string[256];
    MemoryRegion mmio;
} lkp_enc;

static uint64_t mmio_read(void *opaque, hwaddr addr, unsigned size) {
    lkp_enc *dev = (lkp_enc *)opaque;

    if(addr == 0x0) {
        return dev->seed_register;
    }
    else if(addr >= 0x4 && size == 1) {
        int index = addr - 0x4;

        return dev->string[index];
    }

    return 0x0;
}

static char cipher(char c, uint32_t seed) {
    if('A' <= c && c <= 'Z') {
        return ((c - 'A') + seed) % 26 + 'A';
    }
    else if('a' <= c && c <= 'z') {
        return ((c - 'a') + seed) % 26 + 'a';
    }
    else {
        return c;
    }
}

static void mmio_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
    lkp_enc *dev = (lkp_enc *)opaque;

    if(addr == 0x0) {
        dev->seed_register = val;
    }
    else if(addr >= 0x4 && size == 1) {
        int index = addr - 0x4;
        char app = (char)val;

        dev->string[index] = cipher(app, dev->seed_register);
    }

    return;
}

static const MemoryRegionOps lkp_enc_ops = {
    .read = mmio_read,
    .write = mmio_write,
};

static void lkp_enc_realize(PCIDevice *pdev, Error **errp) {
    lkp_enc *s = LKP_ENC(pdev);
    memory_region_init_io(&s->mmio, OBJECT(s), &lkp_enc_ops, s,
                          "lkp_enc", 4096);
    pci_register_bar(&s->parent_obj, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mmio);
}

static void lkp_enc_class_init(ObjectClass *class, void *data) {
    DeviceClass *dc = DEVICE_CLASS(class);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(class);

    k->realize = lkp_enc_realize;
    k->vendor_id = PCI_VENDOR_ID_QEMU;
    k->device_id = 0xcafe;
    k->revision = 0x10;
    k->class_id = PCI_CLASS_OTHERS;
    
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static void lkp_enc_register_types(void) {
    static InterfaceInfo interfaces[] = {
        { INTERFACE_CONVENTIONAL_PCI_DEVICE },
        { },
    };

    static const TypeInfo lkp_enc_info = {
        .name = TYPE_LKP_ENC,
        .parent = TYPE_PCI_DEVICE,
        .instance_size = sizeof(lkp_enc),
        .class_init    = lkp_enc_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&lkp_enc_info);
}

type_init(lkp_enc_register_types)
