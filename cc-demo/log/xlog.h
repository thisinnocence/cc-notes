//==============================================================================
// xlog - 简单 C++ 日志库 (Linux Only)
//
// 纯头文件实现，易于集成。
// 支持日志等级过滤、输出到屏幕或文件、可选时间戳、printf 风格格式化。
// 线程安全。
//
// 用法:
//
// 1. 包含头文件:
//    #include "xlog.h"
//
// 2. 配置日志 (可选，通常在程序启动时进行):
//    // 设置日志等级 (Debug, Info, Error, OFF)。默认 Error。
//    xlog::Logger::SetLogLevel(xlog::LogLevel::Debug);
//    XLOG_SET_LEVEL(xlog::LogLevel::Info); // 使用宏
//
//    // 设置日志输出到文件。不设置文件默认打印到屏幕。
//    // 只能从屏幕输出一次性切换到文件。之后调用无效。如果文件打开失败，会回退到屏幕输出。
//    xlog::Logger::SetOutputToFile("app.log");
//    XLOG_SET_FILE("app.log"); // 使用宏
//
//    // 启用/禁用时间戳。默认禁用。
//    xlog::Logger::EnableTimestamp(true);
//    XLOG_ENABLE_TIMESTAMP(false); // 使用宏
//
// 3. 记录日志:
//    // 使用带换行的日志宏 (支持 printf 风格格式化)
//    XLOG_ERROR("发生了一个错误，错误码: %d", errorCode);
//    XLOG_INFO("用户 %s 登录成功", username.c_str());
//    XLOG_DEBUG("处理数据 %f", dataValue);
//
//    // 使用不带换行的日志宏 (支持 printf 风格格式化)
//    XLOG_INFO_RAW("处理进度: ");
//    XLOG_INFO("50%%..."); // 在同一行继续输出
//
// 注意:
// - 仅支持 Linux 平台 (因为使用了 localtime_r)。
// - 日志输出目标 (屏幕/文件) 一旦设置为文件后，无法再切换回屏幕。
// - 使用 printf 风格格式化时，请确保参数类型与格式字符串匹配。
//==============================================================================

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <ratio>
#include <thread>
#include <cstdarg>
#include <cstdio>
#include <vector>

namespace xlog {

enum class LogLevel {
    Debug,
    Info,
    Error,
    OFF
};

class Logger {
public:
    static void SetLogLevel(LogLevel level) {
        GetInstance().currentLogLevel_ = level;
    }

    // 设置日志输出到文件。如果当前正在输出到屏幕，则只在第一次调用时有效。
    static bool SetOutputToFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(GetInstance().mutex_);
        // 只有当当前是输出到屏幕时，才尝试设置文件输出
        if (GetInstance().outputStream_ == &std::cout) {
            GetInstance().outputFile_.open(filename, std::ios::app); // 追加模式
            if (GetInstance().outputFile_.is_open()) {
                GetInstance().outputStream_ = &GetInstance().outputFile_;
                return true;
            } else {
                // 如果文件打开失败，回退到屏幕输出
                GetInstance().outputStream_ = &std::cout;
                return false;
            }
        }
        // 如果输出已经设置为文件，则不做任何事，并报告文件是否已打开
        return GetInstance().outputFile_.is_open();
    }

    // 启用/禁用时间戳
    static void EnableTimestamp(bool enable) {
        GetInstance().timestampEnabled_ = enable;
    }

    // 日志 API (带换行)
    template<typename... Args>
    static void Error(const char* format, Args... args) {
        Log(LogLevel::Error, true, format, args...);
    }

    template<typename... Args>
    static void Info(const char* format, Args... args) {
        Log(LogLevel::Info, true, format, args...);
    }

    template<typename... Args>
    static void Debug(const char* format, Args... args) {
        Log(LogLevel::Debug, true, format, args...);
    }

    // 日志 API (不带换行)
    template<typename... Args>
    static void ErrorRaw(const char* format, Args... args) {
        Log(LogLevel::Error, false, format, args...);
    }

    template<typename... Args>
    static void InfoRaw(const char* format, Args... args) {
        Log(LogLevel::Info, false, format, args...);
    }

    template<typename... Args>
    static void DebugRaw(const char* format, Args... args) {
        Log(LogLevel::Debug, false, format, args...);
    }

private:
    // 单例的私有构造函数和析构函数
    Logger() : currentLogLevel_(LogLevel::Error), outputStream_(&std::cout), timestampEnabled_(false) {}
    ~Logger() {
        if (outputFile_.is_open()) {
            outputFile_.close();
        }
    }

    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    static void Log(LogLevel level, bool addNewline, const char* format, ...) {
        if (GetInstance().currentLogLevel_ == LogLevel::OFF || level < GetInstance().currentLogLevel_) {
            return;
        }

        std::lock_guard<std::mutex> lock(GetInstance().mutex_);

        if (GetInstance().outputStream_) {
            if (GetInstance().timestampEnabled_) {
                auto now = std::chrono::system_clock::now();
                auto time_t_now = std::chrono::system_clock::to_time_t(now);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                std::tm tm_now;
                localtime_r(&time_t_now, &tm_now);
                *GetInstance().outputStream_ << std::put_time(&tm_now, "%H:%M:%S") << '.' << std::setw(3)
                                             << std::setfill('0') << ms.count() << " ";
            }
            *GetInstance().outputStream_ << "[" << LevelToString(level) << "] ";

            va_list args;
            va_start(args, format);
            va_list args_copy;
            va_copy(args_copy, args);
            int size = vsnprintf(nullptr, 0, format, args_copy);
            va_end(args_copy);

            if (size >= 0) {
                std::vector<char> buffer(size + 1);
                vsnprintf(buffer.data(), buffer.size(), format, args);
                *GetInstance().outputStream_ << buffer.data();
            } else {
                *GetInstance().outputStream_ << "Error formatting log message";  // 格式化错误消息
            }
            va_end(args);

            if (addNewline) {
                *GetInstance().outputStream_ << "\n";
            }
            GetInstance().outputStream_->flush();  // 确保立即写入
        }
    }

    static const char* LevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Error: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    LogLevel currentLogLevel_;    // 当前日志等级
    std::ostream* outputStream_;  // 输出流指针 (指向 cout 或文件流)
    std::ofstream outputFile_;    // 文件输出流
    std::mutex mutex_;            // 互斥锁，用于线程安全
    bool timestampEnabled_;       // 是否启用时间戳
};

} // namespace xlog

//==============================================================================
// log with newline
#define XLOG_ERROR(format, ...) xlog::Logger::Error(format, ##__VA_ARGS__)
#define XLOG_INFO(format, ...) xlog::Logger::Info(format, ##__VA_ARGS__)
#define XLOG_DEBUG(format, ...) xlog::Logger::Debug(format, ##__VA_ARGS__)
// log raw
#define XLOG_ERROR_RAW(format, ...) xlog::Logger::ErrorRaw(format, ##__VA_ARGS__)
#define XLOG_INFO_RAW(format, ...) xlog::Logger::InfoRaw(format, ##__VA_ARGS__)
#define XLOG_DEBUG_RAW(format, ...) xlog::Logger::DebugRaw(format, ##__VA_ARGS__)
// config
#define XLOG_SET_LEVEL(level) xlog::Logger::SetLogLevel(level)
#define XLOG_SET_FILE(filename) xlog::Logger::SetOutputToFile(filename)
#define XLOG_ENABLE_TIMESTAMP(enable) xlog::Logger::EnableTimestamp(enable)