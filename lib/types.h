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


enum DeviceType {
    SoundDevice,
    TestToneGenerator,
    FilePlayer,
    FileRecorder,
    VirtualGpioDevice,
    LogicAndGpioDevice,
    LogicOrGpioDevice,
    AccountGpioDevice,
    LinuxGpioDevice,
    AudioCrosspointDevice
};
Q_ENUMS(DeviceType)

struct s_IODevices{
    DeviceType devicetype;
    QString uid;
    QString inputname;
    QString outputame;
    QList <int> portNo;
    int genfrequency;               // only for devicetype TestToneGenerator
    int RecDevID;                   // only for devicetype AudioDevice
    int PBDevID;
    QString path;                   // ony for devicetype Fileplayer, FileRecorder
    uint inChannelCount = 0;
    uint outChannelCount = 0;
    QJsonObject toJSON() const {
        QJsonArray portNrArr;
        for (auto & port: portNo) {
            portNrArr.append(port);
        }
        return {{"devicetype", devicetype}, {"uid", uid}, {"inputname", inputname}, {"outputname", outputame}, {"portNo", portNrArr},
                {"genfrequency", genfrequency}, {"RecDevID", RecDevID}, {"PBDevID", PBDevID}, {"path", path}, {"inChannelCount", (int) inChannelCount}, {"outChannelCount", (int) outChannelCount}};
    }
    s_IODevices* fromJSON(QJsonObject &ioDeviceJSON) {
        QJsonArray portNrArr = ioDeviceJSON["portNo"].toArray();
        switch (ioDeviceJSON["devicetype"].toInt()) {
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
        case VirtualGpioDevice:
            devicetype = VirtualGpioDevice;
            break;
        case LogicAndGpioDevice:
            devicetype = LogicAndGpioDevice;
            break;
        case LogicOrGpioDevice:
            devicetype = LogicOrGpioDevice;
            break;
        case AccountGpioDevice:
            devicetype = AccountGpioDevice;
            break;
        case LinuxGpioDevice:
            devicetype = LinuxGpioDevice;
            break;
        case AudioCrosspointDevice:
            devicetype = AudioCrosspointDevice;
            break;
        }
        uid = ioDeviceJSON["uid"].toString();
        inputname = ioDeviceJSON["inputname"].toString();
        outputame = ioDeviceJSON["outputname"].toString();
        for (auto portNrObj : portNrArr) {
            portNo.append(portNrObj.toInt());
        }
        genfrequency = ioDeviceJSON["genfrequency"].toInt();
        RecDevID = ioDeviceJSON["RecDevID"].toInt();
        PBDevID = ioDeviceJSON["PBDevID"].toInt();
        path = ioDeviceJSON["path"].toString();
        inChannelCount = (uint) ioDeviceJSON["inChannelCount"].toInt();
        outChannelCount = (uint) ioDeviceJSON["outChannelCount"].toInt();
        return this;
    }
};
Q_DECLARE_METATYPE(s_IODevices);
Q_DECLARE_METATYPE( QList<s_IODevices>);

struct s_callHistory{
    QString callUri;
    int duration;
    QString codec;
    bool outgoing;
    int count;
    QJsonObject toJSON() const {
        return {{"callUri", callUri}, {"duration", duration}, {"codec", codec}, {"outgoing", outgoing}, {"count", count},};
    }
    s_callHistory* fromJSON(const QJsonObject &callHistoryJSON) {
        callUri = callHistoryJSON["callUri"].toString();
        duration = callHistoryJSON["duration"].toInt();
        codec = callHistoryJSON["codec"].toString();
        outgoing = callHistoryJSON["outgoing"].toBool();
        count = callHistoryJSON["count"].toInt();
        return this;
    }
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
    QString FilePlayPath;
    int player_id = -1;
    int rec_id = -1;
    int AccID;
    int splitterSlot;
    QList <QJsonObject> CallList;
    QList <s_callHistory> CallHistory;
    bool fixedJitterBuffer = true;
    uint fixedJitterBufferValue = 160;
    bool autoredialLastCall = false;
    QString SIPStatusText = "waiting for SIP state...";
    int SIPStatusCode = 0;
    QString callStatusLastReason = "";
    QJsonObject toJSON() const {
        QJsonArray callHistoryJSON;
        for (auto & callhistory: CallHistory) {
            callHistoryJSON.append(callhistory.toJSON());
        }
        return {
            {"name", name},
            {"user", user},
            {"password", password},
            {"serverURI", serverURI},
            {"uid", uid},
            {"FileRecordPath", FileRecordPath},
            {"FilePlayPath", FilePlayPath},
            {"AccID", AccID},
            {"CallHistory", callHistoryJSON},
            {"fixedJitterBuffer", fixedJitterBuffer},
            {"fixedJitterBufferValue", (int)fixedJitterBufferValue},
            {"autoredialLastCall", autoredialLastCall}
        };
    }
    s_account* fromJSON(QJsonObject &accountJSON){
        QJsonArray callListArr, callHistoryArr;
        CallList.clear();
        if(accountJSON["CallList"].isArray()){
            callListArr = accountJSON["CallList"].toArray();
            for (auto && callListEntry : callListArr) {
                CallList.append(callListEntry.toObject());
            }
        }
        CallHistory.clear();
        if(accountJSON["CallHistory"].isArray()) {
            s_callHistory entry;
            callHistoryArr = accountJSON["CallHistory"].toArray();
            for (auto && callHistEntry : callHistoryArr) {
                CallHistory.append(*entry.fromJSON(callHistEntry.toObject()));
            }
        }
        name = accountJSON["name"].toString();
        user = accountJSON["user"].toString();
        password = accountJSON["password"].toString();
        serverURI = accountJSON["serverURI"].toString();
        uid = accountJSON["uid"].toString();
        FileRecordPath = accountJSON["FileRecordPath"].toString();
        FilePlayPath = accountJSON["FilePlayPath"].toString();
        player_id = accountJSON["player_id"].toInt();
        rec_id = accountJSON["rec_id"].toInt();
        AccID = accountJSON["AccID"].toInt();
        splitterSlot = accountJSON["splitterSlot"].toInt();
        fixedJitterBuffer = accountJSON["fixedJitterBuffer"].toBool();
        fixedJitterBufferValue = accountJSON["fixedJitterBufferValue"].toInt();
        autoredialLastCall = accountJSON["autoredialLastCall"].toBool();
        SIPStatusCode = accountJSON["SIPStatusCode"].toInt();
        SIPStatusText = accountJSON["SIPStatusText"].toString();
        callStatusLastReason = accountJSON["callStatusLastReason"].toString();
        return this;
    }
};
Q_DECLARE_METATYPE(s_account);
Q_DECLARE_METATYPE( QList<s_account>);

