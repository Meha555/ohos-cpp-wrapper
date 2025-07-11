#ifndef COMMONEV_EVENT_H
#define COMMONEV_EVENT_H

#include <BasicServicesKit/oh_commonevent.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <cstdint>
#include <vector>

#include "error.h"

namespace OHOS {
namespace common {
namespace event {

class Parameters {
public:
    using param_type = CommonEvent_Parameters;
    Parameters(const param_type *param) : param_(param) {}
#if OHOS_API_VERSION >= 18
    explicit Parameters() : param_(OH_CommonEvent_CreateParameters()) {}
    ~Parameters() {
        if (param_) {
            OH_CommonEvent_DestroyParameters(param_);
        }
    }

    void setInt(const char *key, int value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetIntToParameters(param_, key, value));
    }
    void setIntArray(const char *key, const std::vector<int> &value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(
            OH_CommonEvent_SetIntArrayToParameters(param_, key, value.data(), value.size()));
    }
    void setIntArray(const char *key, const int *value, std::size_t num) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetIntArrayToParameters(param_, key, value, num));
    }

    void setLong(const char *key, long value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetLongToParameters(param_, key, value));
    }
    void setLongArray(const char *key, const std::vector<long> &value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(
            OH_CommonEvent_SetLongArrayToParameters(param_, key, value.data(), value.size()));
    }
    void setLongArray(const char *key, const long *value, std::size_t num) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetLongArrayToParameters(param_, key, value, num));
    }

    void setBool(const char *key, bool value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetBoolToParameters(param_, key, value));
    }
    void setBoolArray(const char *key, const std::vector<std::uint8_t> &value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetBoolArrayToParameters(
            param_, key, reinterpret_cast<const bool *>(value.data()), value.size()));
    }
    void setBoolArray(const char *key, const bool *value, std::size_t num) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetBoolArrayToParameters(param_, key, value, num));
    }

    void setChar(const char *key, char value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetCharToParameters(param_, key, value));
    }
    void setCharArray(const char *key, const std::string &value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(
            OH_CommonEvent_SetCharArrayToParameters(param_, key, value.c_str(), value.size()));
    }
    void setCharArray(const char *key, const char *value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetCharArrayToParameters(param_, key, value, strlen(value)));
    }

    void setDouble(const char *key, double value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetDoubleToParameters(param_, key, value));
    }
    void setDoubleArray(const char *key, const std::vector<double> &value) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(
            OH_CommonEvent_SetDoubleArrayToParameters(param_, key, value.data(), value.size()));
    }
    void setDoubleArray(const char *key, const double *value, std::size_t num) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetDoubleArrayToParameters(param_, key, value, num));
    }
#endif

    bool hasKey(const char *key) const { return OH_CommonEvent_HasKeyInParameters(param_, key); }

    int getInt(const char *key, int defaultValue) const {
        return OH_CommonEvent_GetIntFromParameters(param_, key, defaultValue);
    }
    std::vector<int> getIntArray(const char *key) const {
        int *array;
        std::int32_t len = OH_CommonEvent_GetIntArrayFromParameters(param_, key, &array);
        return std::vector<int>(array, array + len);
    }

    long getLong(const char *key, long defaultValue) const {
        return OH_CommonEvent_GetLongFromParameters(param_, key, defaultValue);
    }
    std::vector<long> getLongArray(const char *key) const {
        long *array;
        std::int32_t len = OH_CommonEvent_GetLongArrayFromParameters(param_, key, &array);
        return std::vector<long>(array, array + len);
    }

    bool getBool(const char *key, bool defaultValue) const {
        return OH_CommonEvent_GetBoolFromParameters(param_, key, defaultValue);
    }
    std::vector<std::uint8_t> getBoolArray(const char *key) const {
        bool *array;
        std::int32_t len = OH_CommonEvent_GetBoolArrayFromParameters(param_, key, &array);
        return std::vector<std::uint8_t>(array, array + len);
    }

    char getChar(const char *key, char defaultValue) const {
        return OH_CommonEvent_GetCharFromParameters(param_, key, defaultValue);
    }
    std::string getCharArray(const char *key) const {
        char *array;
        std::int32_t len = OH_CommonEvent_GetCharArrayFromParameters(param_, key, &array);
        return std::string(array, array + len);
    }

    double getDouble(const char *key, double defaultValue) const {
        return OH_CommonEvent_GetDoubleFromParameters(param_, key, defaultValue);
    }
    std::vector<double> getDoubleArray(const char *key) const {
        double *array;
        std::int32_t len = OH_CommonEvent_GetDoubleArrayFromParameters(param_, key, &array);
        return std::vector<double>(array, array + len);
    }

    const param_type *parameters() const { return param_; }
    operator const param_type *() const { return parameters(); }

