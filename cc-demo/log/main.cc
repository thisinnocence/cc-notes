#include "xlog.h"

int main() {
    XLOG_SET_LEVEL(xlog::LogLevel::Error);
    XLOG_ENABLE_TIMESTAMP(true);
    XLOG_INFO("Hello, %s!", "world");
    XLOG_ERROR("Hello, %s!!", "world");
    XLOG_SET_FILE("a.log");
    XLOG_ERROR("Hello, %s!!", "world");
    return 0;
}