struct s_audioPort{
    QString name;
    QString pjName;
    int slot;
    QJsonObject toJSON() const {
        return {{"name", name}, {"pjName", pjName}, {"slot", slot}};
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
        return {{"srcPorts", srcPortsArr}, {"destPorts", destPortsArr}};
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
        return {{"srcSlot", srcSlot}, {"destSlot", destSlot}, {"srcDevName", srcDevName}, {"destDevName", destDevName}, {"level", level}, {"persistant", persistant} };
    }
    s_audioRoutes fromJSON(QJsonObject &audioRoutesJSON) {
        s_audioRoutes audioroutes;
        srcSlot = audioRoutesJSON["srcSlot"].toInt();
        destSlot = audioRoutesJSON["destSlot"].toInt();
        srcDevName = audioRoutesJSON["srcDevName"].toString();
        destDevName = audioRoutesJSON["destDevName"].toString();
        level = audioRoutesJSON["level"].toVariant().toFloat();
        persistant = audioRoutesJSON["persistant"].toBool();
        return audioroutes;

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

enum settingType{
    INTEGER,
    STRING,
    BOOL_T,
    ENUM_INT,
    ENUM_STRING
};
Q_ENUMS(settingType)

struct s_gpioPort{
    QString name;
    QString slotId;
    QJsonObject toJSON() const {
        return {{"name",name}, {"slotId",slotId}};
    }
    s_gpioPort* fromJSON(const QJsonObject &audioPortJSON) {
        name = audioPortJSON["name"].toString();
        slotId = audioPortJSON["slotId"].toString();
        return this;
    }
};
Q_DECLARE_METATYPE(s_gpioPort);

struct s_gpioPortList{
    QList<s_gpioPort> srcPorts;
    QList<s_gpioPort> destPorts;
    QJsonObject toJSON() const {
        QJsonArray srcPortsArr, destPortsArr;
        for (auto & srcPort: srcPorts) {
            srcPortsArr.append(srcPort.toJSON());
        }
        for (auto & destPort: destPorts) {
            destPortsArr.append(destPort.toJSON());
        }
        return {{"srcPorts", srcPortsArr}, {"destPorts", destPortsArr}};
    }
    s_gpioPortList* fromJSON(QJsonObject &gpioPortListJSON) {
        QJsonArray srcPortArr, destPortArr;
        srcPorts.clear();
        destPorts.clear();
        if(gpioPortListJSON["srcPorts"].isArray() && gpioPortListJSON["destPorts"].isArray()) {
            s_gpioPort entry;
            srcPortArr = gpioPortListJSON["srcPorts"].toArray();
            destPortArr = gpioPortListJSON["destPorts"].toArray();
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
Q_DECLARE_METATYPE(s_gpioPortList);

struct s_gpioRoute{
    QString srcSlotId;
    QString destSlotId;
    bool inverted;
    bool persistant;
    QJsonObject toJSON() const {
        return {{"srcSlotId", srcSlotId}, {"destSlotId", destSlotId}, {"inverted", inverted}, {"persistant", persistant} };
    }
    s_gpioRoute* fromJSON(const QJsonObject &audioRouteJSON) {
        srcSlotId = audioRouteJSON["srcSlotId"].toString();
        destSlotId = audioRouteJSON["destSlotId"].toString();
        inverted = audioRouteJSON["inverted"].toBool();
        persistant = audioRouteJSON["persistant"].toBool();
        return this;
    }
};
Q_DECLARE_METATYPE(s_gpioRoute);
Q_DECLARE_METATYPE(QList<s_gpioRoute>);

#endif // TYPES_H
