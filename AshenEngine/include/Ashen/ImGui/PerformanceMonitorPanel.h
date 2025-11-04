#ifndef ASHEN_PERFORMANCEMONITORPANEL_H
#define ASHEN_PERFORMANCEMONITORPANEL_H

#include "Ashen/Core/Types.h"
#include <vector>

namespace ash {
    /**
     * @brief ImGui panel for visualizing performance profiling data
     */
    class PerformanceMonitorPanel {
    public:
        PerformanceMonitorPanel() = default;
        ~PerformanceMonitorPanel() = default;

        /**
         * @brief Render the performance monitor panel
         */
        void Render();

    private:
        // FPS tracking
        std::vector<float> m_FrameTimes;
        static constexpr size_t MaxFrameSamples = 120;
        float m_FrameTimeAccumulator = 0.0f;
        size_t m_FrameCount = 0;
    };
}

#endif // ASHEN_PERFORMANCEMONITORPANEL_H
