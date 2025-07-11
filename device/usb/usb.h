#ifndef USBDEVICE_USB_H
#define USBDEVICE_USB_H

#include "common.h"
#include "device.h"
#include "event.h"

namespace OHOS {
namespace DDK {
namespace USB {

// class TransferCallbackInfo {
// public:
//     int32_t status;
//     int32_t actualLength;
// };
//
// using TransferCallback = std::function<void(const TransferCallbackInfo &,
//     const std::vector<UsbIsoPacketDescriptor> &, uint64_t)>;
//
// class USBDevicePipe {
// public:
//     explicit USBDevicePipe() = default;
//     USBDevicePipe(uint8_t busNum, uint8_t devAddr) : busNum_(busNum), devAddr_(devAddr) {}
//     ~USBDevicePipe() {}
//     int32_t ClaimInterface(const USBInterface &interface, bool force);
//     int32_t ReleaseInterface(const USBInterface &interface);
//     int32_t BulkTransfer(const USBEndpoint &endpoint, std::vector<uint8_t> &bufferData, int32_t timeOut);
//    
//     int32_t UsbSubmitTransfer(USBTransferInfo &info, const TransferCallback &cb,
//         sptr<Ashmem> &ashmem);
//     int32_t UsbCancelTransfer(int32_t &endpoint);
//    
//     int32_t ControlTransfer(const HDI::Usb::V1_0::UsbCtrlTransfer &ctrl, std::vector<uint8_t> &bufferData);
//     int32_t UsbControlTransfer(
//         const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
//     int32_t SetConfiguration(const USBConfig &config);
//     int32_t SetInterface(const USBInterface &interface);
//     int32_t Close();
//
//     void setBusNum(uint8_t busNum);
//     void setDevAddr(uint8_t devAddr);
//     uint8_t busNum() const;
//     uint8_t devAddr() const;
// private:
//     uint8_t busNum_ = UINT8_MAX;
//     uint8_t devAddr_ = UINT8_MAX;
// };

/**
 * @brief 监听USB插拔事件
 * @note singleton
 */
class USBEventListener {
    // 禁用拷贝构造后，移动自动也禁用了
    USBEventListener(const USBEventListener &) = delete;
    USBEventListener &operator=(const USBEventListener &) = delete;

public:
    using NotifyCallback = std::function<void(const common::event::RcvData &, void *userData)>;

    static USBEventListener &Instance() {
        static USBEventListener instance;
        return instance;
    }

    USBEventListener &onAttach(NotifyCallback cb, void *userData = nullptr) {
        onAttach_ = {cb, userData};
        return *this;
    }
    USBEventListener &onDetach(NotifyCallback cb, void *userData = nullptr) {
        onDetach_ = {cb, userData};
        return *this;
    }

    void start() {
        if (subscriber_) {
            return;
        }
        std::vector<const char *> events;
        if (onAttach_.has_value()) {
            events.emplace_back(COMMON_EVENT_USB_DEVICE_ATTACHED);
        }
        if (onDetach_.has_value()) {
            events.emplace_back(COMMON_EVENT_USB_DEVICE_DETACHED);
        }
        common::event::SubscribeInfo info(events.data(), events.size());
        subscriber_.reset(new common::event::Subscriber(&info, OnEvent));
        subscriber_->subscribe();
        subscribed_ = true;
    }

    void reset() {
        if (!subscriber_) {
            return;
        }
        subscriber_->unSubscribe();
        subscribed_ = false;
    }

private:
    explicit USBEventListener() : subscriber_(nullptr) {}

    static void OnEvent(const CommonEvent_RcvData *data) {
        const common::event::RcvData rcvData(data);
        const auto &lis = Instance();
        if (std::strcmp(rcvData.event(), COMMON_EVENT_USB_DEVICE_ATTACHED) == 0) {
            (*lis.onAttach_)(rcvData);
        } else if (std::strcmp(rcvData.event(), COMMON_EVENT_USB_DEVICE_DETACHED) == 0) {
            (*lis.onDetach_)(rcvData);
        }
    }

    struct Notifyer {
        NotifyCallback notify;
        void *userData{nullptr};
        void operator()(const common::event::RcvData &data) const {
            notify(data, userData);
        }
    };
    std::atomic_bool subscribed_ = false;
    std::unique_ptr<common::event::Subscriber> subscriber_;
    std::optional<Notifyer> onAttach_;
    std::optional<Notifyer> onDetach_;
};

/**
 * @brief 初始化DDK环境
 * @note singleton（单例必然不要公开拷贝和移动）
 */
class USBHostManager {
    // 禁用拷贝构造后，移动自动也禁用了
    USBHostManager &operator=(const USBHostManager &) = delete;
    USBHostManager(const USBHostManager &) = delete;

public:
    explicit USBHostManager() {
        grab();
#if OHOS_API_VERSION >= 18
        enumerate();
#endif
    }
    ~USBHostManager() { ungrab(); }

    static USBHostManager &Instance() {
        static USBHostManager instance;
        return instance;
    }

    void grab() const { USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_Init()); }
    void ungrab() const { OH_Usb_Release(); }

#if OHOS_API_VERSION >= 18
    void enumerate() {
        Usb_DeviceArray deviceArray{};
        deviceArray.deviceIds = new uint64_t[MAX_USB_DEVICE_NUM];
        USB_CHECK_ERROR_INLINE_DEFAULT(OH_Usb_GetDevices(&deviceArray));
        devices_.reserve(deviceArray.num);
        for (std::size_t i = 0; i < deviceArray.num; i++) {
            auto device = USBDevice::Get(deviceArray.deviceIds[i]);
            devices_.emplace(device.identifier(), device);
        }
    }
#endif
    const std::unordered_map<std::uint64_t, USBDevice> &devices() const { return devices_; }

    void addDevice(std::uint64_t deviceId, USBDevice &&device) { devices_.emplace(deviceId, device); }

    USBDevice deviceOf(std::uint64_t deviceId) const { return devices_.at(deviceId); }

    std::optional<USBDevice> findDevice(USBDevice::Identifier identifier) const {
        for (auto &device : devices_) {
            if (device.second.identifier() == identifier) {
                return device.second;
            }
        }
        return std::nullopt;
    }

private:
    std::unordered_map<std::uint64_t, USBDevice> devices_;
};

} // namespace USB
} // namespace DDK
} // namespace OHOS

#endif // USBDEVICE_USB_H
