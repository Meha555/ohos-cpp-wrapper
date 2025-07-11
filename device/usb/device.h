#ifndef USBDEVICE_DEVICE_H
#define USBDEVICE_DEVICE_H

#include "common.h"
#include "config.h"

namespace OHOS {
namespace DDK {
namespace USB {

// constexpr std::uint64_t kDeviceIdNone = 0xFFFFFFFFFFFFFFFF;

// NOTE RAII本来要重写移动函数的，但是这里猜测多次Init或Free也不会崩溃，就暂时不写。也没有遵守5原则

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UsbDeviceDescriptor, bLength, bDescriptorType, bcdUSB, bDeviceClass, bDeviceSubClass,
                                   bDeviceProtocol, bMaxPacketSize0, idVendor, idProduct, bcdDevice, iManufacturer,
                                   iProduct, iSerialNumber, bNumConfigurations)

/**
 * @brief USBDevice负责维护Descriptor的层级关系以及存储通过上层ArkTs拿到的值，内部类Descriptor负责封装C API的Descriptor
 *        但同时也会USBDevice类也会提供便利方法来获取USBDevice的Descriptor信息
 * @note
 * 为了不因为内部的Descriptor类RAII的性质而导致外部USBDevice类无法拷贝，USBDevice类应当是持有Descriptor的shared_ptr
 *       因为我们存在只存储的USBDevice信息，而不需要操作Descriptor的需求
 */
class USBDevice : public Serializable {
public:
    using Identifier = std::string; // 应用层采用 busNum-devAddress 唯一定位一个USBDevice
    class Descriptor {
    public:
        using sptr = std::shared_ptr<Descriptor>;
        using descriptor_type = UsbDeviceDescriptor;
        /**
         * @param deviceId
         * @return
         * @see OH_Usb_GetDeviceDescriptor
         */
        static Descriptor::sptr Get(std::uint64_t deviceId) { return std::make_shared<Descriptor>(deviceId); }

        Descriptor(std::uint64_t deviceId) : deviceId_(deviceId) {
            USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_GetDeviceDescriptor(deviceId, &descriptor_));
        }
        ~Descriptor() = default;

        std::uint64_t deviceId() const { return deviceId_; }
        const descriptor_type &descriptor() const { return descriptor_; }

    private:
        const std::uint64_t deviceId_;
        descriptor_type descriptor_;
    };

//    static USBDevice Get(std::uint64_t deviceId) { return USBDevice(Descriptor::Get(deviceId)); }

    ~USBDevice() = default;

    USBDevice(std::uint64_t deviceId) : deviceId_(deviceId) {}
    USBDevice(std::uint64_t deviceId, std::string name, std::string manufacturerName, std::string productName,
              std::string version, std::uint8_t devAddr, std::uint8_t busNum, std::int32_t vendorId,
              std::int32_t productId, std::int32_t clazz, std::int32_t subClass, std::int32_t protocol,
              std::vector<std::shared_ptr<USBConfig>> configs)
        : deviceId_(deviceId), name_(name), manufacturerName_(manufacturerName), productName_(productName),
          version_(version), devAddr_(devAddr), busNum_(busNum), vendorId_(vendorId), productId_(productId),
          clazz_(clazz), subClass_(subClass), protocol_(protocol), configs_(configs) {
        parsed_ = true;
    }
    USBDevice(std::uint64_t deviceId, json j)
        : deviceId_(deviceId), name_(j["name"]), manufacturerName_(j["manufacturerName"]),
          productName_(j["productName"]), version_(j["version"]), devAddr_(j["devAddress"]), busNum_(j["busNum"]),
          vendorId_(j["vendorId"]), productId_(j["productId"]), clazz_(j["clazz"]), subClass_(j["subClass"]),
          protocol_(j["protocol"]) {
        for (const auto &config : j["configs"]) {
            configs_.emplace_back(std::make_shared<USBConfig>(deviceId, config));
        }
        parsed_ = true;
    }
    explicit USBDevice(Descriptor::sptr descriptor) : deviceId_(descriptor->deviceId()) {
        for (std::uint8_t i = 0; i < descriptor->descriptor().bNumConfigurations; ++i) {
            configs_.emplace_back(std::make_shared<USBConfig>(descriptor->deviceId(), i));
        }
    }

    Identifier identifier() const {
        char buf[MAX_PATH] = {0};
        std::sprintf(buf, "%03d-%03d", busNum(), devAddr());
        return Identifier(buf);
    }
    Descriptor::sptr descriptor() const { return Descriptor::Get(deviceId()); }
    std::uint64_t deviceId() const { return deviceId_; }

    std::shared_ptr<USBConfig> configOf(std::uint32_t index) { return configs_.at(index); }
    const std::vector<std::shared_ptr<USBConfig>> &configs() { return configs_; }
    std::shared_ptr<USBConfig> configOf(std::uint32_t index) const { return configs_.at(index); }
    const std::vector<std::shared_ptr<USBConfig>> &configs() const { return configs_; }

