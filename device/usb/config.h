#ifndef USBDEVICE_CONFIG_H
#define USBDEVICE_CONFIG_H

#include "common.h"
#include "interface.h"

namespace OHOS {
namespace DDK {
namespace USB {

// NOTE RAII本来要重写移动函数的，但是这里猜测多次Init或Free也不会崩溃，就暂时不写。也没有遵守5原则

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UsbConfigDescriptor, bLength, bDescriptorType, wTotalLength, bNumInterfaces,
                                   bConfigurationValue, iConfiguration, bmAttributes, bMaxPower)

class USBInterface;

/**
 * @brief USBConfig负责维护Descriptor的层级关系以及存储通过上层ArkTs拿到的值，内部类Descriptor负责封装C API的Descriptor
 */
class USBConfig : public Serializable {
public:
    class Descriptor {
    public:
        using sptr = std::shared_ptr<Descriptor>;
        using descriptor_type = UsbConfigDescriptor;
        using ddk_descriptor_type = UsbDdkConfigDescriptor;
        /**
         * @param deviceId
         * @param configIndex
         * @return
         * @see OH_Usb_GetConfigDescriptor
         */
        static Descriptor::sptr Get(std::uint64_t deviceId, std::uint8_t configIndex) {
            return std::make_shared<Descriptor>(deviceId, configIndex);
        }

        Descriptor(std::uint64_t deviceId, std::uint8_t configIndex) : deviceId_(deviceId), configIndex_(configIndex) {
            USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_GetConfigDescriptor(deviceId, configIndex, &ddk_descriptor_));
        }
        ~Descriptor() { OH_Usb_FreeConfigDescriptor(ddk_descriptor_); }

        Descriptor(const Descriptor &) = delete;
        Descriptor &operator=(const Descriptor &) = delete;
        Descriptor(Descriptor &&other) noexcept
            : deviceId_(other.deviceId_), configIndex_(other.configIndex_),
              ddk_descriptor_(std::exchange(other.ddk_descriptor_, nullptr)) {}
        Descriptor &operator=(Descriptor &&other) noexcept {
            if (this != &other) {
                ddk_descriptor_ = std::exchange(other.ddk_descriptor_, nullptr);
            }
            return *this;
        }

        std::uint64_t deviceId() const { return deviceId_; }
        std::uint8_t configIndex() const { return configIndex_; }
        const descriptor_type *const descriptor() const {
            if (ddk_descriptor_) {
                return &ddk_descriptor_->configDescriptor;
            }
            return nullptr;
        }

        const ddk_descriptor_type *ddkDescriptor() const { return ddk_descriptor_; }

    private:
        const std::uint64_t deviceId_;
        const std::uint8_t configIndex_;
        ddk_descriptor_type *ddk_descriptor_{nullptr};
    };

//    static USBConfig Get(std::uint64_t deviceId, std::uint8_t configIndex) {
//        return USBConfig(Descriptor::Get(deviceId, configIndex));
//    }

    ~USBConfig() = default;

    std::uint64_t deviceId() const { return deviceId_; }
    std::uint8_t configIndex() const { return configIndex_; }
    Descriptor::sptr descriptor() const { return Descriptor::Get(deviceId(), configIndex()); }

    std::shared_ptr<USBInterface> interfaceOf(std::uint32_t index) { return interfaces_[index]; }
    const std::vector<std::shared_ptr<USBInterface>> &interfaces() { return interfaces_; }
    std::shared_ptr<USBInterface> interfaceOf(std::uint32_t index) const { return interfaces_[index]; }
    const std::vector<std::shared_ptr<USBInterface>> &interfaces() const { return interfaces_; }

    std::optional<json> toJson() const override {
        if (parsed_) {
            return std::nullopt;
        }
        json j;
        j["id"] = id();
        j["attributes"] = attributes();
        j["isRemoteWakeup"] = isRemoteWakeup();
        j["isSelfPowered"] = isSelfPowered();
        j["maxPower"] = maxPower();
        j["name"] = name();
        for (const auto &interface : interfaces_) {
            j["interfaces"].emplace_back(interface->toJson());
        }
        return j;
    }

