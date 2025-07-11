#ifndef USBDEVICE_INTERFACE_H
#define USBDEVICE_INTERFACE_H

#include "common.h"
#include "endpoint.h"

namespace OHOS {
namespace DDK {
namespace USB {

// NOTE RAII本来要重写移动函数的，但是这里猜测多次Init或Free也不会崩溃，就暂时不写。也没有遵守5原则

// TODO UsbDdkConfigDescriptor这个结构不知道怎么获取

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UsbInterfaceDescriptor, bLength, bDescriptorType, bInterfaceNumber,
                                   bAlternateSetting, bNumEndpoints, bInterfaceClass, bInterfaceSubClass,
                                   bInterfaceProtocol, iInterface)

class USBInterface : public Serializable {
public:
    using descriptor_type = UsbInterfaceDescriptor;
    using ddk_descriptor_type = UsbDdkInterfaceDescriptor;

    class Handle {
    public:
        using sptr = std::shared_ptr<Handle>;
        using handle_type = std::uint64_t;

        /**
         * @param deviceId
         * @param interfaceIndex
         * @return
         * @see OH_Usb_ClaimInterface
         */
        static Handle::sptr Claim(std::uint64_t deviceId, std::uint8_t interfaceIndex) {
            return std::make_shared<Handle>(deviceId, interfaceIndex);
        }

        Handle(std::uint64_t deviceId, std::uint8_t interfaceIndex)
            : deviceId_(deviceId), interfaceIndex_(interfaceIndex) {
            USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_ClaimInterface(deviceId, interfaceIndex, &handle_));
        }

        Handle(const Handle &) = delete;
        Handle &operator=(const Handle &) = delete;
        Handle(Handle &&) = default;
        Handle &operator=(Handle &&) = default;
        ~Handle() { USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_ReleaseInterface(handle_)); }

        std::uint64_t deviceId() const { return deviceId_; }
        std::uint8_t interfaceIndex() const { return interfaceIndex_; }
        handle_type handle() const { return handle_; }

        std::uint8_t currentInterfacesetting() const {
            std::uint8_t settingIndex = 0;
            USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_GetCurrentInterfaceSetting(handle_, &settingIndex));
            return settingIndex;
        }

        void selectInterfacesetting(std::uint8_t settingIndex) const {
            USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_SelectInterfaceSetting(handle_, settingIndex));
        }

        void read(std::uint8_t *dataRead, std::uint32_t *dataReadLen, std::uint32_t timeout_ms) const {
            UsbControlRequestSetup setupRead;
            setupRead.bmRequestType = USB_ENDPOINT_DIR_IN;
            setupRead.bRequest = 0x08;
            setupRead.wValue = 0;
            setupRead.wIndex = 0;
            setupRead.wLength = 0x01;
            USB_CHECK_ERROR_INLINE_DEFAULT(
                OH_Usb_SendControlReadRequest(handle_, &setupRead, timeout_ms, dataRead, dataReadLen));
        }

        void write(std::uint8_t *dataWrite, std::uint32_t dataWriteLen, std::uint32_t timeout_ms) const {
            UsbControlRequestSetup setupWrite;
            setupWrite.bmRequestType = USB_ENDPOINT_DIR_OUT;
            setupWrite.bRequest = 0x09;
            setupWrite.wValue = 1;
            setupWrite.wIndex = 0;
            setupWrite.wLength = 0;
            USB_CHECK_ERROR_INLINE_DEFAULT(
                OH_Usb_SendControlWriteRequest(handle_, &setupWrite, timeout_ms, dataWrite, dataWriteLen));
        }

    private:
        std::uint64_t deviceId_{UINT64_MAX};
        std::uint8_t interfaceIndex_{UINT8_MAX};
        handle_type handle_{UINT64_MAX};
    };

