#ifndef ASHEN_PROFILER_H
#define ASHEN_PROFILER_H

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

#include "Ashen/Core/Types.h"

namespace ash {
    /**
     * @brief Performance profiling data for a single scope
     */
    struct ProfileResult {
        String name;
        double duration = 0.0; // milliseconds
        uint64_t callCount = 0;
        double totalTime = 0.0; // milliseconds (accumulated)
        double minTime = 1e9;
        double maxTime = 0.0;
        double avgTime = 0.0;
    };

    /**
     * @brief Scoped timer for automatic profiling
     * Measures time from construction to destruction
     */
    class ScopedTimer final {
    public:
        explicit ScopedTimer(String name);
        ~ScopedTimer();

        // No copy
        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;

        // No move (because of reference in destructor)
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        /**
         * @brief Stop the timer manually (before destructor)
         */
        void Stop();

    private:
        String m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
        bool m_Stopped = false;
    };

    /**
     * @brief Performance profiler singleton
     * Collects and displays profiling data
     */
    class Profiler final {
    public:
        static Profiler& Instance();

        // No copy/move
        Profiler(const Profiler&) = delete;
        Profiler& operator=(const Profiler&) = delete;
        Profiler(Profiler&&) = delete;
        Profiler& operator=(Profiler&&) = delete;

        /**
         * @brief Record a profiling result
         */
        void Record(const String& name, double duration);

        /**
         * @brief Get all profiling results
         */
        [[nodiscard]] std::vector<ProfileResult> GetResults() const;

        /**
         * @brief Clear all profiling data
         */
        void Clear();

        /**
         * @brief Reset statistics (but keep scope names)
         */
        void Reset();

        /**
         * @brief Enable/disable profiling
         */
        void SetEnabled(bool enabled) { m_Enabled = enabled; }

        /**
         * @brief Check if profiling is enabled
         */
        [[nodiscard]] bool IsEnabled() const { return m_Enabled; }

        /**
         * @brief Print profiling results to console
         */
        void PrintResults() const;

        /**
         * @brief Get total frame time (sum of all root scopes)
         */
        [[nodiscard]] double GetTotalFrameTime() const;

    private:
        Profiler() = default;

        std::unordered_map<String, ProfileResult> m_Results;
        mutable std::mutex m_Mutex;
        bool m_Enabled = true;
    };
}

// Profiling macros for easy usage
#ifdef ASHEN_ENABLE_PROFILING
    #define ASH_PROFILE_SCOPE(name) ash::ScopedTimer timer##__LINE__(name)
    #define ASH_PROFILE_FUNCTION() ASH_PROFILE_SCOPE(__FUNCTION__)
#else
    #define ASH_PROFILE_SCOPE(name)
    #define ASH_PROFILE_FUNCTION()
#endif

#endif // ASHEN_PROFILER_H
