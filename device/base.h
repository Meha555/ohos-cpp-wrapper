#ifndef USBDEVICE_BASE_H
#define USBDEVICE_BASE_H

#include <ddk/ddk_api.h>
#include <ddk/ddk_types.h>
#include <memory>
#include <string>
#include <system_error>

#include "nlohmann/json.hpp"

namespace std {
template <> struct is_error_code_enum<DDK_RetCode> : public true_type {};
} // namespace std

namespace OHOS {
namespace DDK {

template <typename T> using sptr = std::shared_ptr<T>;

#define DDK_CHECK_ERROR(exp, message)                                                                                  \
    if (auto __code = (exp); __code != DDK_SUCCESS)                                                                    \
    throw std::system_error(__code, DDKErrorCategory::Instance(), (message))

#define DDK_CHECK_ERROR_INLINE_DEFAULT(exp) DDK_CHECK_ERROR(exp, #exp)

class DDKErrorCategory : public std::error_category {
public:
    const char *name() const noexcept override { return "DDK Error"; }
    std::string message(int ec) const override {
        switch (ec) {
        case DDK_SUCCESS:
            return "Operation success";
        case DDK_FAILURE:
            return "Operation failed";
        case DDK_INVALID_PARAMETER:
            return "Invalid parameter";
        case DDK_INVALID_OPERATION:
            return "Invalid operation";
        case DDK_NULL_PTR:
            return "Null pointer exception";
        default:
            return "Unknown error";
        }
    }

    static const DDKErrorCategory &Instance() {
        static DDKErrorCategory instance;
        return instance;
    }

private:
    DDKErrorCategory() = default;
};

class MemMap {
public:
    virtual ~MemMap() = default;

    virtual void memMap(const uint8_t ashmemMapType) = 0;
    virtual void unMemMap() = 0;

    virtual std::int32_t fd() const = 0;
    virtual std::uint32_t size() const = 0;
    virtual std::uint32_t offset() const = 0;
    virtual std::uint32_t bufferLength() const = 0;
    virtual std::uint32_t transferredLength() const = 0;
};

class Ashmem : public MemMap {
public:
    Ashmem(const std::uint8_t *name, std::uint32_t size) {
        DDK_CHECK_ERROR_INLINE_DEFAULT(OH_DDK_CreateAshmem(name, size, &ashmem_));
    }
    ~Ashmem() {
        if (ashmem_) {
            DDK_CHECK_ERROR_INLINE_DEFAULT(OH_DDK_DestroyAshmem(ashmem_));
        }
    }

    void memMap(const uint8_t ashmemMapType) override {
        DDK_CHECK_ERROR_INLINE_DEFAULT(OH_DDK_MapAshmem(ashmem_, ashmemMapType));
    }
    void unMemMap() override { DDK_CHECK_ERROR_INLINE_DEFAULT(OH_DDK_UnmapAshmem(ashmem_)); }

    std::int32_t fd() const override { return ashmem_ ? ashmem_->ashmemFd : -1; }
    const uint8_t *address() const { return ashmem_ ? ashmem_->address : nullptr; }
    std::uint32_t size() const override { return ashmem_ ? ashmem_->size : 0; }
    std::uint32_t offset() const override { return ashmem_ ? ashmem_->offset : 0; }
    std::uint32_t bufferLength() const override { return ashmem_ ? ashmem_->bufferLength : 0; }
    std::uint32_t transferredLength() const override { return ashmem_ ? ashmem_->transferredLength : 0; }

private:
    struct DDK_Ashmem *ashmem_ = nullptr;
};

// 提取deviceManager.queryDevices()获取到的deviceId的前32位作为C_API的deviceId。
inline std::uint64_t JsDeviceIdToNative(std::uint64_t deviceId) {
    auto busNum = static_cast<std::uint32_t>(deviceId >> 48);
    auto devNum = static_cast<std::uint32_t>((deviceId & 0x0000FFFF00000000) >> 32);
    return (((static_cast<std::uint64_t>(busNum)) << 32) | devNum);
//    return deviceId & 0xFFFFFFFF00000000;
}

inline std::uint32_t JsDeviceIdToBusNum(std::uint64_t deviceId) { return static_cast<std::uint32_t>(deviceId >> 48); }

inline std::uint32_t JsDeviceIdToDevNum(std::uint64_t deviceId) {
    return static_cast<std::uint32_t>((deviceId & 0x0000FFFF00000000) >> 32);
}

class Serializable {
public:
    using json = nlohmann::ordered_json;

    virtual ~Serializable() = default;
    virtual std::optional<json> toJson() const = 0;

protected:
    bool parsed_ = false; // 是否已经更新过需要序列化的字段
};

} // namespace DDK
} // namespace OHOS

#endif // USBDEVICE_BASE_H
