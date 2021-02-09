/*
 * Copyright (C) 2016 - 2021 Andy Weiss, Adi Hilber
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef TYPES_H
#define TYPES_H

#include <QUuid>
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QtCore/QDataStream>
#include <QJsonObject>
#include <QJsonArray>

//inline QString createNewUID() { return QUuid::createUuid().toString(QUuid::Id128); }

//inline pj_str_t str2Pj(const QString &input_str)
//{
//    pj_str_t output_str;
//    output_str.ptr = (char*)input_str.toStdString().c_str();
//    output_str.slen = input_str.size();
//    return output_str;
//}

//inline QString pj2Str(const pj_str_t &input_str)
//{
//    if (input_str.ptr && input_str.slen>0)
//        return QString::fromStdString(std::string(input_str.ptr, input_str.slen));
//    return QString();
//}

enum DeviceType {
    SoundDevice,
    TestToneGenerator,
    FilePlayer,
    FileRecorder
};
Q_ENUMS(DeviceType)



struct s_audioDevices{
    DeviceType devicetype;
    QString uid;
    QString inputname;
    QString outputame;
    QList <int> portNo;
    int genfrequency;               // only for devicetype TestToneGenerator
    int RecDevID;                   // only for devicetype AudioDevice
    int PBDevID;
    QString path;                   // ony for devicetype Fileplayer, FileRecorder
    QJsonObject toJSON() const {
        QJsonArray portNrArr;
        for (auto & port: portNo) {
            portNrArr.append(port);
        }
        return {{"devicetype", devicetype}, {"uid", uid}, {"inputname", inputname}, {"outputname", outputame}, {"portNo", portNrArr},
                {"genfrequency", genfrequency}, {"RecDevID", RecDevID}, {"PBDevID", PBDevID}, {"path", path},};
    }
    s_audioDevices* fromJSON(QJsonObject &audioDeviceJSON) {
        QJsonArray portNrArr = audioDeviceJSON["portNo"].toArray();
        switch (audioDeviceJSON["devicetype"].toInt()) {
        case SoundDevice:
            devicetype = SoundDevice;
            break;
        case TestToneGenerator:
            devicetype = TestToneGenerator;
            break;
        case FilePlayer:
            devicetype = FilePlayer;
            break;
        case FileRecorder:
            devicetype = FileRecorder;
            break;
        }
        uid = audioDeviceJSON["uid"].toString();
        inputname = audioDeviceJSON["inputname"].toString();
        outputame = audioDeviceJSON["outputname"].toString();
        for (auto portNrObj : portNrArr) {
            portNo.append(portNrObj.toInt());
        }
        genfrequency = audioDeviceJSON["genfrequency"].toInt();
        RecDevID = audioDeviceJSON["RecDevID"].toInt();
        PBDevID = audioDeviceJSON["PBDevID"].toInt();
        path = audioDeviceJSON["path"].toString();
        return this;
    }
};
Q_DECLARE_METATYPE(s_audioDevices);
Q_DECLARE_METATYPE( QList<s_audioDevices>);

struct s_callHistory{
    QString callUri;
    int duration;
    QString codec;
    bool outgoing;
    int count;
};
Q_DECLARE_METATYPE(s_callHistory);
Q_DECLARE_METATYPE(QList<s_callHistory>);

struct s_account{
    QString name;
    QString user;
    QString password;
    QString serverURI;
    QString uid;
    QString FileRecordPath;
    QString FilePlayPath;           // not saved to file, only for runtime handling
    int player_id = -1;
    int rec_id = -1;
    int AccID;
    int splitterSlot;
    QList <int> CallList;       // In JSON only QList <int> CallIDList
    QList <s_callHistory> CallHistory;
};
Q_DECLARE_METATYPE(s_account);
Q_DECLARE_METATYPE( QList<s_account>);

struct s_audioPort{
    QString name;
    QString pjName;
    int slot;
    QJsonObject toJSON() const {
        return {{"name", name}, {"pjName", pjName}, {"slot", slot},};
    }
    s_audioPort* fromJSON(const QJsonObject &audioPortJSON) {
        name = audioPortJSON["name"].toString();
        pjName = audioPortJSON["pjName"].toString();
        slot = audioPortJSON["slot"].toInt();
        return this;
    }
};
Q_DECLARE_METATYPE(s_audioPort);

struct s_audioPortList{
    QList<s_audioPort> srcPorts;
    QList<s_audioPort> destPorts;
    QJsonObject toJSON() const {
        QJsonArray srcPortsArr, destPortsArr;
        for (auto & srcPort: srcPorts) {
            srcPortsArr.append(srcPort.toJSON());
        }
        for (auto & destPort: destPorts) {
            destPortsArr.append(destPort.toJSON());
        }
        return {{"srcPorts", srcPortsArr}, {"destPorts", destPortsArr},};
    }
    s_audioPortList* fromJSON(QJsonObject &audioPortListJSON) {
        QJsonArray srcPortArr, destPortArr;
        srcPorts.clear();
        destPorts.clear();
        if(audioPortListJSON["srcPorts"].isArray() && audioPortListJSON["destPorts"].isArray()) {
            s_audioPort entry;
            srcPortArr = audioPortListJSON["srcPorts"].toArray();
            destPortArr = audioPortListJSON["destPorts"].toArray();
            for (auto srcPort : srcPortArr) {
                srcPorts.append(*entry.fromJSON(srcPort.toObject()));
            }
            for (auto destPort : destPortArr) {
                destPorts.append(*entry.fromJSON(destPort.toObject()));
            }
        }
        return this;
    }
};
Q_DECLARE_METATYPE(s_audioPortList);

struct s_audioRoutes{
    int srcSlot;
    int destSlot;
    QString srcDevName;
    QString destDevName;
    float level;
    bool persistant;
    QJsonObject toJSON() const {
        return {{"srcSlot", srcSlot}, {"destSlot", destSlot}, {"srcDevName", srcDevName}, {"destDevName", destDevName},
                {"level", level}, {"persistant", persistant},};
    }
    s_audioRoutes* fromJSON(const QJsonObject &audioRouteJSON) {
        srcSlot = audioRouteJSON["srcSlot"].toInt();
        destSlot = audioRouteJSON["destSlot"].toInt();
        srcDevName = audioRouteJSON["srcDevName"].toString();
        destDevName = audioRouteJSON["destDevName"].toString();
        level = audioRouteJSON["level"].toDouble();
        persistant = audioRouteJSON["persistant"].toBool();
        return this;
    }
};
Q_DECLARE_METATYPE(s_audioRoutes);
Q_DECLARE_METATYPE(QList<s_audioRoutes>);

struct s_setting{
    QVariant value;
    int min;
    int max;                    // -1 for unknown ??????                   // todo to be discussed....
    QMap<QString, QVariant> enumlist;
};
Q_DECLARE_METATYPE(s_setting);

#endif // TYPES_H