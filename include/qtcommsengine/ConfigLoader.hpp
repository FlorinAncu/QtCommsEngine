#pragma once

#include "ConnectionConfig.hpp"
#include <QString>

class ConfigLoader
{
public:
    // Load config.json from the executable's directory
    static ConnectionConfig load();

    // Load a specific file
    static ConnectionConfig load(const QString &filePath);

private:
    static QString getExecutableDirectory();
};
