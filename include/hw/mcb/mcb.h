/*
 * QEMU MEN Chameleon Bus emulation
 *
 * Copyright (C) 2016 Johannes Thumshirn <jthumshirn@suse.de>
 *
 * This code is licensed under the GNU GPL v2 or (at your opinion) any
 * later version
 */

#ifndef QEMU_MCB_H
#define QEMU_MCB_H

#include "hw/qdev.h"

#define CHAMELEON_DTYPE_GENERAL  0x0
#define CHAMELEON_DTYPE_END 0xf

typedef struct {
	uint32_t reg1;
	uint32_t reg2;
	uint32_t offset;
	uint32_t size;
} ChameleonDeviceDescriptor;

#define GDD_DEV(x) (((x) & 0x3ff) << 18)
#define GDD_DTY(x) (((x) & 0xf) << 28)
#define GDD_REV(x) (((x) & 0x3f) << 5)
#define GDD_VAR(x) (((x) & 0x3f) << 11)

/* GDD Register 1 fields */
#define GDD_IRQ(x) ((x) & 0x1f)

/* GDD Register 2 fields */
#define GDD_BAR(x) ((x) & 0x7)
#define GDD_INS(x) (((x) >> 3) & 0x3f)
#define GDD_GRP(x) (((x) >> 9) & 0x3f)

typedef struct MCBus MCBus;

#define TYPE_MCB_BUS "MEN Chameleon Bus"
#define MCB_BUS(obj) OBJECT_CHECK(MCBus, (obj), TYPE_MCB_BUS)

struct MCBus {
    /*< private >*/
    BusState parent_obj;

    uint8_t n_slots;
    uint8_t free_slot;
    qemu_irq_handler set_irq;
    MemoryRegion mmio_region;
};

typedef struct MCBDevice MCBDevice;
typedef struct MCBDeviceClass MCBDeviceClass;

#define TYPE_MCB_DEVICE "mcb-device"
#define MCB_DEVICE(obj) \
    OBJECT_CHECK(MCBDevice, (obj), TYPE_MCB_DEVICE)
#define MCB_DEVICE_CLASS(klass) \
    OBJECT_CLASS_CHECK(MCBDeviceClass, (klass), TYPE_MCB_DEVICE)
#define MCB_DEVICE_GET_CLASS(obj) \
     OBJECT_GET_CLASS(MCBDeviceClass, (obj), TYPE_MCB_DEVICE)

struct MCBDeviceClass {
    /*< private >*/
    DeviceClass parent_class;
    /*< public >*/


    DeviceRealize realize;
    DeviceUnrealize unrealize;

    uint8_t (*men_read8)(MCBDevice *dev, uint32_t addr);
    void (*mem_write8)(MCBDevice *dev, uint32_t addr, uint8_t val);

    uint16_t (*men_read16)(MCBDevice *dev, uint32_t addr);
    void (*mem_write16)(MCBDevice *dev, uint32_t addr, uint16_t val);

    uint32_t (*men_read32)(MCBDevice *dev, uint32_t addr);
    void (*mem_write32)(MCBDevice *dev, uint32_t addr, uint32_t val);

    uint16_t device_id;
};

struct MCBDevice {
    /*< private >*/
    DeviceState parent_obj;
    /*< public >*/

    qemu_irq *irq;
    MemoryRegion mmio;
    ChameleonDeviceDescriptor *gdd;
    int slot;

    uint8_t rev;
    uint8_t var;
};

extern const VMStateDescription vmstate_mcb_device;

ChameleonDeviceDescriptor *mcb_new_chameleon_descriptor(MCBus *bus, uint8_t id,
                                                        uint8_t rev, uint8_t var,
                                                        uint32_t size);

#define VMSTATE_MCB_DEVICE(_field, _state)      \
    VMSTATE_STRUCT(_field, _state, 1, vmstate_mcb_device, MCBDevice)

MCBDevice *mcb_device_find(MCBus *bus, hwaddr addr);
void mcb_bus_new_inplace(MCBus *bus, size_t bus_size,
                         DeviceState *parent,
                         const char *name, uint8_t slots,
                         qemu_irq_handler handler);

qemu_irq mcb_allocate_irq(MCBDevice *dev);
#endif