private:
    const param_type *param_;
};

class SubscribeInfo {
public:
    using info_type = CommonEvent_SubscribeInfo;

    SubscribeInfo(const SubscribeInfo &) = delete;
    SubscribeInfo &operator=(const SubscribeInfo &) = delete;
    SubscribeInfo(SubscribeInfo &&other) noexcept : info_(std::exchange(other.info_, nullptr)) {}
    SubscribeInfo &operator=(SubscribeInfo &&other) noexcept {
        if (this != &other) {
            OH_CommonEvent_DestroySubscribeInfo(info_);
            info_ = std::exchange(other.info_, nullptr);
        }
        return *this;
    }

    SubscribeInfo(std::initializer_list<const char *> events)
        : info_(OH_CommonEvent_CreateSubscribeInfo(const_cast<const char **>(std::data(events)), events.size())) {}
    SubscribeInfo(const char *events[], std::int32_t eventsNum)
        : info_(OH_CommonEvent_CreateSubscribeInfo(events, eventsNum)) {}
    ~SubscribeInfo() {
        if (info_) {
            OH_CommonEvent_DestroySubscribeInfo(info_);
        }
    }

    void setPublisherPermission(const char *permission) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetPublisherPermission(info_, permission));
    }

    void setPublisherBundleName(const char *bundleName) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetPublisherBundleName(info_, bundleName));
    }

    info_type *info() const { return info_; }
    operator info_type *() const { return info(); }

private:
    info_type *info_ = nullptr;
};

class RcvData {
public:
    using data_type = CommonEvent_RcvData;
    RcvData(const data_type *data) : data_(data) {}

    const char *event() const { return OH_CommonEvent_GetEventFromRcvData(data_); }

    std::int32_t code() const { return OH_CommonEvent_GetCodeFromRcvData(data_); }

    const char *dataStr() const { return OH_CommonEvent_GetDataStrFromRcvData(data_); }

    const char *bundleName() const { return OH_CommonEvent_GetBundleNameFromRcvData(data_); }

    Parameters parameters() const { return OH_CommonEvent_GetParametersFromRcvData(data_); }

    const data_type *data() const { return data_; }
    operator const data_type *() const { return data(); }

private:
    const data_type *data_;
};

class Subscriber {
public:
    using subcriber_type = CommonEvent_Subscriber;
    using ReceiveCallback = CommonEvent_ReceiveCallback;

    Subscriber(const SubscribeInfo *info, ReceiveCallback callback) {
        subscriber_ = OH_CommonEvent_CreateSubscriber(info->info(), callback);
    }

    Subscriber(const Subscriber &) = delete;
    Subscriber &operator=(const Subscriber &) = delete;
    Subscriber(Subscriber &&other) noexcept : subscriber_(std::exchange(other.subscriber_, nullptr)) {}
    Subscriber &operator=(Subscriber &&other) noexcept {
        if (this != &other) {
            OH_CommonEvent_DestroySubscriber(subscriber_);
            subscriber_ = std::exchange(other.subscriber_, nullptr);
        }
        return *this;
    }

    ~Subscriber() {
        if (subscriber_)
            OH_CommonEvent_DestroySubscriber(subscriber_);
    }

    void subscribe() const { COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_Subscribe(subscriber_)); }

    void unSubscribe() const { COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_UnSubscribe(subscriber_)); }

    subcriber_type *subscriber() const { return subscriber_; }
    operator subcriber_type *() const { return subscriber(); }

private:
    subcriber_type *subscriber_ = nullptr;
};

#if OHOS_API_VERSION >= 18

class PublishInfo {
    using info_type = CommonEvent_PublishInfo;

public:
    PublishInfo(bool ordered) : info_(OH_CommonEvent_CreatePublishInfo(ordered)) {}
    PublishInfo(const PublishInfo &) = delete;
    PublishInfo &operator=(const PublishInfo &) = delete;
    PublishInfo(PublishInfo &&other) noexcept : info_(std::exchange(other.info_, nullptr)) {}
    PublishInfo &operator=(PublishInfo &&other) noexcept {
        if (this != &other) {
            OH_CommonEvent_DestroyPublishInfo(info_);
            info_ = std::exchange(other.info_, nullptr);
        }
        return *this;
    }

    ~PublishInfo() {
        if (info_) {
            OH_CommonEvent_DestroyPublishInfo(info_);
        }
    }

