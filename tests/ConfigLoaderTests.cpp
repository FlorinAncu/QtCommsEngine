#include <catch2/catch_test_macros.hpp>
#include <QFile>
#include <QTemporaryDir>

#include "qtcommsengine/ConfigLoader.hpp"

TEST_CASE("ConfigLoader returns a deterministic empty config for a missing file",
          "[ConfigLoader]")
{
    QTemporaryDir directory;
    REQUIRE(directory.isValid());

    ConnectionConfig config;
    config.serverAddress = QStringLiteral("stale");
    config.serverPort = 1234;
    config.protocolVersion = 2;
    QString errorMessage;

    REQUIRE_FALSE(ConfigLoader::load(directory.filePath(QStringLiteral("missing.json")),
                                     config, errorMessage));

    REQUIRE_FALSE(errorMessage.isEmpty());
    REQUIRE(config.serverAddress.isEmpty());
    REQUIRE(config.serverPort == 0);
    REQUIRE(config.protocolVersion == 0);
}

TEST_CASE("ConfigLoader reads valid connection settings", "[ConfigLoader]")
{
    QTemporaryDir directory;
    REQUIRE(directory.isValid());

    const QString filePath = directory.filePath(QStringLiteral("config.json"));
    QFile file(filePath);
    REQUIRE(file.open(QIODevice::WriteOnly));
    REQUIRE(file.write(R"({"serverAddress":"localhost","serverPort":12345,"protocolVersion":2})")
            > 0);
    file.close();

    ConnectionConfig config;
    QString errorMessage;
    REQUIRE(ConfigLoader::load(filePath, config, errorMessage));
    REQUIRE(errorMessage.isEmpty());
    REQUIRE(config.serverAddress == QStringLiteral("localhost"));
    REQUIRE(config.serverPort == 12345);
    REQUIRE(config.protocolVersion == 2);
}
