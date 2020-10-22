#ifndef MYPRINT_H
#define MYPRINT_H

#include <QObject>
#include <GlobalData/globaldata.h>
class QSerialPort;
class MyPrint :public QObject,public GlobalData
{

public:
    static char lf[2];
    static QSerialPort *myCom;
    static MyPrint *print;
    static void initSerialPort(QString printName);
    static MyPrint *getInstance();
    static void selfCheck();
    static void dataPring(QString pass, QString canId, QString state, QString time, QString addr);
    static void autoPring(int pass, int canId, int state, QString time, QString addr);



private:

    MyPrint();
   ~MyPrint();


};

#endif // MYPRINT_H
