#ifndef ASHEN_LOGGER_H
#define ASHEN_LOGGER_H

#include <format>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace ash {
    enum class LogLevel { TRACE, INFO, WARN, ERROR };

    class LogStream;

    struct LogMessage {
        std::string text;
        LogLevel level;
    };

    class Logger {
    public:
        static Logger &instance() {
            static Logger logger(std::cout);
            return logger;
        }

        template<typename... Args>
        void log(const LogLevel level, std::format_string<Args...> fmt, Args &&... args) {
            const auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif

            std::string levelStr;
            switch (level) {
                case LogLevel::INFO: levelStr = "INFO";
                    break;
                case LogLevel::WARN: levelStr = "WARN";
                    break;
                case LogLevel::ERROR: levelStr = "ERROR";
                    break;
                default: levelStr = "TRACE";
                    break;
            } {
                const std::string msg = std::format("[{:02}:{:02}:{:02} {}] {}",
                                                    tm.tm_hour, tm.tm_min, tm.tm_sec,
                                                    levelStr,
                                                    std::format(fmt, std::forward<Args>(args)...));
                std::scoped_lock lock(queueMutex);
                messageQueue.push({msg, level});
            }
            cv.notify_one();
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

        static LogStream info();

        static LogStream warn();

        static LogStream error();

        void setOutput(std::ostream &newOut) {
            std::scoped_lock lock(outputMutex);
            out = &newOut;
        }

        void stop() {
            running = false;
            cv.notify_one();
            if (worker.joinable()) worker.join();
        }

        ~Logger() {
            stop();
        }

    private:
        explicit Logger(std::ostream &output) : out(&output), running(true) {
            worker = std::thread([this] { this->processLoop(); });
        }

        static bool isConsole(const std::ostream *os) {
            return os == &std::cout || os == &std::cerr;
        }

        void processLoop() {
            while (running) {
                std::unique_lock lock(queueMutex);
                cv.wait(lock, [this] { return !messageQueue.empty() || !running; });

                while (!messageQueue.empty()) {
                    auto [text, level] = std::move(messageQueue.front());
                    messageQueue.pop();
                    lock.unlock();

                    std::string colorStart, colorEnd = "\033[0m";
                    switch (level) {
                        case LogLevel::INFO: colorStart = "\033[32m";
                            break;
                        case LogLevel::WARN: colorStart = "\033[33m";
                            break;
                        case LogLevel::ERROR: colorStart = "\033[31m";
                            break;
                        default: colorStart = "\033[37m";
                            break;
                    } {
                        std::scoped_lock outLock(outputMutex);
                        if (isConsole(out))
                            *out << colorStart << text << colorEnd << '\n';
                        else
                            *out << text << '\n';
                        out->flush();
                    }

                    lock.lock();
                }
            }

            while (!messageQueue.empty()) {
                auto [text, level] = std::move(messageQueue.front());
                messageQueue.pop();
                if (isConsole(out))
                    *out << text << '\n';
                else
                    *out << text << '\n';
            }
        }

        std::ostream *out;
        std::mutex outputMutex;
        std::queue<LogMessage> messageQueue;
        std::mutex queueMutex;
        std::condition_variable cv;
        std::thread worker;
        std::atomic<bool> running;
    };

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

    inline LogStream Logger::info() { return LogStream(LogLevel::INFO); }
    inline LogStream Logger::warn() { return LogStream(LogLevel::WARN); }
    inline LogStream Logger::error() { return LogStream(LogLevel::ERROR); }
}

#endif // ASHEN_LOGGER_H