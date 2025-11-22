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
template<size_t N, typename CharT = char>
class OHLogStreamBuf : public std::basic_streambuf<CharT> {
    using Base = std::basic_streambuf<CharT>;
public:
    using char_type = typename Base::char_type;
    using traits_type = typename Base::traits_type;
    using int_type = typename traits_type::int_type;
    using pos_type = typename traits_type::pos_type;
    using off_type = typename traits_type::off_type;

    // 构造函数，可指定日志标签和级别
    explicit OHLogStreamBuf(LogLevel level)
        : level_(level) {
        // 设置输出缓冲区
        setp(buffer_.begin(), buffer_.end() - 1);  // 预留一个位置给终止符
    }

protected:
    // 当缓冲区满或遇到特定字符时调用
    int_type overflow(int_type c) override {
        if (c != traits_type::eof()) {
            *Base::pptr() = static_cast<char_type>(c);
            Base::pbump(1);
        }
        // 刷新缓冲区
        if (sync() == -1) {
            return traits_type::eof();
        }
        return c;
    }

    // 同步缓冲区，将数据输出
    int sync() override {
        std::streamsize len = Base::pptr() - Base::pbase();
        if (len > 0) {
            // 终止字符串
            (buffer_ + Base::pbase())[len] = '\0';

            (void)OH_LOG_Print(LOG_APP, level_, LOG_DOMAIN, LOG_TAG, "%{public}s", Base::pbase());

            // 重置缓冲区指针
            setp(buffer_.begin(), buffer_.end() - 1);
        } else {
            return -1;
        }
        return 0;
    }

private:
    std::array<char_type, N> buffer_;  // 缓冲区，可根据需要调整大小
    LogLevel level_;
};

extern std::ostream cout;
extern std::ostream cerr;
extern std::ostream clog;

} // namespace OHOS

#endif  // LOG_OH_LOG_STREAM_H_
