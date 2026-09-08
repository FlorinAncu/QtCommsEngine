#include "ConfigLoader.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

ConnectionConfig ConfigLoader::load()
{
    QString dir = getExecutableDirectory();
    QString path = dir + "/config.json";
    return load(path);
}

ConnectionConfig ConfigLoader::load(const QString &filePath)
{
    ConnectionConfig cfg;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return cfg;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject())
    {
        return cfg;
    }

    QJsonObject obj = doc.object();

    cfg.serverAddress = obj.value("serverAddress").toString();
    cfg.serverPort = static_cast<quint16>(obj.value("serverPort").toInt());
    cfg.protocolVersion = static_cast<quint16>(obj.value("protocolVersion").toInt());

    return cfg;
}

QString ConfigLoader::getExecutableDirectory()
{
    return QCoreApplication::applicationDirPath();
}
