#pragma once

#include "ConnectionConfig.h"
#include <QString>

class ConfigLoader
{
public:
    // Încarcă config.json din folderul executabilului
    static ConnectionConfig load();

    // Încarcă un fișier specific
    static ConnectionConfig load(const QString &filePath);

private:
    static QString getExecutableDirectory();
};
