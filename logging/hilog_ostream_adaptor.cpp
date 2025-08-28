#include "hilog_ostream_adaptor.h"

namespace OHOS {

#ifndef HILOGPP_STREAMBUF_SIZE
# warning "HILOGPP_STREAMBUF_SIZE is not defined, using default value 1024"
# define HILOGPP_STREAMBUF_SIZE 1024
#endif

// 如果日志输出不全，可以再把缓冲区大小改大一些（比如跑单测的场景）
static OHLogStreamBuf<HILOGPP_STREAMBUF_SIZE> coutLogBuf(LOG_INFO);
static OHLogStreamBuf<HILOGPP_STREAMBUF_SIZE> cerrLogBuf(LOG_ERROR);
static OHLogStreamBuf<HILOGPP_STREAMBUF_SIZE> clogLogBuf(LOG_WARN);

std::ostream cout(&coutLogBuf);
std::ostream cerr(&cerrLogBuf);
std::ostream clog(&clogLogBuf);

} // namespace OHOS
