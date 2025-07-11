#ifndef COMMONEV_ERROR_H
#define COMMONEV_ERROR_H

#include <BasicServicesKit/oh_commonevent.h>
#include <system_error>

namespace std {
template <> struct is_error_code_enum<CommonEvent_ErrCode> : public true_type {};
} // namespace std

namespace OHOS {
namespace common {
namespace event {

#define COMMON_CHECK_ERROR(exp, message)                                                                               \
    if (auto __code = (exp); __code != COMMONEVENT_ERR_OK)                                                             \
    throw std::system_error(__code, CommonEventErrCategory::Instance(), (message))

#define COMMON_CHECK_ERROR_INLINE_DEFAULT(exp) COMMON_CHECK_ERROR(exp, #exp)

class CommonEventErrCategory : public std::error_category {
public:
    const char *name() const noexcept override { return "CommonEvent Error"; }
    std::string message(int ec) const override {
        switch (ec) {
        case COMMONEVENT_ERR_OK:
            return "Execution successful";
        case COMMONEVENT_ERR_PERMISSION_ERROR:
            return "permission verification failed";
        case COMMONEVENT_ERR_INVALID_PARAMETER:
            return "invalid input parameter";
        case COMMONEVENT_ERR_NOT_SYSTEM_SERVICE:
            return "this app cannot send system common events";
        case COMMONEVENT_ERR_SENDING_REQUEST_FAILED:
            return "IPC request failed to send";
        case COMMONEVENT_ERR_INIT_UNDONE:
            return "Common event service not init";
        case COMMONEVENT_ERR_OBTAIN_SYSTEM_PARAMS:
            return "obtain system parameters failed";
        case COMMONEVENT_ERR_SUBSCRIBER_NUM_EXCEEDED:
            return "The subscriber number exceed system specification";
        case COMMONEVENT_ERR_ALLOC_MEMORY_FAILED:
            return "A memory allocation error occurs";
        default:
            return "unknown error";
        }
    }

    static CommonEventErrCategory &Instance() {
        static CommonEventErrCategory instance;
        return instance;
    }

private:
    CommonEventErrCategory() = default;
};

} // namespace event
} // namespace common
} // namespace OHOS

#endif // COMMONEV_ERROR_H
