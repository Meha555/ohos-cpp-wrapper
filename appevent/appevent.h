#pragma once

#include <hiappevent/hiappevent.h>
#include <vector>

#include "error.h"

namespace OHOS {
namespace appevent {

using AppEventInfo = HiAppEvent_AppEventInfo;
using AppEventGroup = HiAppEvent_AppEventGroup;

enum EventTypes : uint8_t { BEHAVIOR = 0x08, SECURITY = 0x04, STATISTIC = 0x02, FAULT = 0x01, ALL = 0xff };

template <typename W> class Watcher {
public:
    using watcher_type = HiAppEvent_Watcher;
    using TakeCallback = OH_HiAppEvent_OnTake;

    explicit Watcher(const char *name) : watcher_(OH_HiAppEvent_CreateWatcher(name)) {}

    ~Watcher() {
        if (watcher_) {
            OH_HiAppEvent_DestroyWatcher(watcher_);
        }
    }

    Watcher(const Watcher &) = delete;
    Watcher &operator=(const Watcher &) = delete;
    Watcher(Watcher &&other) noexcept : watcher_(other.watcher_) { watcher_ = nullptr; }
    Watcher operator=(Watcher &&other) noexcept {
        if (this != &other) {
            OH_HiAppEvent_DestroyWatcher(watcher_);
            watcher_ = other.watcher_;
            other.watcher_ = nullptr;
        }
        return *this;
    }

    operator watcher_type *() { return watcher_; }

    W &SetAppEventFilter(const char *domain, EventTypes eventTypes, const std::vector<const char *> &names) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(
            OH_HiAppEvent_SetAppEventFilter(watcher_, domain, eventTypes, names.data(), names.size()));
        return *static_cast<W *>(this);
    }

    W &SetAppEventFilter(const char *domain, EventTypes eventTypes, const std::initializer_list<const char *> &names) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(
            OH_HiAppEvent_SetAppEventFilter(watcher_, domain, eventTypes, names.begin(), names.size()));
        return *static_cast<W *>(this);
    }

    W &OnTake(uint32_t eventNum, TakeCallback callback) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_TakeWatcherData(watcher_, eventNum, callback));
        return *static_cast<W *>(this);
    }

protected:
    watcher_type *watcher_ = nullptr;
};

class ReceiveWatcher : public Watcher<ReceiveWatcher> {
public:
    using ReceiveCallback = OH_HiAppEvent_OnReceive;

    explicit ReceiveWatcher(const char *name) : Watcher(name) {}

    ReceiveWatcher &OnReceive(ReceiveCallback callback) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetWatcherOnReceive(watcher_, callback));
        return *this;
    }
};

class TriggerWatcher : public Watcher<TriggerWatcher> {
public:
    using TriggerCallback = OH_HiAppEvent_OnTrigger;

    explicit TriggerWatcher(const char *name) : Watcher(name) {}

    TriggerWatcher &OnTrigger(TriggerCallback callback) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetWatcherOnTrigger(watcher_, callback));
        return *this;
    }

    TriggerWatcher &OnCondition(int row, int size, int timeout) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetTriggerCondition(watcher_, row, size, timeout));
        return *this;
    }
};

class ParamList {
public:
    using list_type = ::ParamList;

    ParamList() : params_(OH_HiAppEvent_CreateParamList()) {}

    ~ParamList() { OH_HiAppEvent_DestroyParamList(params_); }

    ParamList(const ParamList &) = delete;
    ParamList &operator=(const ParamList &) = delete;
    ParamList(ParamList &&other) noexcept : params_(other.params_) { other.params_ = nullptr; }
    ParamList &operator=(ParamList &&other) noexcept {
        if (this != &other) {
            OH_HiAppEvent_DestroyParamList(params_);
            params_ = other.params_;
            other.params_ = nullptr;
        }
        return *this;
    }

    operator list_type() { return params_; }

    ParamList &AddBool(const char *name, bool value) {
        params_ = OH_HiAppEvent_AddBoolParam(params_, name, value);
        return *this;
    }

    ParamList &AddBoolArray(const char *name, const bool *booleans, int arrSize) {
        params_ = OH_HiAppEvent_AddBoolArrayParam(params_, name, booleans, arrSize);
        return *this;
    }

    ParamList &AddInt8(const char *name, int8_t value) {
        params_ = OH_HiAppEvent_AddInt8Param(params_, name, value);
        return *this;
    }

    ParamList &AddInt8Array(const char *name, const int8_t *int8s, int arrSize) {
        params_ = OH_HiAppEvent_AddInt8ArrayParam(params_, name, int8s, arrSize);
        return *this;
    }

    ParamList &AddInt16(const char *name, int16_t value) {
        params_ = OH_HiAppEvent_AddInt16Param(params_, name, value);
        return *this;
    }

    ParamList &AddInt16Array(const char *name, const int16_t *int16s, int arrSize) {
        params_ = OH_HiAppEvent_AddInt16ArrayParam(params_, name, int16s, arrSize);
        return *this;
    }

    ParamList &AddInt32(const char *name, int32_t value) {
        params_ = OH_HiAppEvent_AddInt32Param(params_, name, value);
        return *this;
    }

    ParamList &AddInt32Array(const char *name, const int32_t *int32s, int arrSize) {
        params_ = OH_HiAppEvent_AddInt32ArrayParam(params_, name, int32s, arrSize);
        return *this;
    }

