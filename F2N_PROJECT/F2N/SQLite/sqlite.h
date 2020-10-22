#ifndef SQLITE_H
#define SQLITE_H

#include <QDebug>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QtSql>
#include <QMutexLocker>
#include <QSqlQueryModel>
#include <QWaitCondition>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <GlobalData/globaldata.h>
class SQLite : public GlobalData
{
public:
    ~SQLite();

    //
    static bool getKeyState(QSqlDatabase db, int &start, uint &totalTime);
    //
    static bool setTotalTime(QSqlDatabase db, int days, int start);
    //
    static bool setPastTime(QSqlDatabase db, uint passTime);
    //
    static bool setCmdConf(QSqlDatabase db, QString cmdSpeed, QString cmdTimes);
    //
    static void getCmdConf(QSqlDatabase db, int &cmdSpeed, int &cmdTimes);
    //
    static bool setPritnType(QSqlDatabase db, int printError, int printAlarm);
    //
    static void getPritnType(QSqlDatabase db, int &printError, int &printAlarm);
    //获取用户密码
    //static QString getUserPwd(QSqlDatabase db, UserType user);
    //更新用户密码
    //static bool updateUserPwd(QSqlDatabase db, UserType userType, QString pwd);
    //获取主机序列号
    static QString getSerialNumber(QSqlDatabase db);
    //设置主机序列号
    static bool setSerialNumber(QSqlDatabase db, QString serialNumber);
    //获取序列号密钥
    static QString getSecretKey(QSqlDatabase db);
    // 获取数据库连接
    static QSqlDatabase openConnection();
    // 释放数据库连接回连接池
    static void closeConnection(QSqlDatabase connection);
    //获取相应用户下的密码
    //static QString getUserPasswd(QSqlDatabase db,UserType user);
    //获取记录的条数
    static int getRowCount(QSqlDatabase db, const QString &sqlQuery);
    //获取节点的条数
    static int getNodeCount(QSqlDatabase db, int pass);
    //获取节点的条数
    static int getNodeCount(QSqlDatabase db);
    //获取节点地址
    static QString getNodeArea(QSqlDatabase db, QString pass, QString canId);
    //获取节点类型
    static QStringList getNodeInfo(QSqlDatabase db, QString pass, QString canId);
    //获取节点类型
    static QList<QStringList> getNodeInfoList(QSqlDatabase db, QString pass);
    //删除节点数据
    static bool delelteRecordItem(QSqlDatabase db, int pass, int canId, int state, uint time);
    //
    static bool delelteAllItem(QSqlDatabase db);
    //
    static bool insertRecord(QSqlDatabase db, int pass, int canId,int state,uint time, QString area);

    static QList<int> getNodeList(QSqlDatabase db,int pass);

    //static int getPrintState(QSqlDatabase db,PrintType printType);

    static bool getNetWorkInfo(QSqlDatabase db,QString &ip, int &port);

    static bool setNetWorkInfo(QSqlDatabase db, QString used, QString ip, QString port);

    static bool updateNodeInfo(QSqlDatabase db,QString pass, QString canId, QString used, QString ratio);

    static bool insertNode(QSqlDatabase db,QString pass, QString canId, QString used, QString ratio,QString addr);

private:
    //互斥锁
    static QMutex mutex;

    static QWaitCondition waitConnection;
    static SQLite *instance;
    static SQLite& getInstance();

    SQLite();
    // 创建数据库连接
    QSqlDatabase createConnection(const QString &connectionName);
    // 已使用的数据库连接名
    QQueue<QString> usedConntNames;
    // 未使用的数据库连接名
    QQueue<QString> unusedConntNames;
    // 数据库信息 ./Bevone_V2.db
    QString dataBaseName;
    // 数据库类型 QSQLITE
    QString dataBaseType;
    // 取得连接的时候验证连接是否有效
    bool testOnBorrow;
    // 测试访问数据库的 SQL
    QString testOnBorrowSql;
    // 获取连接最大等待时间
    int maxWaitTime;
    // 尝试获取连接时等待间隔时间
    int waitInterval;
    // 最大连接数
    int maxConntCount;
};

#endif // SQLITE_H
