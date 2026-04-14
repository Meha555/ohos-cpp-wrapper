#pragma once

#include <hiappevent/hiappevent.h>
#include <system_error>

namespace std {
template <> struct is_error_code_enum<HiAppEvent_ErrorCode> : public std::true_type {};
} // namespace std

namespace OHOS {
namespace appevent {

#define APPEVENT_CHECK_ERROR(exp, message)                                                                             \
    if (auto __code = (exp); __code != HIAPPEVENT_SUCCESS)                                                             \
    throw std::system_error(__code, AppEventErrCategory::Instance(), (message))

#define APPEVENT_CHECK_ERROR_INLINE_DEFAULT(exp) APPEVENT_CHECK_ERROR(exp, #exp)

class AppEventErrCategory : public std::error_category {
public:
    const char *name() const noexcept override { return "HiAppEvent Error"; }
    std::string message(int ec) const override {
        switch (ec) {
        case HIAPPEVENT_SUCCESS:
            return "HIAPPEVENT_SUCCESS";
        case HIAPPEVENT_INVALID_PARAM_VALUE_LENGTH:
            return "HIAPPEVENT_INVALID_PARAM_VALUE_LENGTH";
        case HIAPPEVENT_PROCESSOR_IS_NULL:
            return "HIAPPEVENT_PROCESSOR_IS_NULL";
        case HIAPPEVENT_PROCESSOR_NOT_FOUND:
            return "HIAPPEVENT_PROCESSOR_NOT_FOUND";
        case HIAPPEVENT_INVALID_PARAM_VALUE:
            return "HIAPPEVENT_INVALID_PARAM_VALUE";
        case HIAPPEVENT_EVENT_CONFIG_IS_NULL:
            return "HIAPPEVENT_EVENT_CONFIG_IS_NULL";
        case HIAPPEVENT_OPERATE_FAILED:
            return "HIAPPEVENT_OPERATE_FAILED";
        case HIAPPEVENT_INVALID_UID:
            return "HIAPPEVENT_INVALID_UID";
        default:
            return "unknown error";
        }
    }

    static bool IsError(int ec) {
        return ec != HIAPPEVENT_INVALID_PARAM_VALUE_LENGTH && ec != HIAPPEVENT_PROCESSOR_IS_NULL ||
               ec != HIAPPEVENT_PROCESSOR_NOT_FOUND || ec != HIAPPEVENT_INVALID_PARAM_VALUE ||
               ec != HIAPPEVENT_EVENT_CONFIG_IS_NULL || ec != HIAPPEVENT_OPERATE_FAILED || ec != HIAPPEVENT_INVALID_UID;
    }

    static const AppEventErrCategory &Instance() {
        static AppEventErrCategory instance;
        return instance;
    }

private:
    AppEventErrCategory() = default;
};

} // namespace appevent
}