//    static USBInterface Claim(std::uint64_t deviceId, std::uint8_t interfaceIndex) {
//        return USBInterface(Handle::Claim(deviceId, interfaceIndex));
//    }

    static std::shared_ptr<USBInterface> Attach(std::uint64_t deviceId, ddk_descriptor_type ddk_descriptor) {
        return std::make_shared<USBInterface>(deviceId, ddk_descriptor);
    }

    ~USBInterface() = default;

    std::uint64_t deviceId() const { return deviceId_; }
    std::uint8_t interfaceIndex() const { return interfaceIndex_; }
    Handle::sptr handle() const { return handle_; }

    std::shared_ptr<USBEndpoint> endpointOf(std::uint32_t index) { return endpoints_[index]; }
    std::vector<std::shared_ptr<USBEndpoint>> &endpoints() { return endpoints_; }
    std::shared_ptr<USBEndpoint> endpointOf(std::uint32_t index) const { return endpoints_[index]; }
    const std::vector<std::shared_ptr<USBEndpoint>> &endpoints() const { return endpoints_; }

    std::optional<json> toJson() const override {
        if (parsed_) {
            return std::nullopt;
        }
        json j;
        j["id"] = id();
        j["protocol"] = protocol();
        j["clazz"] = clazz();
        j["subClass"] = subClass();
        j["alterateSetting"] = alternateSetting();
        j["name"] = name();
        // TODO 由于不知道UsbDdkConfigDescriptor如何获取，所以拿不到UsbConfigDescriptor，所以打印不了
        for (const auto &endpoint : endpoints_) {
            j["endpoints"].emplace_back(endpoint->toJson());
        }
        return j;
    }

    // FIXME 这个函数确定是这样实现吗
    bool isAlternateSetting() const { return ddk_descriptor_.has_value(); }

    std::uint8_t currentInterfacesetting() const { return handle_->currentInterfacesetting(); }

    void selectInterfacesetting(std::uint8_t settingIndex) const { handle_->selectInterfacesetting(settingIndex); }

    void claim() { handle_ = Handle::Claim(deviceId_, interfaceIndex_); }

    void release() { handle_.reset(); }

    void read(std::uint8_t *dataRead, std::uint32_t *dataReadLen, std::uint32_t timeout_ms) const {
        handle_->read(dataRead, dataReadLen, timeout_ms);
    }

    void write(std::uint8_t *dataWrite, std::uint32_t dataWriteLen, std::uint32_t timeout_ms) const {
        handle_->write(dataWrite, dataWriteLen, timeout_ms);
    }

    USBInterface(std::uint64_t deviceId, std::uint8_t interfaceIndex)
        : deviceId_(deviceId), interfaceIndex_(interfaceIndex) {}
    explicit USBInterface(Handle::sptr handle)
        : handle_(handle), deviceId_(handle->deviceId()), interfaceIndex_(handle->interfaceIndex()) {}
    explicit USBInterface(std::uint64_t deviceId, ddk_descriptor_type ddk_descriptor)
        : deviceId_(deviceId), interfaceIndex_(ddk_descriptor.interfaceDescriptor.bInterfaceNumber),
          ddk_descriptor_(ddk_descriptor) {}

    USBInterface(std::uint64_t deviceId, std::uint8_t interfaceIndex, std::int32_t id, std::int32_t protocol,
                 std::int32_t interfaceClass, std::int32_t subClass, std::int32_t alternatesetting, std::string name,
                 std::vector<std::shared_ptr<USBEndpoint>> endpoints)
        : deviceId_(deviceId), interfaceIndex_(interfaceIndex), id_(id), protocol_(protocol), clazz_(interfaceClass),
          subClass_(subClass), alternateSetting_(alternatesetting), name_(name), endpoints_(endpoints),
          iInterface_(UINT8_MAX) {
        parsed_ = true;
    }
    USBInterface(std::uint64_t deviceId, std::uint8_t interfaceIndex, json j)
        : deviceId_(deviceId), interfaceIndex_(interfaceIndex), id_(j["id"]), protocol_(j["protocol"]),
          clazz_(j["clazz"]), subClass_(j["subClass"]), alternateSetting_(j["alternateSetting"]), name_(j["name"]) {
        for (auto &endpoint : j["endpoints"]) {
            endpoints_.emplace_back(std::make_shared<USBEndpoint>(endpoint));
        }
        parsed_ = true;
    }

    const std::string &name() const { return name_; }

    std::int32_t id() const { return id_; }

    std::int32_t clazz() const { return clazz_; }

    std::int32_t subClass() const { return subClass_; }

    std::int32_t alternateSetting() const { return alternateSetting_; }

    std::int32_t protocol() const { return protocol_; }

    std::int32_t endpointCount() const { return endpoints_.size(); }

    std::uint8_t iInterface() { return iInterface_; }

    void setEndpoints(const std::vector<std::shared_ptr<USBEndpoint>> &eps) { endpoints_ = eps; }

    void setId(std::int32_t id) { id_ = id; }

    void setProtocol(std::int32_t protocol) { protocol_ = protocol; }

    void setClazz(std::int32_t clazz) { clazz_ = clazz; }

    void setSubClass(std::int32_t subClass) { subClass_ = subClass; }

    void setAlternatesetting(std::int32_t alternatesetting) { alternateSetting_ = alternatesetting; }

    void setName(const std::string &name) { name_ = name; }

    void setiInterface(std::uint8_t idx) { iInterface_ = idx; }

private:
    const std::uint64_t deviceId_;
    const std::uint8_t interfaceIndex_;
    std::vector<std::shared_ptr<USBEndpoint>> endpoints_;
    Handle::sptr handle_;
    std::optional<ddk_descriptor_type> ddk_descriptor_;

    std::int32_t id_ = INT32_MAX;
    std::int32_t protocol_ = INT32_MAX;
    std::int32_t clazz_ = INT32_MAX;
    std::int32_t subClass_ = INT32_MAX;
    std::int32_t alternateSetting_ = INT32_MAX;
    std::string name_;
    std::uint8_t iInterface_ = UINT8_MAX;
};

constexpr std::uint32_t USB_INTERFACE_MAX_NUM = 128;

} // namespace USB
} // namespace DDK
} // namespace OHOS

#endif // USBDEVICE_INTERFACE_H
