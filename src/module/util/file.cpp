/*
 * Copyright (c) 2021. Uniontech Software Ltd. All rights reserved.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "file.h"

#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>

namespace linglong {
namespace util {

// 存储软件包信息服务器配置文件
const QString serverConfigPath = getLinglongRootPath() + "/config.json";

QString jonsPath(const QStringList &component)
{
    return QDir::toNativeSeparators(component.join(QDir::separator()));
}

QString getUserFile(const QString &path)
{
    auto dirPath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);
    if (!path.isEmpty()) {
        dirPath += "/" + path;
    }
    return dirPath;
}
QString ensureUserDir(const QString &relativeDirPath)
{
    QStringList dirPathComponents = {
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0),
        relativeDirPath,
    };
    auto dirPath = QDir::cleanPath(dirPathComponents.join(QDir::separator()));
    QDir(dirPath).mkpath(".");
    return dirPath;
}

QString ensureUserDir(const QStringList &relativeDirPathComponents)
{
    auto relativeFilepath = QDir::cleanPath(relativeDirPathComponents.join(QDir::separator().toLatin1()));
    return ensureUserDir(relativeFilepath);
}

bool ensureDir(const QString &path)
{
    QDir dir(path);
    dir.mkpath(".");
    return true;
}

QString createProxySocket(const QString &pattern)
{
    auto userRuntimeDir = QString("/run/user/%1/").arg(getuid());
    QString socketDir = userRuntimeDir + ".dbus-proxy/";
    bool ret = util::createDir(socketDir);
    if (!ret) {
        qCritical() << "createProxySocket pattern:" << pattern << " failed";
        return "";
    }
    QTemporaryFile tmpFile(socketDir + pattern);
    tmpFile.setAutoRemove(false);
    if (!tmpFile.open()) {
        qCritical() << "create " << socketDir + pattern << " failed";
        return "";
    }
    tmpFile.close();
    return tmpFile.fileName();
}

/*
 * 从配置文件获取服务器配置参数
 *
 * @param key: 参数名称
 * @param value: 查询结果
 *
 * @return int: 0:成功 其它:失败
 */
int getLocalConfig(const QString &key, QString &value)
{
    if (!linglong::util::fileExists(serverConfigPath)) {
        qCritical() << serverConfigPath << " not exist";
        return STATUS_CODE(kFail);
    }
    QFile dbFile(serverConfigPath);
    dbFile.open(QIODevice::ReadOnly);
    QString qValue = dbFile.readAll();
    dbFile.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(qValue.toUtf8(), &parseJsonErr);
    if (QJsonParseError::NoError != parseJsonErr.error) {
        qCritical() << "parse linglong config file err";
        return STATUS_CODE(kFail);
    }
    QJsonObject dataObject = document.object();
    if (!dataObject.contains(key)) {
        qWarning() << "key:" << key << " not found in config";
        return STATUS_CODE(kFail);
    }
    value = dataObject[key].toString();
    return STATUS_CODE(kSuccess);
}
} // namespace util
} // namespace linglong