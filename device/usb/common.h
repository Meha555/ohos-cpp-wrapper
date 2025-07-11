#ifndef USBDEVICE_COMMON_H
#define USBDEVICE_COMMON_H

#include <cstdint>
#include <system_error>

#include <usb/usb_ddk_api.h>
#include <usb/usb_ddk_types.h>

#include "base.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace DDK {
namespace USB {

#define USB_CHECK_ERROR(exp, message)                                                                                  \
    if (auto __code = (exp); __code != USB_DDK_SUCCESS)                                                                \
    throw std::system_error(__code, USBErrorCategory::Instance(), (message))

#define USB_CHECK_ERROR_INLINE_DEFAULT(exp) USB_CHECK_ERROR(exp, #exp)

/**
 * Invalid usb int value
 */
constexpr std::int32_t INVALID_USB_INT_VALUE = -1;

/**
 * Bitmask used for extracting the USBEndpoint type from it's address
 */
constexpr std::uint32_t USB_ENDPOINT_XFERTYPE_MASK = 0x03;

/**
 * Control USBEndpoint type
 */
constexpr std::int32_t USB_ENDPOINT_XFER_CONTROL = 0;

/**
 * Isochronous USBEndpoint type
 */
constexpr std::int32_t USB_ENDPOINT_XFER_ISOC = 1;

/**
 * Bulk USBEndpoint type
 */
constexpr std::int32_t USB_ENDPOINT_XFER_BULK = 2;

/**
 * Interrupt USBEndpoint type
 */
constexpr std::int32_t USB_ENDPOINT_XFER_INT = 3;

/**
 * Bitmask used for extracting the USBEndpoint number from it's address
 */
constexpr std::uint32_t USB_ENDPOINT_NUMBER_MASK = 0x0f;

/**
 * Bitmask used for extracting the USBEndpoint direction from it's address
 */
constexpr std::uint32_t USB_ENDPOINT_DIR_MASK = 0x80;

/**
 * Used to signify direction of data for USBEndpoint is OUT, host to device
 */
constexpr std::uint32_t USB_ENDPOINT_DIR_OUT = 0;

/**
 * Used to signify direction of data for USBEndpoint is IN, device to host
 */
constexpr std::uint32_t USB_ENDPOINT_DIR_IN = 0x80;

/**
 * Bitmask for self power in the USBConfig
 */
constexpr std::uint32_t USB_CFG_SELF_POWERED = 0x80;

/**
 * Bitmask for remote wakeup in the USBConfig
 */
constexpr std::uint32_t USB_CFG_REMOTE_WAKEUP = 0x20;

enum class USBErrCode {
#if OHOS_API_VERSION >= 18
    USB_DDK_SUCCESS = 0,
    USB_DDK_FAILED = -1,
    USB_DDK_NO_PERM = 201,
    USB_DDK_INVALID_PARAMETER = 401,
    USB_DDK_MEMORY_ERROR = 27400001,
    USB_DDK_NULL_PTR = -5,
    USB_DDK_DEVICE_BUSY = -6,
    USB_DDK_INVALID_OPERATION = 27400002,
    USB_DDK_IO_FAILED = 27400003,
    USB_DDK_TIMEOUT = 27400004,
#else
    USB_DDK_SUCCESS = 0,
    USB_DDK_FAILED = -1,
    USB_DDK_INVALID_PARAMETER = -2,
    USB_DDK_MEMORY_ERROR = -3,
    USB_DDK_INVALID_OPERATION = -4,
    USB_DDK_NULL_PTR = -5,
    USB_DDK_DEVICE_BUSY = -6,
    USB_DDK_TIMEOUT = -7
#endif
};

class USBErrorCategory : public std::error_category {
public:
    const char *name() const noexcept override { return "USB Error"; }
    std::string message(int ec) const override {
        switch (static_cast<USBErrCode>(ec)) {
#if OHOS_API_VERSION >= 18
        case USBErrCode::USB_DDK_SUCCESS:
            return "The operation is successful";
        case USBErrCode::USB_DDK_FAILED:
            return "The operation failed";
        case USBErrCode::USB_DDK_NO_PERM:
            return "No permission";
        case USBErrCode::USB_DDK_INVALID_PARAMETER:
            return "Invalid parameter";
        case USBErrCode::USB_DDK_MEMORY_ERROR:
            return "Memory-related error";
        case USBErrCode::USB_DDK_INVALID_OPERATION:
            return "Invalid operation";
        case USBErrCode::USB_DDK_NULL_PTR:
            return "Null pointer exception";
        case USBErrCode::USB_DDK_DEVICE_BUSY:
            return "Device busy";
        case USBErrCode::USB_DDK_IO_FAILED:
            return "Device I/O operation failed";
        case USBErrCode::USB_DDK_TIMEOUT:
            return "Transmission timeout";
#else
        case USBErrCode::USB_DDK_SUCCESS:
            return "The operation is successful";
        case USBErrCode::USB_DDK_FAILED:
            return "The operation failed";
        case USBErrCode::USB_DDK_INVALID_PARAMETER:
            return "Invalid parameter";
        case USBErrCode::USB_DDK_MEMORY_ERROR:
            return "Memory-related error";
        case USBErrCode::USB_DDK_INVALID_OPERATION:
            return "Invalid operation";
        case USBErrCode::USB_DDK_NULL_PTR:
            return "Null pointer exception";
        case USBErrCode::USB_DDK_DEVICE_BUSY:
            return "Device busy";
        case USBErrCode::USB_DDK_TIMEOUT:
            return "Transmission timeout";
#endif
        default:
            return "Unknown error";
        }
    }

    static const USBErrorCategory &Instance() {
        static const USBErrorCategory category{};
        return category;
    }

protected:
    USBErrorCategory() = default;
};

// TODO 改异常为返回错误码，或者只在析构函数中返回错误码

constexpr int MAX_PATH = 260;

} // namespace USB
} // namespace DDK
} // namespace OHOS

namespace std {
template <> struct is_error_code_enum<OHOS::DDK::USB::USBErrCode> : public true_type {};
} // namespace std


#endif // USBDEVICE_COMMON_H
