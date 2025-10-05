#ifndef ASHEN_LOGGER_H
#define ASHEN_LOGGER_H

#include <format>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>

namespace ash {
    enum class LogLevel { TRACE, INFO, WARN, ERROR };

    class LogStream;

    class Logger {
    public:
        static Logger &instance() {
            static Logger logger(std::cout);
            return logger;
        }

        template<typename... Args>
        void log(const LogLevel level, std::format_string<Args...> fmt, Args &&... args) {
            std::scoped_lock lock(mutex);

            const auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif

            std::string levelStr, colorStart;
            const std::string colorEnd = "\033[0m";

            switch (level) {
                case LogLevel::INFO: levelStr = "INFO";
                    colorStart = "\033[32m";
                    break;
                case LogLevel::WARN: levelStr = "WARN";
                    colorStart = "\033[33m";
                    break;
                case LogLevel::ERROR: levelStr = "ERROR";
                    colorStart = "\033[31m";
                    break;
                default: levelStr = "TRACE";
                    colorStart = "\033[37m";
                    break;
            }

            const std::string message = std::format("[{:02}:{:02}:{:02} {}] {}",
                                                    tm.tm_hour, tm.tm_min, tm.tm_sec,
                                                    levelStr,
                                                    std::format(fmt, std::forward<Args>(args)...)
            );

            if (isConsole(out)) {
                *out << colorStart << message << colorEnd << '\n';
            } else {
                *out << message << '\n';
            }
        }

        template<typename... Args>
        static void info(std::format_string<Args...> fmt, Args &&... args) {
            instance().log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void warn(std::format_string<Args...> fmt, Args &&... args) {
            instance().log(LogLevel::WARN, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void error(std::format_string<Args...> fmt, Args &&... args) {
            instance().log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
        }

        void setOutput(std::ostream &newOut) {
            std::scoped_lock lock(mutex);
            out = &newOut;
        }

        // version stream
        static LogStream info();

        static LogStream warn();

        static LogStream error();

    private:
        explicit Logger(std::ostream &output) : out(&output) {
        }

        static bool isConsole(const std::ostream *os) {
            return os == &std::cout || os == &std::cerr;
        }

        std::ostream *out;
        std::mutex mutex;
    };

    // ------------------- LogStream -------------------
    class LogStream {
    public:
        explicit LogStream(const LogLevel level) : m_level(level) {
        }

        ~LogStream() {
            Logger::instance().log(m_level, "{}", m_stream.str());
        }

        template<typename T>
        LogStream &operator<<(T &&value) {
            m_stream << std::forward<T>(value);
            return *this;
        }

    private:
        LogLevel m_level;
        std::ostringstream m_stream;
    };

    // ------------------- Logger stream definitions -------------------
    inline LogStream Logger::info() { return LogStream(LogLevel::INFO); }
    inline LogStream Logger::warn() { return LogStream(LogLevel::WARN); }
    inline LogStream Logger::error() { return LogStream(LogLevel::ERROR); }
}

#endif //ASHEN_LOGGER_H