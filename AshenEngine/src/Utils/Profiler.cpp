#include "Ashen/Utils/Profiler.h"
#include "Ashen/Core/Logger.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace ash {
    // ========== ScopedTimer ==========

    ScopedTimer::ScopedTimer(String name)
        : m_Name(std::move(name)),
          m_StartTime(std::chrono::high_resolution_clock::now()) {
    }

    ScopedTimer::~ScopedTimer() {
        if (!m_Stopped) {
            Stop();
        }
    }

    void ScopedTimer::Stop() {
        if (m_Stopped) {
            return;
        }

        const auto endTime = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_StartTime);
        const double ms = static_cast<double>(duration.count()) / 1000.0;

        Profiler::Instance().Record(m_Name, ms);

        m_Stopped = true;
    }

    // ========== Profiler ==========

    Profiler& Profiler::Instance() {
        static Profiler instance;
        return instance;
    }

    void Profiler::Record(const String& name, double duration) {
        if (!m_Enabled) {
            return;
        }

        std::lock_guard lock(m_Mutex);

        auto& result = m_Results[name];
        result.name = name;
        result.callCount++;
        result.totalTime += duration;
        result.duration = duration;

        // Update min/max
        if (duration < result.minTime) {
            result.minTime = duration;
        }
        if (duration > result.maxTime) {
            result.maxTime = duration;
        }

        // Calculate average
        result.avgTime = result.totalTime / static_cast<double>(result.callCount);
    }

    std::vector<ProfileResult> Profiler::GetResults() const {
        std::lock_guard lock(m_Mutex);

        std::vector<ProfileResult> results;
        results.reserve(m_Results.size());

        for (const auto& [name, result] : m_Results) {
            results.push_back(result);
        }

        // Sort by total time (descending)
        std::sort(results.begin(), results.end(),
            [](const ProfileResult& a, const ProfileResult& b) {
                return a.totalTime > b.totalTime;
            });

        return results;
    }

    void Profiler::Clear() {
        std::lock_guard lock(m_Mutex);
        m_Results.clear();
    }

    void Profiler::Reset() {
        std::lock_guard lock(m_Mutex);

        for (auto& [name, result] : m_Results) {
            result.callCount = 0;
            result.totalTime = 0.0;
            result.duration = 0.0;
            result.minTime = 1e9;
            result.maxTime = 0.0;
            result.avgTime = 0.0;
        }
    }

    void Profiler::PrintResults() const {
        auto results = GetResults();

        if (results.empty()) {
            Logger::Info("No profiling data available");
            return;
        }

        Logger::Info("=== Performance Profile ===");

        // Calculate total time
        double totalTime = 0.0;
        for (const auto& result : results) {
            totalTime += result.totalTime;
        }

        // Print header
        std::stringstream ss;
        ss << std::left << std::setw(40) << "Scope"
           << std::right << std::setw(12) << "Calls"
           << std::setw(12) << "Total (ms)"
           << std::setw(12) << "Avg (ms)"
           << std::setw(12) << "Min (ms)"
           << std::setw(12) << "Max (ms)"
           << std::setw(10) << "% Time";

        Logger::Info(ss.str());
        Logger::Info(std::string(110, '-'));

        // Print results
        for (const auto& result : results) {
            ss.str("");
            ss.clear();

            const double percentage = (result.totalTime / totalTime) * 100.0;

            ss << std::left << std::setw(40) << result.name
               << std::right << std::setw(12) << result.callCount
               << std::setw(12) << std::fixed << std::setprecision(3) << result.totalTime
               << std::setw(12) << std::fixed << std::setprecision(3) << result.avgTime
               << std::setw(12) << std::fixed << std::setprecision(3) << result.minTime
               << std::setw(12) << std::fixed << std::setprecision(3) << result.maxTime
               << std::setw(9) << std::fixed << std::setprecision(1) << percentage << "%";

            Logger::Info(ss.str());
        }

        Logger::Info(std::string(110, '-'));
        ss.str("");
        ss.clear();
        ss << "Total measured time: " << std::fixed << std::setprecision(3) << totalTime << " ms";
        Logger::Info(ss.str());
        Logger::Info("===========================");
    }

    double Profiler::GetTotalFrameTime() const {
        auto results = GetResults();

        double total = 0.0;
        for (const auto& result : results) {
            total += result.duration;
        }

        return total;
    }
}
