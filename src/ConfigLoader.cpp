#include "qtcommsengine/ConfigLoader.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <cmath>

ConnectionConfig ConfigLoader::load()
{
    return load(getExecutableDirectory() + QStringLiteral("/config.json"));
}

ConnectionConfig ConfigLoader::load(const QString &filePath)
{
    ConnectionConfig cfg;
    QString errorMessage;
    if (!load(filePath, cfg, errorMessage))
    {
        qWarning().noquote() << QStringLiteral("Failed to load configuration '%1': %2")
                                    .arg(filePath, errorMessage);
    }
    return cfg;
}

bool ConfigLoader::load(const QString &filePath,
                        ConnectionConfig &config,
                        QString &errorMessage)
{
    config = {};
    errorMessage.clear();

    const auto fail = [&config, &errorMessage](const QString &reason)
    {
        config = {};
        errorMessage = reason;
        return false;
    };

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return fail(file.errorString());
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        return fail(parseError.errorString());
    }

    if (!document.isObject())
    {
        return fail(QStringLiteral("The JSON root must be an object"));
    }

    const QJsonObject object = document.object();
    const QJsonValue address = object.value(QStringLiteral("serverAddress"));
    const QJsonValue port = object.value(QStringLiteral("serverPort"));
    const QJsonValue version = object.value(QStringLiteral("protocolVersion"));
    if (!address.isString() || address.toString().isEmpty())
    {
        return fail(QStringLiteral("'serverAddress' must be a non-empty string"));
    }

    const auto isValidUnsignedField = [](const QJsonValue &value)
    {
        if (!value.isDouble())
        {
            return false;
        }
        const double number = value.toDouble();
        return std::isfinite(number) && number >= 1.0 && number <= 65535.0
            && std::floor(number) == number;
    };
    if (!isValidUnsignedField(port))
    {
        return fail(QStringLiteral("'serverPort' must be an integer from 1 to 65535"));
    }
    if (!isValidUnsignedField(version))
    {
        return fail(QStringLiteral("'protocolVersion' must be an integer from 1 to 65535"));
    }

    config.serverAddress = address.toString();
    config.serverPort = static_cast<quint16>(port.toInt());
    config.protocolVersion = static_cast<quint16>(version.toInt());

    return true;
}

QString ConfigLoader::getExecutableDirectory()
{
    return QCoreApplication::applicationDirPath();
}