    void setBunduleName(const char *bundleName) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetPublishInfoBundleName(info_, bundleName));
    }
    void setPermissions(const char *permissions[], std::int32_t permissionsNum) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetPublishInfoPermissions(info_, permissions, permissionsNum));
    }
    void setCode(std::int32_t code) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetPublishInfoCode(info_, code));
    }
    void setData(const char *data, std::size_t length) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetPublishInfoData(info_, data, length));
    }
    void setParameters(const Parameters &parameters) const {
        COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_SetPublishInfoParameters(
            info_, const_cast<Parameters::param_type *>(parameters.parameters())));
    }

    const info_type *info() const { return info_; }
    operator const info_type *() const { return info(); }

private:
    info_type *info_;
};

static inline void Publish(const char *event) { COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_Publish(event)); }

static inline void Publish(const char *event, const PublishInfo &info) {
    COMMON_CHECK_ERROR_INLINE_DEFAULT(OH_CommonEvent_PublishWithInfo(event, info.info()));
}

static inline bool IsOrderedCommonEvent(const Subscriber &subscriber) {
    return OH_CommonEvent_IsOrderedCommonEvent(subscriber.subscriber());
}
static inline bool IsOrderedCommonEvent(const Subscriber::subcriber_type *subscriber) {
    return OH_CommonEvent_IsOrderedCommonEvent(subscriber);
}

static inline bool FinishCommonEvent(Subscriber &subscriber) {
    return OH_CommonEvent_FinishCommonEvent(subscriber.subscriber());
}
static inline bool FinishCommonEvent(Subscriber::subcriber_type *subscriber) {
    return OH_CommonEvent_FinishCommonEvent(subscriber);
}

static inline bool GetAbortCommonEvent(const Subscriber &subscriber) {
    return OH_CommonEvent_GetAbortCommonEvent(subscriber.subscriber());
}
static inline bool GetAbortCommonEvent(const Subscriber::subcriber_type *subscriber) {
    return OH_CommonEvent_GetAbortCommonEvent(subscriber);
}

static inline bool AbortCommonEvent(Subscriber &subscriber) {
    return OH_CommonEvent_AbortCommonEvent(subscriber.subscriber());
}
static inline bool AbortCommonEvent(Subscriber::subcriber_type *subscriber) {
    return OH_CommonEvent_AbortCommonEvent(subscriber);
}

static inline bool ClearAbortCommonEvent(Subscriber &subscriber) {
    return OH_CommonEvent_ClearAbortCommonEvent(subscriber.subscriber());
}
static inline bool ClearAbortCommonEvent(Subscriber::subcriber_type *subscriber) {
    return OH_CommonEvent_ClearAbortCommonEvent(subscriber);
}

static inline std::int32_t GetCodeFromSubscriber(const Subscriber &subscriber) {
    return OH_CommonEvent_GetCodeFromSubscriber(subscriber.subscriber());
}
static inline std::int32_t GetCodeFromSubscriber(const Subscriber::subcriber_type *subscriber) {
    return OH_CommonEvent_GetCodeFromSubscriber(subscriber);
}
static inline bool SetCodeToSubscriber(Subscriber &subscriber, std::int32_t code) {
    return OH_CommonEvent_SetCodeToSubscriber(subscriber.subscriber(), code);
}
static inline bool SetCodeToSubscriber(Subscriber::subcriber_type *subscriber, std::int32_t code) {
    return OH_CommonEvent_SetCodeToSubscriber(subscriber, code);
}
static inline std::string GetDataFromSubscriber(const Subscriber &subscriber) {
    return OH_CommonEvent_GetDataFromSubscriber(subscriber.subscriber());
}
static inline std::string GetDataFromSubscriber(const Subscriber::subcriber_type *subscriber) {
    return OH_CommonEvent_GetDataFromSubscriber(subscriber);
}
static inline bool SetDataToSubscriber(Subscriber &subscriber, const std::string &data) {
    return OH_CommonEvent_SetDataToSubscriber(subscriber.subscriber(), data.c_str(), data.size());
}
static inline bool SetDataToSubscriber(Subscriber::subcriber_type *subscriber, const std::string &data) {
    return OH_CommonEvent_SetDataToSubscriber(subscriber, data.c_str(), data.size());
}
static inline bool SetDataToSubscriber(Subscriber &subscriber, const char *data, std::size_t length) {
    return OH_CommonEvent_SetDataToSubscriber(subscriber.subscriber(), data, length);
}
static inline bool SetDataToSubscriber(Subscriber::subcriber_type *subscriber, const char *data, std::size_t length) {
    return OH_CommonEvent_SetDataToSubscriber(subscriber, data, length);
}

#endif

} // namespace event
} // namespace common
} // namespace OHOS


#endif // COMMONEV_EVENT_H