    std::optional<json> toJson() const override {
        if (parsed_) {
            return std::nullopt;
        }
        json j;
        j["name"] = name();
        j["serial"] = mSerial();
        j["manufacturerName"] = manufacturerName();
        j["productName"] = productName();
        j["version"] = version();
        j["vendorId"] = vendorId();
        j["productId"] = productId();
        j["clazz"] = clazz();
        j["subClass"] = subClass();
        j["protocol"] = protocol();
        j["devAddress"] = devAddr();
        j["busNum"] = busNum();
        for (const auto &config : configs_) {
            j["configs"].emplace_back(config->toJson());
        }
        return j;
    }

    const std::string &name() const { return name_; }

    const std::string &manufacturerName() const { return manufacturerName_; }

    const std::string &productName() const { return productName_; }

    const std::string &version() const { return version_; }

    std::int32_t vendorId() const { return vendorId_; }

    std::int32_t productId() const { return productId_; }

    std::int32_t clazz() const { return clazz_; }

    std::int32_t subClass() const { return subClass_; }

    std::int32_t protocol() const { return protocol_; }

    std::int32_t configCount() const { return configs_.size(); }

    std::uint8_t devAddr() const { return devAddr_; }

    std::uint8_t busNum() const { return busNum_; }

    std::uint8_t descConfigCount() { return descConfigCount_; }

    std::uint8_t iManufacturer() { return iManufacturer_; }

    std::uint8_t iProduct() { return iProduct_; }

    std::uint8_t iSerialNumber() { return iSerialNumber_; }

    const std::string mSerial() const { return serial_; }

    std::uint16_t bcdUSB() { return bcdUSB_; }

    std::uint16_t bcdDevice() { return bcdDevice_; }

    std::uint8_t bMaxPacketSize0() { return bMaxPacketSize0_; }

    void setConfigs(const std::vector<std::shared_ptr<USBConfig>> &configs) { configs_ = configs; }

    void setDevAddr(std::uint8_t addr) { devAddr_ = addr; }

    void setBusNum(std::uint8_t num) { busNum_ = num; }

    void setName(const std::string &name) { name_ = name; }

    void setManufacturerName(const std::string &manufacturerName) { manufacturerName_ = manufacturerName; }

    void setProductName(const std::string &productName) { productName_ = productName; }

    void setVersion(const std::string &version) { version_ = version; }

    void setVendorId(std::int32_t vendorId) { vendorId_ = vendorId; }

    void setProductId(std::int32_t productId) { productId_ = productId; }

    void setClazz(std::int32_t deviceClass) { clazz_ = deviceClass; }

    void setSubclass(std::int32_t subClass) { subClass_ = subClass; }

    void setProtocol(std::int32_t protocol) { protocol_ = protocol; }

    void setDescConfigCount(std::uint8_t count) { descConfigCount_ = count; }

    void setiManufacturer(std::uint8_t manufacturer) { iManufacturer_ = manufacturer; }

    void setiProduct(std::uint8_t product) { iProduct_ = product; }

    void setiSerialNumber(std::uint8_t sn) { iSerialNumber_ = sn; }

    void setmSerial(std::string serial) { serial_ = serial; }

    void setbMaxPacketSize0(std::uint8_t maxSize) { bMaxPacketSize0_ = maxSize; }

    void setbcdUSB(std::uint16_t bcdUSB) { bcdUSB_ = bcdUSB; }

    void setbcdDevice(std::uint16_t bcdDevice) { bcdDevice_ = bcdDevice; }

private:
    const std::uint64_t deviceId_;
    std::vector<std::shared_ptr<USBConfig>> configs_;

    std::string name_;
    std::string manufacturerName_;
    std::string productName_;
    std::string version_;
    std::string serial_;
    std::uint8_t devAddr_ = UINT8_MAX;
    std::uint8_t busNum_ = UINT8_MAX;
    std::uint8_t descConfigCount_ = UINT8_MAX;

    std::uint8_t bMaxPacketSize0_ = UINT8_MAX;
    std::uint16_t bcdUSB_ = UINT16_MAX;
    std::uint16_t bcdDevice_ = UINT16_MAX;
    std::uint8_t iManufacturer_ = UINT8_MAX;
    std::uint8_t iProduct_ = UINT8_MAX;
    std::uint8_t iSerialNumber_ = UINT8_MAX;

    std::int32_t vendorId_ = INVALID_USB_INT_VALUE;
    std::int32_t productId_ = INVALID_USB_INT_VALUE;
    std::int32_t clazz_ = INVALID_USB_INT_VALUE;
    std::int32_t subClass_ = INVALID_USB_INT_VALUE;
    std::int32_t protocol_ = INVALID_USB_INT_VALUE;
};

constexpr std::size_t MAX_USB_DEVICE_NUM = 128;

} // namespace USB
} // namespace DDK
} // namespace OHOS

#endif // USBDEVICE_DEVICE_H
