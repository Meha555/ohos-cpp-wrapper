#ifndef LOG_OH_LOG_STREAM_H_
#define LOG_OH_LOG_STREAM_H_

#ifndef LOG_DOMAIN
# warning "LOG_DOMAIN is not defined, no log will be output"
#endif
#ifndef LOG_TAG
# warning "LOG_TAG is not defined, no log will be output"
#endif

#include <ostream>
#include <streambuf>
#include <array>
#include <hilog/log.h>

namespace OHOS {

// 自定义流缓冲区，将数据通过OH_Log_Print输出
template<size_t N>
class OHLogStreamBuf : public std::streambuf {
public:
    // 构造函数，可指定日志标签和级别
    explicit OHLogStreamBuf(LogLevel level)
        : level_(level) {
        // 设置输出缓冲区
        setp(buffer_.begin(), buffer_.end() - 1);  // 预留一个位置给终止符
    }

protected:
    // 当缓冲区满或遇到特定字符时调用
    int_type overflow(int_type c) override {
        if (c != EOF) {
            *pptr() = static_cast<char>(c);
            pbump(1);
        }
        // 刷新缓冲区
        if (sync() == -1) {
            return EOF;
        }
        return c;
    }

    // 同步缓冲区，将数据输出
    int sync() override {
        std::streamsize len = pptr() - pbase();
        if (len > 0) {
            // 终止字符串
            buffer_[len] = '\0';

            (void)OH_LOG_Print(LOG_APP, level_, LOG_DOMAIN, LOG_TAG, "%{public}s", pbase());

            // 重置缓冲区指针
            setp(buffer_.begin(), buffer_.end() - 1);
        } else {
            return -1;
        }
        return 0;
    }

private:
    std::array<char, N> buffer_;  // 缓冲区，可根据需要调整大小
    LogLevel level_;
};

extern std::ostream cout;
extern std::ostream cerr;
extern std::ostream clog;

} // namespace OHOS

#endif  // LOG_OH_LOG_STREAM_H_
