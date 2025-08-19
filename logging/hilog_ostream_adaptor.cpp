#include "hilog_ostream_adaptor.h"

namespace OHOS {

// 如果日志输出不全，可以再把缓冲区大小改大一些（比如跑单测的场景）
static OHLogStreamBuf<4096> coutLogBuf(LOG_INFO);
static OHLogStreamBuf<4096> cerrLogBuf(LOG_ERROR);
static OHLogStreamBuf<4096> clogLogBuf(LOG_WARN);

std::ostream cout(&coutLogBuf);
std::ostream cerr(&cerrLogBuf);
std::ostream clog(&clogLogBuf);

} // namespace OHOS