    ParamList &AddInt64(const char *name, int64_t value) {
        params_ = OH_HiAppEvent_AddInt64Param(params_, name, value);
        return *this;
    }

    ParamList &AddInt64Array(const char *name, const int64_t *int64s, int arrSize) {
        params_ = OH_HiAppEvent_AddInt64ArrayParam(params_, name, int64s, arrSize);
        return *this;
    }

    ParamList &AddFloat(const char *name, float value) {
        params_ = OH_HiAppEvent_AddFloatParam(params_, name, value);
        return *this;
    }

    ParamList &AddFloatArray(const char *name, const float *floats, int arrSize) {
        params_ = OH_HiAppEvent_AddFloatArrayParam(params_, name, floats, arrSize);
        return *this;
    }

    ParamList &AddDouble(const char *name, double value) {
        params_ = OH_HiAppEvent_AddDoubleParam(params_, name, value);
        return *this;
    }

    ParamList &AddDoubleArray(const char *name, const double *doubles, int arrSize) {
        params_ = OH_HiAppEvent_AddDoubleArrayParam(params_, name, doubles, arrSize);
        return *this;
    }

    ParamList &AddString(const char *name, const char *value) {
        params_ = OH_HiAppEvent_AddStringParam(params_, name, value);
        return *this;
    }

    ParamList &AddStringArray(const char *name, const char **strings, int arrSize) {
        params_ = OH_HiAppEvent_AddStringArrayParam(params_, name, strings, arrSize);
        return *this;
    }

private:
    list_type params_ = nullptr;
};

class Processor {
public:
    using processor_type = ::HiAppEvent_Processor;

    Processor(const char *name) : processor_(OH_HiAppEvent_CreateProcessor(name)) {}

    ~Processor() { OH_HiAppEvent_DestroyProcessor(processor_); }

    Processor(const Processor &) = delete;
    Processor &operator=(const Processor &) = delete;
    Processor(Processor &&other) noexcept : processor_(other.processor_) { other.processor_ = nullptr; }
    Processor &operator=(Processor &&other) noexcept {
        if (this != &other) {
            OH_HiAppEvent_DestroyProcessor(processor_);
            processor_ = other.processor_;
            other.processor_ = nullptr;
        }
        return *this;
    }

    operator processor_type *() { return processor_; }

    Processor &SetReportRoute(const char *appId, const char *routeInfo) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetReportRoute(processor_, appId, routeInfo));
        return *this;
    }

    Processor &SetReportPolicy(int periodReport, int batchReport, bool onStartReport, bool onBackgroundReport) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(
            OH_HiAppEvent_SetReportPolicy(processor_, periodReport, batchReport, onStartReport, onBackgroundReport));
        return *this;
    }

    Processor &SetReportEvent(const char *domain, const char *name, bool isRealTime) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetReportEvent(processor_, domain, name, isRealTime));
        return *this;
    }

    Processor &SetCustomConfig(const char *key, const char *value) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetCustomConfig(processor_, key, value));
        return *this;
    }

    Processor &SetConfigId(int configId) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetConfigId(processor_, configId));
        return *this;
    }

    Processor &SetReportUserId(const char **userIdNames, int size) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetReportUserId(processor_, userIdNames, size));
        return *this;
    }

    Processor &SetReportUserProperty(const char **userPropertyNames, int size) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetReportUserProperty(processor_, userPropertyNames, size));
        return *this;
    }

private:
    processor_type *processor_ = nullptr;
};

class Config {
public:
    using config_type = ::HiAppEvent_Config;

    Config() : config_(OH_HiAppEvent_CreateConfig()) {}

    ~Config() { OH_HiAppEvent_DestroyConfig(config_); }

    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;
    Config(Config &&other) noexcept : config_(other.config_) { other.config_ = nullptr; }
    Config &operator=(Config &&other) noexcept {
        if (this != &other) {
            OH_HiAppEvent_DestroyConfig(config_);
            config_ = other.config_;
            other.config_ = nullptr;
        }
        return *this;
    }

    operator config_type *() { return config_; }

    Config &SetItem(const char *key, const char *value) {
        APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetConfigItem(config_, key, value));
        return *this;
    }

private:
    config_type *config_ = nullptr;
};

inline void AddWatcher(HiAppEvent_Watcher *watcher) {
    APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_AddWatcher(watcher));
}

inline void RemoveWatcher(HiAppEvent_Watcher *watcher) {
    APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_RemoveWatcher(watcher));
}


inline void ClearData() { OH_HiAppEvent_ClearData(); }

inline int64_t AddProcessor(Processor::processor_type *processor) {
    int64_t ret = OH_HiAppEvent_AddProcessor(processor);
    if (!AppEventErrCategory::IsError(ret)) {
        return ret;
    }
    throw std::system_error(ret, AppEventErrCategory::Instance(), "OH_HiAppEvent_AddProcessor");
}

inline void RemoveProcessor(int64_t processorId) {
    APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_RemoveProcessor(processorId));
}

inline void SetEventConfig(const char *name, Config::config_type *config) {
    APPEVENT_CHECK_ERROR_INLINE_DEFAULT(OH_HiAppEvent_SetEventConfig(name, config));
}

} // namespace appevent
}