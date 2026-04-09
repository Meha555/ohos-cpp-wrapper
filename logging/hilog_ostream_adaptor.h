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
template <size_t N, typename CharT = char>
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
        : level_(level)
    {
        this->setp(buffer_.data(), buffer_.data() + N - 1);
    }

    virtual ~OHLogStreamBuf() override
    {
        sync();
    }

    void setLevel(LogLevel level)
    {
        level_ = level;
    }

    LogLevel level() const
    {
        return level_;
    }

protected:
    int_type overflow(int_type c) override
    {
        if (traits_type::eq_int_type(c, traits_type::eof())) {
            return traits_type::not_eof(c);
        }
        // 刷新缓冲区的内容到输出后端
        if (sync() == -1) { // 刷新失败
            return traits_type::eof();
        }

        *this->pptr() = traits_type::to_char_type(c);
        this->pbump(1);

        // 特殊处理换行符
        if (traits_type::eq_int_type(c, traits_type::to_int_type('\n'))) {
            if (sync() == -1) { // 刷新失败
                return traits_type::eof();
            }
        }

        return traits_type::not_eof(c);
    }

    std::streamsize xsputn(const char_type* s, std::streamsize count) override
    {
        std::streamsize written = 0;
        for (; written < count; ++written) {
            if (traits_type::eq_int_type(this->sputc(s[written]), traits_type::eof())) {
                // 写入失败
                break;
            }
            // 特殊处理换行符
            if (s[written] == '\n') {
                sync();
            }
        }
        return written;
    }

    int sync() override
    {
        std::streamsize len = this->pptr() - this->pbase();
        if (len > 0) {
            *this->pptr() = '\0';
            (void)OH_LOG_Print(LOG_APP, level_, LOG_DOMAIN, LOG_TAG, "%{public}s", Base::pbase());
            this->setp(buffer_.data(), buffer_.data() + N - 1);
        }
        return 0;
    }

private:
    std::array<char_type, N> buffer_; // 缓冲区，可根据需要调整大小
    LogLevel level_;
};

extern std::ostream cout;
extern std::ostream cerr;
extern std::ostream clog;

} // namespace OHOS

#endif  // LOG_OH_LOG_STREAM_H_
