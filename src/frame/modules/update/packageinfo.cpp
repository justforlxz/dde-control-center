#include "packageinfo.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QVariantMap>

Description::Description()
{
}

Description::Description(const QJsonObject &obj)
{
    fromJson(obj);
}

void Description::fromJson(const QJsonObject &obj)
{
    description = obj["description"].toString();
    name        = obj["name"].toString();
    slogan      = obj["slogan"].toString();
}

QJsonObject Description::toJson() const
{
    QJsonObject obj;
    obj["description"] = description;
    obj["name"]        = name;
    obj["slogan"]      = slogan;

    return std::move(obj);
}

Locale::Locale()
{
}

Locale::Locale(const QJsonObject &obj)
{
    fromJson(obj);
}

void Locale::fromJson(const QJsonObject &obj)
{
    description.fromJson(obj["description"].toObject());
    tags = obj["tags"].toString();

    const QJsonArray array = obj["versions"].toArray();
    for (const QJsonValue &value : array) {
        versions << Versions(value.toObject());
    }
}

QJsonObject Locale::toJson() const
{
    QJsonObject obj;
    obj["description"] = description.toJson();
    obj["tags"]        = tags;
    QJsonArray array;
    for (const Versions &v : versions) {
        array.append(v.toJson());
    }
    obj["versions"] = array;

    return std::move(obj);
}

PackageInfo::PackageInfo()
{
}

PackageInfo::PackageInfo(const QJsonObject &obj)
{
    fromJson(obj);
}

void PackageInfo::fromJson(const QJsonObject &obj)
{
    author   = obj["author"].toString();
    category = obj["category"].toString();
    extra    = obj["extra"].toString();
    homePage = obj["homePage"].toString();
    icon     = obj["icon"].toString();
    id       = obj["id"].toInt();
    const QVariantMap localeMap{ obj["locale"].toObject().toVariantMap() };

    for (auto it = localeMap.constBegin(); it != localeMap.constEnd(); ++it) {
        locale[it.key()] = Locale(it.value().toJsonObject());
    }

    name       = obj["name"].toString();
    packageURI = obj["packageURI"].toString();
    packager   = obj["packager"].toString();
    updateTime = obj["updateTime"].toString();
}

QJsonObject PackageInfo::toJson() const
{
    QJsonObject obj;
    obj["author"]     = author;
    obj["category"]   = category;
    obj["extra"]      = extra;
    obj["homePage"]   = homePage;
    obj["icon"]       = icon;
    obj["id"]         = id;
    obj["name"]       = name;
    obj["packageURI"] = packageURI;
    obj["packager"]   = packager;
    obj["updateTime"] = updateTime;

    QVariantMap map;
    for (auto it = locale.constBegin(); it != locale.constEnd(); ++it) {
        map[it.key()] = QVariant(it.value().toJson());
    }
    obj["locale"] = QJsonDocument::fromVariant(map).object();

    return std::move(obj);
}

Versions::Versions()
{
}

Versions::Versions(const QJsonObject &obj)
{
    fromJson(obj);
}

void Versions::fromJson(const QJsonObject &obj)
{
    changeLog = obj["changeLog"].toString();
    order     = obj["order"].toInt();
    version   = obj["version"].toString();
}

QJsonObject Versions::toJson() const
{
    QJsonObject obj;
    obj["changeLog"] = changeLog;
    obj["order"]     = order;
    obj["version"]   = version;

    return std::move(obj);
}
