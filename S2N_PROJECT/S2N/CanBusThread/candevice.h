#ifndef CANDEVICE_H
#define CANDEVICE_H

#include <QDebug>
#include <QObject>
#include "ECanVci.h"
#define USBCAN  1
class CanDevice : public QObject
{
    Q_OBJECT
public:
    explicit CanDevice(QObject *parent = 0);
    static DWORD m_devType;
    static bool openCanBusDev(uint DeviceType) {
        if (OpenDevice(DeviceType,0,0) != STATUS_OK) {
            qDebug()<<"OpenCanDev USBCAN ERROR";
            return false;
        }
        qDebug()<<"OpenCanDev USBCAN OK";
        return true;
    }

    static bool closeCanBusDev(uint DeviceType) {
        if (CloseDevice(DeviceType,0) != STATUS_OK) {
            qDebug()<<"CloseCanDev USBCAN ERROR";
            return false;
        }
        qDebug()<<"CloseCanDev USBCAN OK";
        return true;
    }
};

#endif // CANDEVICE_H
