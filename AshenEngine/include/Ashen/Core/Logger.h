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

    enum class LogLevel { Trace, Debug, Info, Warn, Error, Fatal };

    class LogStream;

    struct LogMessage {
        std::string text;
        LogLevel level;
    };

    class Logger {
    public:
        static Logger &Get() {
            static Logger logger(std::cout);
            return logger;
        }

        // Logging formaté
        template<typename... Args>
        void Log(const LogLevel level, std::format_string<Args...> fmt, Args &&... args) {
            if (level < minLevel) return;

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
                case LogLevel::Trace: levelStr = "TRACE"; break;
                case LogLevel::Debug: levelStr = "DEBUG"; break;
                case LogLevel::Info:  levelStr = "INFO"; break;
                case LogLevel::Warn:  levelStr = "WARN"; break;
                case LogLevel::Error: levelStr = "ERROR"; break;
                case LogLevel::Fatal: levelStr = "FATAL"; break;
            }

            const std::string msg = std::format("[{:02}:{:02}:{:02} {}] {}",
                                                tm.tm_hour, tm.tm_min, tm.tm_sec,
                                                levelStr,
                                                std::format(fmt, std::forward<Args>(args)...));

            {
                std::scoped_lock lock(queueMutex);
                messageQueue.push({msg, level});
            }
            cv.notify_one();
        }

        // Versions statiques
        template<typename... Args> static void Trace(std::format_string<Args...> fmt, Args&&... args) { Get().Log(LogLevel::Trace, fmt, std::forward<Args>(args)...); }
        template<typename... Args> static void Debug(std::format_string<Args...> fmt, Args&&... args) { Get().Log(LogLevel::Debug, fmt, std::forward<Args>(args)...); }
        template<typename... Args> static void Info(std::format_string<Args...> fmt, Args&&... args) { Get().Log(LogLevel::Info, fmt, std::forward<Args>(args)...); }
        template<typename... Args> static void Warn(std::format_string<Args...> fmt, Args&&... args) { Get().Log(LogLevel::Warn, fmt, std::forward<Args>(args)...); }
        template<typename... Args> static void Error(std::format_string<Args...> fmt, Args&&... args) { Get().Log(LogLevel::Error, fmt, std::forward<Args>(args)...); }
        template<typename... Args> static void Fatal(std::format_string<Args...> fmt, Args&&... args) { Get().Log(LogLevel::Fatal, fmt, std::forward<Args>(args)...); }

        // Versions LogStream
        static LogStream Trace();
        static LogStream Debug();
        static LogStream Info();
        static LogStream Warn();
        static LogStream Error();
        static LogStream Fatal();

        void SetOutput(std::ostream &newOut) {
            std::scoped_lock lock(outputMutex);
            out = &newOut;
        }

        void SetFileOutput(const std::string& filename) {
            std::scoped_lock lock(outputMutex);
            file.open(filename, std::ios::app);
            if (!file.is_open()) throw std::runtime_error("Impossible d'ouvrir le fichier de log");
        }

        void SetMinLevel(const LogLevel level) {
            minLevel = level;
        }

        void Stop() {
            running = false;
            cv.notify_one();
            if (worker.joinable()) worker.join();
        }

        ~Logger() {
            Stop();
        }

    private:
        explicit Logger(std::ostream &output) : out(&output), running(true), minLevel(LogLevel::Trace) {
            worker = std::thread([this] { this->ProcessLoop(); });
        }

        static bool IsConsole(const std::ostream *os) {
            return os == &std::cout || os == &std::cerr;
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
                        case LogLevel::Trace: colorStart = "\033[37m"; break;  // gris clair
                        case LogLevel::Debug: colorStart = "\033[36m"; break;  // cyan
                        case LogLevel::Info:  colorStart = "\033[32m"; break;  // vert
                        case LogLevel::Warn:  colorStart = "\033[33m"; break;  // jaune
                        case LogLevel::Error: colorStart = "\033[31m"; break;  // rouge
                        case LogLevel::Fatal: colorStart = "\033[1;41m"; break; // fond rouge
                    }

                    std::scoped_lock outLock(outputMutex);
                    if (IsConsole(out)) *out << colorStart << text << colorEnd << '\n';
                    else *out << text << '\n';
                    if (file.is_open()) file << text << '\n';
                    out->flush();
                    if (file.is_open()) file.flush();

                    lock.lock();
                }
            }

            // flush remaining
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

        std::ostream *out;
        std::ofstream file;
        std::mutex outputMutex;
        std::queue<LogMessage> messageQueue;
        std::mutex queueMutex;
        std::condition_variable cv;
        std::thread worker;
        std::atomic<bool> running;
        LogLevel minLevel;
    };

    // LogStream pour syntaxe << pratique
    class LogStream {
    public:
        explicit LogStream(const LogLevel level) : m_level(level) {}
        ~LogStream() {
            Logger::Get().Log(m_level, "{}", m_stream.str());
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

    inline LogStream Logger::Trace() { return LogStream(LogLevel::Trace); }
    inline LogStream Logger::Debug() { return LogStream(LogLevel::Debug); }
    inline LogStream Logger::Info()  { return LogStream(LogLevel::Info); }
    inline LogStream Logger::Warn()  { return LogStream(LogLevel::Warn); }
    inline LogStream Logger::Error() { return LogStream(LogLevel::Error); }
    inline LogStream Logger::Fatal() { return LogStream(LogLevel::Fatal); }

}

#endif // ASHEN_LOGGER_H