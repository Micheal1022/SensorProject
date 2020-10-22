#ifndef MYPRINT_H
#define MYPRINT_H

#include <QObject>
class QSerialPort;
class MyPrint :public QObject
{

public:
    static char lf[2];
    static QSerialPort *myCom;
    static MyPrint *print;
    static void initSerialPort(QString printName);
    static MyPrint *getInstance();
    static void selfCheckFinished();
    static void dataPring(QString pass, QString canId, QString type, QString state, QString value, QString time, QString addr);
    //static void autoPring(int pass, int canId, int state, QString time, QString addr);
private:

    MyPrint();
   ~MyPrint();


};

#endif // MYPRINT_H
