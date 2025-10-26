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
#include <stdexcept>

namespace ash {

/**
 * LogLevel defines the severity of a log message.
 */
enum class LogLevel { Trace, Debug, Info, Warn, Error, Fatal };

/**
 * LogMessage represents a single log entry with its content and severity.
 */
struct LogMessage {
    std::string text;
    LogLevel level;
};

class LogStream;

/**
 * Logger provides thread-safe logging with level filtering and optional file output.
 *
 * Usage:
 *   ash::Logger::Info("Started: {}", val);
 *   ash::Logger::Error("Error: {}", err);
 *   ash::Logger::Warn() << "Warn: " << msg;
 *
 * You can change the output stream or log to a file.
 * Logging is asynchronous.
 */
class Logger {
public:
    /// Singleton access
    static Logger& Get() {
        static Logger logger(std::cout);
        return logger;
    }

    /// Generic log function with format string (C++20)
    template<typename... Args>
    void Log(const LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
        if (level < minLevel) return;

        const auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

        std::string levelStr = ToString(level);
        std::string msg = std::format("[{:02}:{:02}:{:02} {}] {}",
                                      tm.tm_hour, tm.tm_min, tm.tm_sec, levelStr,
                                      std::format(fmt, std::forward<Args>(args)...));
        {
            std::scoped_lock lock(queueMutex);
            messageQueue.push({msg, level});
        }
        cv.notify_one();
    }

    // Convenience static wrappers for each level (format style)
    template<typename... Args> static void Trace(std::format_string<Args...> fmt, Args&&... args) {
        Get().Log(LogLevel::Trace, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args> static void Debug(std::format_string<Args...> fmt, Args&&... args) {
        Get().Log(LogLevel::Debug, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args> static void Info(std::format_string<Args...> fmt, Args&&... args) {
        Get().Log(LogLevel::Info, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args> static void Warn(std::format_string<Args...> fmt, Args&&... args) {
        Get().Log(LogLevel::Warn, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args> static void Error(std::format_string<Args...> fmt, Args&&... args) {
        Get().Log(LogLevel::Error, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args> static void Fatal(std::format_string<Args...> fmt, Args&&... args) {
        Get().Log(LogLevel::Fatal, fmt, std::forward<Args>(args)...);
    }

    // Stream-style logging API (<< style)
    static LogStream Trace();
    static LogStream Debug();
    static LogStream Info();
    static LogStream Warn();
    static LogStream Error();
    static LogStream Fatal();

    /// Set output to an arbitrary std::ostream.
    void SetOutput(std::ostream& newOut) {
        std::scoped_lock lock(outputMutex);
        out = &newOut;
    }

    /// Log output to a file.
    void SetFileOutput(const std::string& filename) {
        std::scoped_lock lock(outputMutex);
        file.open(filename, std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Failed to open log file");
    }

    /// Set the minimal severity level for messages to be logged.
    void SetMinLevel(const LogLevel level) {
        minLevel = level;
    }

    /// Gracefully stop the logger.
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
    explicit Logger(std::ostream& output)
        : out(&output), running(true), minLevel(LogLevel::Trace) {
        worker = std::thread([this] { this->ProcessLoop(); });
    }

    static bool IsConsole(const std::ostream* os) {
        return os == &std::cout || os == &std::cerr;
    }

    static std::string ToString(const LogLevel level) {
        switch (level) {
            case LogLevel::Trace:  return "TRACE";
            case LogLevel::Debug:  return "DEBUG";
            case LogLevel::Info:   return "INFO";
            case LogLevel::Warn:   return "WARN";
            case LogLevel::Error:  return "ERROR";
            case LogLevel::Fatal:  return "FATAL";
        }
        return "UNKNOWN";
    }

    void ProcessLoop() {
        while (running) {
            std::unique_lock lock(queueMutex);
            cv.wait(lock, [this] { return !messageQueue.empty() || !running; });

            while (!messageQueue.empty()) {
                auto [text, level] = std::move(messageQueue.front());
                messageQueue.pop();
                lock.unlock();

                std::string colorStart, colorEnd = "\033[0m";
                switch (level) {
                    case LogLevel::Trace: colorStart = "\033[90m"; break;
                    case LogLevel::Debug: colorStart = "\033[36m"; break;
                    case LogLevel::Info:  colorStart = "\033[32m"; break;
                    case LogLevel::Warn:  colorStart = "\033[33m"; break;
                    case LogLevel::Error: colorStart = "\033[31m"; break;
                    case LogLevel::Fatal: colorStart = "\033[1;41m"; break;
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

        // Flush remaining messages
        while (!messageQueue.empty()) {
            auto [text, level] = std::move(messageQueue.front());
            messageQueue.pop();
            std::scoped_lock outLock(outputMutex);
            *out << text << '\n';
            if (file.is_open()) file << text << '\n';
            out->flush();
            if (file.is_open()) file.flush();
        }
    }

    std::ostream* out;
    std::ofstream file;
    std::mutex outputMutex;

    std::queue<LogMessage> messageQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::thread worker;
    std::atomic<bool> running;
    LogLevel minLevel;
};

/**
 * LogStream is used for stream-style (<<) logging.
 * The message is sent when LogStream is destroyed.
 */
class LogStream {
public:
    explicit LogStream(const LogLevel level) : m_level(level) {}
    ~LogStream() {
        Logger::Get().Log(m_level, "{}", m_stream.str());
    }

    template<typename T>
    LogStream& operator<<(T&& value) {
        m_stream << std::forward<T>(value);
        return *this;
    }

private:
    LogLevel m_level;
    std::ostringstream m_stream;
};

// Definitions of LogStream creators
inline LogStream Logger::Trace() { return LogStream(LogLevel::Trace); }
inline LogStream Logger::Debug() { return LogStream(LogLevel::Debug); }
inline LogStream Logger::Info()  { return LogStream(LogLevel::Info); }
inline LogStream Logger::Warn()  { return LogStream(LogLevel::Warn); }
inline LogStream Logger::Error() { return LogStream(LogLevel::Error); }
inline LogStream Logger::Fatal() { return LogStream(LogLevel::Fatal); }

} // namespace ash

#endif // ASHEN_LOGGER_H