#ifndef USBDEVICE_ENDPOINT_H
#define USBDEVICE_ENDPOINT_H

#include "common.h"

namespace OHOS {
namespace DDK {
namespace USB {

// NOTE RAII本来要重写移动函数的，但是这里猜测多次Init或Free也不会崩溃，就暂时不写。也没有遵守5原则

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UsbEndpointDescriptor, bLength, bDescriptorType, bEndpointAddress, bmAttributes,
                                   wMaxPacketSize, bInterval, bRefresh, bSynchAddress)

class USBEndpoint : public Serializable {
public:
    using descriptor_type = UsbEndpointDescriptor;
    using ddk_descriptor_type = UsbDdkEndpointDescriptor;
    explicit USBEndpoint() = default;
    ~USBEndpoint() = default;

    std::optional<json> toJson() const override {
        if (parsed_) {
            return std::nullopt;
        }
        json j;
        j["address"] = address();
        j["attributes"] = attributes();
        j["interval"] = interval();
        j["maxPacketSize"] = maxPacketSize();
        j["direction"] = direction();
        j["number"] = number();
        j["type"] = type();
        j["interfaceId"] = InterfaceId();
        return j;
    }

    USBEndpoint(json j)
        : address_(j["address"]), attributes_(j["attributes"]), interval_(j["interval"]),
          maxPacketSize_(j["maxPacketSize"]), interfaceId_(j["interfaceId"]) {
        parsed_ = true;
    }

    std::uint8_t number() const { return address_ & USB_ENDPOINT_NUMBER_MASK; }

    std::uint32_t address() const { return address_; }

    std::uint32_t direction() const { return address_ & USB_ENDPOINT_DIR_MASK; }

    std::uint32_t attributes() const { return attributes_; }

    std::uint32_t endpointNumber() const { return address_ & USB_ENDPOINT_NUMBER_MASK; }

    std::int32_t interval() const { return interval_; }

    std::int32_t maxPacketSize() const { return maxPacketSize_; }

    std::uint32_t type() const { return (attributes_ & USB_ENDPOINT_XFERTYPE_MASK); }

    std::int8_t InterfaceId() const { return interfaceId_; }

    void setAddr(std::uint32_t val) { address_ = val; }

    void setAttr(std::uint32_t val) { attributes_ = val; }

    void setInterval(std::int32_t val) { interval_ = val; }

    void setMaxPacketSize(std::int32_t val) { maxPacketSize_ = val; }

    void setInterfaceId(std::uint8_t interfaceId) { this->interfaceId_ = interfaceId; }

private:
    std::uint32_t address_ = 0;
    std::uint32_t attributes_ = 0;
    int32_t interval_ = INVALID_USB_INT_VALUE;
    int32_t maxPacketSize_ = INVALID_USB_INT_VALUE;
    std::uint8_t interfaceId_ = UINT8_MAX;
};

constexpr std::uint32_t USB_ENDPOINT_MAX_NUM = 128;

} // namespace USB
} // namespace DDK
} // namespace OHOS

#endif // USBDEVICE_ENDPOINT_H
