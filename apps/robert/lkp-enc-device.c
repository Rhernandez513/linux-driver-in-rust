#include "qemu/osdep.h"
#include "hw/pci/msi.h"
#include "hw/pci/pci.h"
#include <ctype.h>
#include <stdint.h>

#define TYPE_LKP_ENC "lkp_enc"
#define LKP_ENC(obj) OBJECT_CHECK(lkp_enc, (obj), TYPE_LKP_ENC)
#define MAX_LKP_ENC_BUFFER_SIZE 128

typedef struct {
    PCIDevice parent_obj;
    uint64_t encryption_key;
    char buffer[MAX_LKP_ENC_BUFFER_SIZE];
    MemoryRegion mmio;
} lkp_enc;

static char modify_char(int decimal, int modifier);

static uint64_t mmio_read(void *opaque, hwaddr addr, unsigned size) {
    lkp_enc *dev;
    dev = (lkp_enc *)opaque;

    if(addr == 0x0) {
        return dev->encryption_key;
    }

    // Ensure that the address is within the bounds of the buffer
    if (addr < sizeof(dev->buffer)) {
        // As we are not attempting to read the encryption key,
        // we need to decrement the offset by 1
        --addr;

        // Calculate the offset within the buffer based on the address
        char *buffer_addr = dev->buffer + addr;

        // Return the byte at the calculated offset
        return *buffer_addr;
    }

    // Handle out-of-bounds access, for now, returning 0
    return 0;
}

static void mmio_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
    lkp_enc *dev;
    dev = (lkp_enc *)opaque;

    if(addr == 0x0) {
        dev->encryption_key = val;
        return;
    }

    // Ensure that the address is within the bounds of the buffer
    if (addr < sizeof(dev->buffer)) {
        // As we are not attempting to write the encryption key,
        // we need to decrement the offset by 1
        --addr;

        // Calculate the offset within the buffer based on the address
        char *buffer_addr = dev->buffer + addr;

        *buffer_addr = modify_char((int)val, (int) dev->encryption_key);
    }
}

static const MemoryRegionOps lkp_enc_ops = {
    .read = mmio_read,
    .write = mmio_write,
};

static void lkp_enc_realize(PCIDevice *pdev, Error **errp) {
    lkp_enc *s = LKP_ENC(pdev);
    memory_region_init_io(&s->mmio, OBJECT(s), &lkp_enc_ops, s,
                          "lkp-enc", 4096);
    pci_register_bar(&s->parent_obj, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mmio);
}

static void lkp_enc_class_init(ObjectClass *class, void *data) {
    DeviceClass *dc = DEVICE_CLASS(class);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(class);

    k->realize = lkp_enc_realize;
    k->vendor_id = PCI_VENDOR_ID_QEMU;
    k->device_id = 0xbeef;
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
        .class_init = lkp_enc_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&lkp_enc_info);
}

static char modify_char(int decimal, int modifier) {
    // Check if the decimal represents an uppercase letter
    if (decimal >= 'A' && decimal <= 'Z') {
        // Calculate the new decimal value with wrapping
        int new_decimal = ((decimal - 'A' + modifier) % 26) + 'A';
        return (char)new_decimal;
    }
    // Check if the decimal represents a lowercase letter
    else if (decimal >= 'a' && decimal <= 'z') {
        // Calculate the new decimal value with wrapping
        int new_decimal = ((decimal - 'a' + modifier) % 26) + 'a';
        return (char)new_decimal;
    }
    // Return the original character if not in alphabet
    else {
        return (char)decimal;
    }
}

type_init(lkp_enc_register_types);
