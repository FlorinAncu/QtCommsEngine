#pragma once

#include "ConnectionConfig.hpp"
#include <QString>

class ConfigLoader
{
public:
    /// Loads config.json from the executable's directory.
    /// Returns an empty configuration and logs a warning on failure.
    static ConnectionConfig load();

    /// Loads a configuration from the specified JSON file.
    /// Returns an empty configuration and logs a warning on failure.
    static ConnectionConfig load(const QString &filePath);

    /// Loads a configuration and reports failures through errorMessage.
    /// On failure, config is reset to its empty state.
    static bool load(const QString &filePath,
                     ConnectionConfig &config,
                     QString &errorMessage);

private:
    static QString getExecutableDirectory();
};
