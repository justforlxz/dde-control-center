#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H

#include <QJsonObject>
#include <QMap>
#include <QString>

class Versions {
public:
    Versions();
    Versions(const QJsonObject &obj);

    void fromJson(const QJsonObject &obj);
    QJsonObject toJson() const;

public:
    QString changeLog;
    int     order;
    QString version;
};

class Description {
public:
    Description();
    Description(const QJsonObject &obj);

    void fromJson(const QJsonObject &obj);
    QJsonObject toJson() const;

public:
    QString description;
    QString name;
    QString slogan;
};

class Locale {
public:
    Locale();
    Locale(const QJsonObject &obj);

    void fromJson(const QJsonObject &obj);
    QJsonObject toJson() const;

public:
    Description     description;
    QString         tags;
    QList<Versions> versions;
};

class PackageInfo {
public:
    PackageInfo();
    PackageInfo(const QJsonObject &obj);

    void fromJson(const QJsonObject &obj);
    QJsonObject toJson() const;

public:
    QString author;
    QString category;
    QString extra;
    QString homePage;
    QString icon;
    int     id;
    QMap<QString, Locale> locale;
    QString name;
    QString packageURI;
    QString packager;
    QString updateTime;
};

#endif  // PACKAGEINFO_H
