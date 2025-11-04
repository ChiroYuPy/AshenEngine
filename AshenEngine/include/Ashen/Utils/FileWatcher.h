#ifndef ASHEN_FILEWATCHER_H
#define ASHEN_FILEWATCHER_H

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <chrono>

#include "Ashen/Core/Types.h"

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief File watcher that detects file modifications
     * Uses polling with last-write-time comparison
     */
    class FileWatcher final {
    public:
        using Callback = std::function<void(const fs::path&)>;
        using TimePoint = fs::file_time_type;

        FileWatcher() = default;
        ~FileWatcher() = default;

        // No copy
        FileWatcher(const FileWatcher&) = delete;
        FileWatcher& operator=(const FileWatcher&) = delete;

        // Move allowed
        FileWatcher(FileWatcher&&) noexcept = default;
        FileWatcher& operator=(FileWatcher&&) noexcept = default;

        /**
         * @brief Watch a file for changes
         * @param path File path to watch
         * @param callback Function to call when file changes
         */
        void Watch(const fs::path& path, Callback callback) {
            if (!fs::exists(path)) {
                return;
            }

            WatchEntry entry;
            entry.lastWriteTime = fs::last_write_time(path);
            entry.callback = std::move(callback);

            m_WatchedFiles[path] = std::move(entry);
        }

        /**
         * @brief Stop watching a file
         */
        void Unwatch(const fs::path& path) {
            m_WatchedFiles.erase(path);
        }

        /**
         * @brief Check all watched files for changes
         * Should be called once per frame
         */
        void Poll() {
            for (auto& [path, entry] : m_WatchedFiles) {
                if (!fs::exists(path)) {
                    continue;
                }

                try {
                    const auto currentWriteTime = fs::last_write_time(path);

                    // Check if file was modified
                    if (currentWriteTime != entry.lastWriteTime) {
                        entry.lastWriteTime = currentWriteTime;

                        // Call the callback
                        if (entry.callback) {
                            entry.callback(path);
                        }
                    }
                } catch (const std::exception&) {
                    // Ignore errors (file might be locked during write)
                    continue;
                }
            }
        }

        /**
         * @brief Clear all watched files
         */
        void Clear() {
            m_WatchedFiles.clear();
        }

        /**
         * @brief Get number of watched files
         */
        [[nodiscard]] size_t WatchCount() const {
            return m_WatchedFiles.size();
        }

    private:
        struct WatchEntry {
            TimePoint lastWriteTime;
            Callback callback;
        };

        std::unordered_map<fs::path, WatchEntry> m_WatchedFiles;
    };
}

#endif // ASHEN_FILEWATCHER_H