    USBConfig(std::uint64_t deviceId, std::uint8_t configIndex) : deviceId_(deviceId), configIndex_(configIndex) {}
    USBConfig(std::uint64_t deviceId, std::uint8_t configIndex, std::uint32_t id, std::uint32_t attributes,
              std::string name, std::uint32_t maxPower, std::vector<std::shared_ptr<USBInterface>> interfaces)
        : deviceId_(deviceId), configIndex_(configIndex), id_(static_cast<int32_t>(id)), attributes_(attributes),
          maxPower_(static_cast<int32_t>(maxPower)), name_(name), interfaces_(interfaces) {
        parsed_ = true;
    }
    USBConfig(std::uint64_t deviceId, std::uint8_t configIndex, json j)
        : deviceId_(deviceId), configIndex_(configIndex), id_(j["id"]), attributes_(j["attributes"]),
          maxPower_(j["maxPower"]), name_(j["name"]) {
        for (const auto &interface : j["interfaces"]) {
            interfaces_.emplace_back(std::make_shared<USBInterface>(deviceId_, interface));
        }
        parsed_ = true;
    }
    explicit USBConfig(Descriptor::sptr descriptor)
        : deviceId_(descriptor->deviceId()), configIndex_(descriptor->configIndex()) {
        for (std::uint8_t i = 0; i < descriptor->descriptor()->bNumInterfaces; ++i) {
            interfaces_.emplace_back(std::make_shared<USBInterface>(deviceId_, i));
        }
        // FIXME 这里不知道是不是这样获取备用配置
        auto *alts = descriptor->ddkDescriptor()->interface;
        if (alts) {
            for (std::uint8_t i = 0; i < alts->numAltsetting; ++i) {
                altsettings_.emplace_back(USBInterface::Attach(deviceId_, alts->altsetting[i]));
            }
        }
    }

    std::int32_t id() const { return id_; }

    std::uint32_t attributes() const { return attributes_; }

    std::uint32_t interfaceCount() const { return interfaces_.size(); }

    std::int32_t maxPower() const {
        return maxPower_ * 2; // 2 represent maxPower units
    }

    const std::string &name() const { return name_; }

    std::uint8_t iConfiguration() { return iConfiguration_; }

    bool isRemoteWakeup() const { return (attributes_ & USB_CFG_REMOTE_WAKEUP) != 0; }

    bool isSelfPowered() const { return (attributes_ & USB_CFG_SELF_POWERED) != 0; }

    void setInterfaces(const std::vector<std::shared_ptr<USBInterface>> &interfaces) { interfaces_ = interfaces; }

    void setId(std::int32_t id) { id_ = id; }

    void setAttribute(std::uint32_t attributes) { attributes_ = attributes; }

    void setMaxPower(std::int32_t maxPower) { maxPower_ = maxPower; }

    void setName(const std::string &name) { name_ = name; }

    void setiConfiguration(std::uint8_t idx) { iConfiguration_ = idx; }

private:
    const std::uint64_t deviceId_;
    const std::uint8_t configIndex_;
    std::vector<std::shared_ptr<USBInterface>> interfaces_;
    std::vector<std::shared_ptr<USBInterface>> altsettings_; // 备用配置

    std::int32_t id_ = INVALID_USB_INT_VALUE;
    std::uint32_t attributes_ = 0;
    std::int32_t maxPower_ = INVALID_USB_INT_VALUE;
    std::string name_;
    std::uint8_t iConfiguration_ = UINT8_MAX;
};

constexpr std::uint32_t USB_CONFIG_MAX_NUM = 128;

} // namespace USB
} // namespace DDK
} // namespace OHOS

#endif // USBDEVICE_CONFIG_H
