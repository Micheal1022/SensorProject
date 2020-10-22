#include "candevice.h"

DWORD CanDevice::m_devType = USBCAN;
CanDevice::CanDevice(QObject *parent) : QObject(parent)
{

}

