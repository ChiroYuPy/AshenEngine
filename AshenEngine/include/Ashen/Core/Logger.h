#ifndef ASHEN_LOGGER_H
#define ASHEN_LOGGER_H

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
#include <stdexcept>

#include "Ashen/Core/Types.h"

namespace ash {
    enum class LogLevel { Trace, Debug, Info, Warn, Error, Fatal };

    struct LogMessage {
        String text;
        LogLevel level;
    };

    class Logger {
    public:
        static Logger &Get() {
            static Logger logger(std::cout);
            return logger;
        }

        void Log(const LogLevel level, const String &msg) {
            if (level < minLevel) return;

            const String outputMsg = FormatMessage(level, msg); {
                std::scoped_lock lock(queueMutex);
                messageQueue.push({outputMsg, level});
            }
            cv.notify_one();
        }

        static void Trace(const String &msg) { Get().Log(LogLevel::Trace, msg); }
        static void Debug(const String &msg) { Get().Log(LogLevel::Debug, msg); }
        static void Info(const String &msg) { Get().Log(LogLevel::Info, msg); }
        static void Warn(const String &msg) { Get().Log(LogLevel::Warn, msg); }
        static void Error(const String &msg) { Get().Log(LogLevel::Error, msg); }
        static void Fatal(const String &msg) { Get().Log(LogLevel::Fatal, msg); }

        static std::ostringstream Trace() { return std::ostringstream(OStream(LogLevel::Trace)); }
        static std::ostringstream Debug() { return std::ostringstream(OStream(LogLevel::Debug)); }
        static std::ostringstream Info() { return std::ostringstream(OStream(LogLevel::Info)); }
        static std::ostringstream Warn() { return std::ostringstream(OStream(LogLevel::Warn)); }
        static std::ostringstream Error() { return std::ostringstream(OStream(LogLevel::Error)); }
        static std::ostringstream Fatal() { return std::ostringstream(OStream(LogLevel::Fatal)); }

        void SetOutput(std::ostream &newOut) {
            std::scoped_lock lock(outputMutex);
            out = &newOut;
        }

        void SetFileOutput(const String &filename) {
            std::scoped_lock lock(outputMutex);
            file.open(filename, std::ios::app);
            if (!file.is_open())
                throw std::runtime_error("Failed to open log file");
        }

        void SetMinLevel(const LogLevel level) {
            minLevel = level;
        }

        void Stop() {
            running = false;
            cv.notify_one();
            if (worker.joinable())
                worker.join();
        }

        ~Logger() {
            Stop();
        }

    private:
        explicit Logger(std::ostream &output)
            : out(&output), running(true), minLevel(LogLevel::Trace) {
            worker = std::thread([this] { this->ProcessLoop(); });
        }

        static bool IsConsole(const std::ostream *os) {
            return os == &std::cout || os == &std::cerr;
        }

        static String ToString(const LogLevel level) {
            switch (level) {
                case LogLevel::Trace: return "TRACE";
                case LogLevel::Debug: return "DEBUG";
                case LogLevel::Info: return "INFO";
                case LogLevel::Warn: return "WARN";
                case LogLevel::Error: return "ERROR";
                case LogLevel::Fatal: return "FATAL";
            }
            return "UNKNOWN";
        }

        String FormatMessage(const LogLevel level, const String &msg) {
            const auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            std::ostringstream oss;
            oss << "[" << std::setw(2) << std::setfill('0') << tm.tm_hour << ":"
                    << std::setw(2) << std::setfill('0') << tm.tm_min << ":"
                    << std::setw(2) << std::setfill('0') << tm.tm_sec << " "
                    << ToString(level) << "] " << msg;
            return oss.str();
        }

        void ProcessLoop() {
            while (running) {
                std::unique_lock lock(queueMutex);
                cv.wait(lock, [this] { return !messageQueue.empty() || !running; });

                while (!messageQueue.empty()) {
                    auto [text, level] = std::move(messageQueue.front());
                    messageQueue.pop();
                    lock.unlock();

                    String colorStart, colorEnd = "\033[0m";
                    switch (level) {
                        case LogLevel::Trace: colorStart = "\033[90m";
                            break;
                        case LogLevel::Debug: colorStart = "\033[36m";
                            break;
                        case LogLevel::Info: colorStart = "\033[32m";
                            break;
                        case LogLevel::Warn: colorStart = "\033[33m";
                            break;
                        case LogLevel::Error: colorStart = "\033[31m";
                            break;
                        case LogLevel::Fatal: colorStart = "\033[1;41m";
                            break;
                    }

                    std::scoped_lock outLock(outputMutex);
                    if (IsConsole(out))
                        *out << colorStart << text << colorEnd << '\n';
                    else
                        *out << text << '\n';
                    if (file.is_open()) file << text << '\n';

                    out->flush();
                    if (file.is_open()) file.flush();

                    lock.lock();
                }
            }

            while (!messageQueue.empty()) {
                auto [text, level] = MovePtr(messageQueue.front());
                messageQueue.pop();
                std::scoped_lock outLock(outputMutex);
                *out << text << '\n';
                if (file.is_open()) file << text << '\n';
                out->flush();
                if (file.is_open()) file.flush();
            }
        }

        std::ostream *out;
        std::ofstream file;
        std::mutex outputMutex;

        std::queue<LogMessage> messageQueue;
        std::mutex queueMutex;
        std::condition_variable cv;
        std::thread worker;
        std::atomic<bool> running;
        LogLevel minLevel;

        class OStream final : public std::ostringstream {
        public:
            explicit OStream(const LogLevel level) : m_level(level) {
            }

            ~OStream() override {
                Get().Log(m_level, this->str());
            }

        private:
            LogLevel m_level;
        };
    };
}

#endif // ASHEN_LOGGER_H