#ifndef USBDEVICE_PIPE_H
#define USBDEVICE_PIPE_H

#include "common.h"

namespace OHOS {
namespace DDK { 
namespace USB {

// NOTE RAII本来要重写移动函数的，但是这里猜测多次Init或Free也不会崩溃，就暂时不写。也没有遵守5原则

class UsbDeviceMemMap : public MemMap {
public:
    UsbDeviceMemMap(std::uint64_t deviceId, std::size_t size)
        : deviceId_(deviceId)
    {
        USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_CreateDeviceMemMap(deviceId, size, &devMmap_));
    }

    ~UsbDeviceMemMap() {
        USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_DestroyDeviceMemMap (devMmap_));
    }

    std::uint64_t deviceId() const { return deviceId_; }
    uint8_t * const address() const {
        return devMmap_ ? devMmap_->address : nullptr;
    }
    std::uint32_t size() const override {
        return devMmap_ ? devMmap_->size : 0;
    }
    std::uint32_t offset() const override {
        return devMmap_ ? devMmap_->offset : 0;
    }
    std::uint32_t bufferLength() const override {
        return devMmap_ ? devMmap_->bufferLength : 0;
    }
    std::uint32_t transferredLength() const override {
        return devMmap_ ? devMmap_->transferredLength : 0;
    }

private:
    struct UsbDeviceMemMap * const devMmap_;
    std::uint64_t deviceId_;
};

class UsbRequestPipe {
public:
    UsbRequestPipe(std::uint64_t interfaceHandle, std::uint8_t endpoint, std::uint32_t timeout)
        : interfaceHandle_(interfaceHandle), endpoint_(endpoint), timeout_(timeout)
    {
    }
    ~UsbRequestPipe() = default;

    void sendRequest(UsbDeviceMemMap *memMap) const {
        USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_SendPipeRequest(&pipe_, memMap));
    }

    void sendRequest(Ashmem *memMap) const {
        USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_SendPipeRequestWithAshmem(&pipe_, memMap));
    }

private:
    struct UsbRequestPipe pipe_;
    std::uint64_t interfaceHandle_;
    std::uint8_t endpoint_;
    std::uint32_t timeout_{UINT32_MAX};
};

} // namespace USB
} // namespace DDK
} // namespace OHOS

#endif //USBDEVICE_PIPE_H
