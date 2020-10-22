#include "corecontrol.h"
#include "MainWindow/mainwindow.h"
#include "CanBusThread/canbusthread.h"
CoreControl::CoreControl(QObject *parent) : QObject(parent)
{

}

CoreControl::~CoreControl()
{
    delete m_mainWindow;
}

void CoreControl::initObj()
{
    m_mainWindow = new MainWindow;
    m_mainWindow->show();
}
