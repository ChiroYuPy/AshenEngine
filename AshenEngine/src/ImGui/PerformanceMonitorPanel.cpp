#include "Ashen/ImGui/PerformanceMonitorPanel.h"

#include <imgui.h>
#include <algorithm>

#ifdef ASHEN_ENABLE_PROFILING
#include "Ashen/Utils/Profiler.h"
#endif

namespace ash {
    void PerformanceMonitorPanel::Render() {
        ImGui::Begin("Performance Monitor");

#ifdef ASHEN_ENABLE_PROFILING
        Profiler& profiler = Profiler::Instance();

        // Controls
        if (ImGui::BeginTabBar("PerformanceMonitorTabs")) {
            // Overview Tab
            if (ImGui::BeginTabItem("Overview")) {
                bool enabled = profiler.IsEnabled();
                if (ImGui::Checkbox("Profiling Enabled", &enabled)) {
                    profiler.SetEnabled(enabled);
                }

                ImGui::SameLine();
                if (ImGui::Button("Reset Stats")) {
                    profiler.Reset();
                }

                ImGui::SameLine();
                if (ImGui::Button("Clear All")) {
                    profiler.Clear();
                }

                ImGui::Separator();

                // Frame time and FPS
                double totalFrameTime = profiler.GetTotalFrameTime();
                float fps = totalFrameTime > 0.0 ? 1000.0f / static_cast<float>(totalFrameTime) : 0.0f;

                // Track frame times for graph
                m_FrameTimes.push_back(static_cast<float>(totalFrameTime));
                if (m_FrameTimes.size() > MaxFrameSamples) {
                    m_FrameTimes.erase(m_FrameTimes.begin());
                }

                // Calculate average FPS
                m_FrameTimeAccumulator += static_cast<float>(totalFrameTime);
                m_FrameCount++;
                float avgFrameTime = m_FrameTimeAccumulator / static_cast<float>(m_FrameCount);
                float avgFPS = avgFrameTime > 0.0f ? 1000.0f / avgFrameTime : 0.0f;

                // Display stats
                ImGui::Text("Current Frame Time: %.3f ms", totalFrameTime);
                ImGui::Text("Current FPS: %.1f", fps);
                ImGui::Spacing();
                ImGui::Text("Average Frame Time: %.3f ms", avgFrameTime);
                ImGui::Text("Average FPS: %.1f", avgFPS);

                // Frame time graph
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Frame Time History");
                if (!m_FrameTimes.empty()) {
                    ImGui::PlotLines("##FrameTime", m_FrameTimes.data(), static_cast<int>(m_FrameTimes.size()),
                                    0, nullptr, 0.0f, 33.33f, ImVec2(0, 80));
                }

                // Reset averaging
                if (ImGui::Button("Reset Average")) {
                    m_FrameTimeAccumulator = 0.0f;
                    m_FrameCount = 0;
                }

                ImGui::EndTabItem();
            }

            // Detailed Stats Tab
            if (ImGui::BeginTabItem("Detailed Stats")) {
                auto results = profiler.GetResults();

                if (results.empty()) {
                    ImGui::TextDisabled("No profiling data available");
                    ImGui::TextWrapped("Use ASH_PROFILE_FUNCTION() or ASH_PROFILE_SCOPE(name) macros in your code to collect profiling data.");
                } else {
                    // Sort by average time (descending)
                    std::sort(results.begin(), results.end(), [](const ProfileResult& a, const ProfileResult& b) {
                        return a.avgTime > b.avgTime;
                    });

                    // Table of results
                    if (ImGui::BeginTable("ProfileResults", 6,
                                         ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                         ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable |
                                         ImGuiTableFlags_ScrollY)) {
                        ImGui::TableSetupColumn("Scope Name", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Calls", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                        ImGui::TableSetupColumn("Total (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        ImGui::TableSetupColumn("Avg (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        ImGui::TableSetupColumn("Min (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        ImGui::TableSetupColumn("Max (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        ImGui::TableHeadersRow();

                        for (const auto& result : results) {
                            ImGui::TableNextRow();

                            ImGui::TableNextColumn();
                            ImGui::Text("%s", result.name.c_str());

                            ImGui::TableNextColumn();
                            ImGui::Text("%llu", result.callCount);

                            ImGui::TableNextColumn();
                            ImGui::Text("%.3f", result.totalTime);

                            ImGui::TableNextColumn();
                            // Highlight slow operations
                            if (result.avgTime > 10.0) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                            } else if (result.avgTime > 5.0) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.3f, 1.0f));
                            }
                            ImGui::Text("%.3f", result.avgTime);
                            if (result.avgTime > 5.0) {
                                ImGui::PopStyleColor();
                            }

                            ImGui::TableNextColumn();
                            ImGui::Text("%.3f", result.minTime);

                            ImGui::TableNextColumn();
                            ImGui::Text("%.3f", result.maxTime);
                        }

                        ImGui::EndTable();
                    }
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
#else
        ImGui::TextDisabled("Profiling is disabled");
        ImGui::Spacing();
        ImGui::TextWrapped("To enable profiling, build in Debug mode or add ASHEN_ENABLE_PROFILING to your compile definitions.");
        ImGui::Spacing();
        ImGui::TextWrapped("Then use ASH_PROFILE_FUNCTION() or ASH_PROFILE_SCOPE(name) macros in your code to collect profiling data.");
#endif

        ImGui::End();
    }
}
