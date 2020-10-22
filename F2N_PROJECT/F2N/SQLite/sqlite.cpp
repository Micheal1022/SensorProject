#include "sqlite.h"

#include <QCryptographicHash>

QMutex SQLite::mutex;
SQLite* SQLite::instance = NULL;
QWaitCondition SQLite::waitConnection;

SQLite::SQLite()
{
    dataBaseName = "./SSApp.db";
    dataBaseType = "QSQLITE";
    testOnBorrow = true;

    maxWaitTime  = 1000;
    waitInterval = 200;
    maxConntCount = 5;
}

SQLite::~SQLite()
{
    // 销毁连接池的时候删除所有的连接
    foreach(QString connectionName, usedConntNames)
    {
        QSqlDatabase::removeDatabase(connectionName);
    }

    foreach(QString connectionName, unusedConntNames)
    {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

bool SQLite::getKeyState(QSqlDatabase db, int &start,uint &totalTime)
{
    uint pastTime;
    QString sqlQuery = "select START,TOTALTIME,PASTTIME from KEYCONF where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            start     = query.value(0).toInt();
            totalTime = query.value(1).toInt();
            pastTime  = query.value(2).toInt();
        }
    }
    query.finish();
    query.clear();

    if(pastTime >= totalTime)
    {
        return true;
    }
    return false;
}

bool SQLite::setTotalTime(QSqlDatabase db, int days, int start)
{
    uint totalTime = 3600*60*60*days+QDateTime::currentDateTime().toTime_t();
    QString startStr = QString::number(start);
    QString totalTimeStr = QString::number(totalTime);
    QString daysStr = QString::number(days);
    QString sqlQuery = "update KEYCONF set TOTALTIME = "+totalTimeStr+",DAYS = "+daysStr+",PASTTIME = 0,START = "+startStr+" where rowid = 1;";
    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

bool SQLite::setPastTime(QSqlDatabase db, uint passTime)
{
    uint totalTime;
    QString sqlQuery1 = "select TOTALTIME from KEYCONF where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery1))
    {
        if(query.next())
        {
            totalTime = query.value(0).toUInt();
        }
    }

    QString sqlQuery2 = "update KEYCONF set PASTTIME = "+QString::number(passTime)+" where rowid = 1;";
    if(!query.exec(sqlQuery2))
    {
        return false;
    }

    if(passTime >= totalTime)
    {
        return true;
    }
    return false;
}

void SQLite::getCmdConf(QSqlDatabase db, int &cmdSpeed, int &cmdTimes)
{
    QString sqlQuery = "select CMDSPEED,CMDTIMES from SYSCONF where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            cmdSpeed = query.value(0).toInt();
            cmdTimes = query.value(1).toInt();
        }
    }
    query.finish();
    query.clear();
}

bool SQLite::setCmdConf(QSqlDatabase db, QString cmdSpeed, QString cmdTimes)
{
    QString sqlQuery = "update SYSCONF set CMDSPEED = "+cmdSpeed+",CMDTIMES = "+cmdTimes+" where rowid = 1;";
    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

bool SQLite::setPritnType(QSqlDatabase db, int printError, int printAlarm)
{
    QString errorStr = QString::number(printError);     QString alarmStr = QString::number(printAlarm);
    QString sqlQuery = "update SYSCONF set PRINTERROR = "+errorStr+",PRINTALARM = "+alarmStr+" where rowid = 1;";
    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

void SQLite::getPritnType(QSqlDatabase db, int &printError, int &printAlarm)
{
    QString sqlQuery = "select PRINTERROR,PRINTALARM from SYSCONF where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            printError = query.value(0).toInt();
            printAlarm = query.value(1).toInt();
        }
    }
    query.finish();
    query.clear();

}

//QString SQLite::getUserPwd(QSqlDatabase db, GlobalData::UserType userType)
//{
//    QString pwd;
//    QString sqlQuery = "select USERPWD from PWD where USERTYPE = "+QString::number(userType)+";";
//    QSqlQuery query(db);
//    if(query.exec(sqlQuery))
//    {
//        if(query.next())
//        {
//            pwd = query.value(0).toString();
//        }
//    }
//    query.clear();
//    return pwd;
//}

//bool SQLite::updateUserPwd(QSqlDatabase db, GlobalData::UserType userType, QString pwd)
//{
//    QString sqlQuery = "update PWD set USERPWD = "+pwd+" where USERTYPE = "+QString::number(userType)+";";
//    QSqlQuery query(db);
//    if(query.exec(sqlQuery))
//    {
//        query.clear();
//        return true;
//    }
//    query.clear();
//    return false;
//}

QString SQLite::getSerialNumber(QSqlDatabase db)
{
    QString serialNumber;
    QString sqlQuery = "select SERIAL from LOCK where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            serialNumber  = query.value(0).toString();
        }
    }
    query.clear();
    return serialNumber;
}

bool SQLite::setSerialNumber(QSqlDatabase db, QString serialNumber)
{
    QByteArray byteKeyMd5 = QCryptographicHash::hash(serialNumber.toLatin1(), QCryptographicHash::Md5);
    QString strPwdMd5 = (byteKeyMd5.toHex().left(12)).toUpper();
    QString sqlQuery = "update LOCK set SERIAL = '"+serialNumber+"',KEY = '"+strPwdMd5+"' where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        return true;
    }
    query.clear();
    return false;
}

QString SQLite::getSecretKey(QSqlDatabase db)
{
    QString passwd;
    QString sqlQuery = "select KEY from LOCK where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            passwd = query.value(0).toString();
        }
    }
    query.finish();
    query.clear();
    return passwd;
}

QSqlDatabase SQLite::openConnection()
{
    SQLite& pool = SQLite::getInstance();
    QMutexLocker locker(&mutex);

    // 已创建连接数
    int conntCount = pool.unusedConntNames.size() + pool.usedConntNames.size();

    // 如果连接已经用完,等待 waitInterval 毫秒看看是否有可用连接,最长等待 maxWaitTime 毫秒
    for(int i=0;i<pool.maxWaitTime&&pool.unusedConntNames.size()==0&&conntCount==pool.maxConntCount;i+=pool.waitInterval)
    {
        waitConnection.wait(&mutex,pool.waitInterval);
        // 重新计算已创建连接数
        conntCount = pool.unusedConntNames.size() + pool.usedConntNames.size();
    }
    QString connectionName;
    if(pool.unusedConntNames.size() > 0)
    {
        // 有已经回收的连接，复用它们
        connectionName = pool.unusedConntNames.dequeue();
    }
    else if(conntCount < pool.maxConntCount)
    {
        // 没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
        connectionName = QString("Connection-%1").arg(conntCount + 1);
    }
    else
    {
        // 已经达到最大连接数
        qDebug() << "Cannot create more connections.";
        return QSqlDatabase();
    }

    // 创建连接
    QSqlDatabase db = pool.createConnection(connectionName);

    // 有效的连接才放入 usedConnectionNames
    if (db.isOpen())
    {
        pool.usedConntNames.enqueue(connectionName);
    }

    return db;
}

void SQLite::closeConnection(QSqlDatabase connection)
{
    SQLite& pool = SQLite::getInstance();
    QString connectionName = connection.connectionName();

    // 如果是我们创建的连接，从 used 里删除,放入 unused 里
    if (pool.usedConntNames.contains(connectionName))
    {
        QMutexLocker locker(&mutex);
        pool.usedConntNames.removeOne(connectionName);
        pool.unusedConntNames.enqueue(connectionName);
        waitConnection.wakeOne();
    }
}

//QString SQLite::getUserPasswd(QSqlDatabase db, GlobalData::UserType user)
//{
//    QString passwd;
//    QString sqlQuery;
//    switch (user) {
//    case GlobalData::User:
//        sqlQuery = "select USER from PWD;";
//        break;
//    case GlobalData::Admin:
//        sqlQuery = "select ADMIN from PWD;";
//        break;
//    case GlobalData::Super:
//        sqlQuery = "select SUPER from PWD;";
//        break;
//    default:
//        break;
//    }

//    QSqlQuery query(db);
//    if(query.exec(sqlQuery))
//    {
//        if(query.next())
//        {
//            passwd = query.value(0).toString();
//        }
//    }
//    query.finish();
//    query.clear();
//    return passwd;
//}

int SQLite::getRowCount(QSqlDatabase db, const QString &sqlQuery)
{
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            return query.value(0).toInt();
        }
    }
    query.finish();
    query.clear();
    return -1;
}

int SQLite::getNodeCount(QSqlDatabase db, int pass)
{
    QString sqlQuery = "select count(*) from NODEINFO where PASS = "+QString::number(pass)+" and USED = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            return query.value(0).toInt();
        }
    }
    query.finish();
    query.clear();
    return 0;
}

int SQLite::getNodeCount(QSqlDatabase db)
{
    QString sqlQuery = "select count(*) from NODEINFO where USED = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            return query.value(0).toInt();
        }
    }
    query.finish();
    query.clear();
    return 0;
}

QString SQLite::getNodeArea(QSqlDatabase db, QString pass, QString canId)
{
    QString address;
    QString sqlQuery = "select AREA from NODEINFO where PASS = "+pass+" and CANID = "+canId+";";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            address = query.value(0).toString();
        }
    }
    query.finish();
    query.clear();
    return address;
}

QStringList SQLite::getNodeInfo(QSqlDatabase db, QString pass, QString canId)
{
    QStringList nodeInfo;
    QString sqlQuery = "select RATIO,AREA from NODEINFO where PASS = "+pass+" and CANID = "+canId+";";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            nodeInfo.append(query.value(0).toString());
            nodeInfo.append(query.value(1).toString());
        }
    }
    query.finish();
    query.clear();
    return nodeInfo;
}

QList<QStringList> SQLite::getNodeInfoList(QSqlDatabase db, QString pass)
{
    QList<QStringList> nodeInfoList;
    QString sqlQuery = "select CANID,USED,RATIO,AREA from NODEINFO where PASS = "+pass+" order by CANID asc;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        while(query.next())
        {
            QStringList nodeInfo;
            nodeInfo.append(query.value(0).toString());
            nodeInfo.append(query.value(1).toString());
            nodeInfo.append(query.value(2).toString());
            nodeInfo.append(query.value(3).toString());
            nodeInfoList.append(nodeInfo);
        }
    }
    query.finish();
    query.clear();
    return nodeInfoList;
}

bool SQLite::delelteRecordItem(QSqlDatabase db, int pass, int canId, int state,uint time)
{
    QString sqlQuery;
    QString passStr = QString::number(pass);
    QString canIdStr = QString::number(canId);
    QString timeStr = QString::number(time);
    QString stateStr = QString::number(state);
    sqlQuery = "delete from RECORD where PASS = "+passStr+" and CANID ="+canIdStr+" and STS ="+stateStr+" and TIME = "+timeStr+";";
    qDebug()<<sqlQuery;
    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();

    return true;
}

bool SQLite::delelteAllItem(QSqlDatabase db)
{
    QString sqlQuery;
    sqlQuery = "delete from RECORD;";
    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

bool SQLite::insertRecord(QSqlDatabase db, int pass, int canId, int state, uint time,QString area)
{
    QString passStr  = QString::number(pass);
    QString canIdStr = QString::number(canId);
    QString stateStr = QString::number(state);
    QString timeStr  = QString::number(time);
    QString sql = "insert into RECORD values(";
    sql += passStr+","+canIdStr+","+stateStr+","+timeStr+",'"+area+"');";
    //qDebug()<<sql;
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

QList<int> SQLite::getNodeList(QSqlDatabase db, int pass)
{
    QList<int> cmdList;
    cmdList.clear();
    QString sqlQuery = "select CANID from NODEINFO where PASS = "+QString::number(pass)+" and USED = 1 order by CANID asc;";
    //qDebug()<<"sqlQuery : "<<sqlQuery;
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        while(query.next())
        {
            cmdList.append(query.value(0).toInt());
        }
    }
    query.finish();
    query.clear();
    return cmdList;
}

//int SQLite::getPrintState(QSqlDatabase db, GlobalData::PrintType printType)
//{
//    QString sqlQuery;
//    if(printType == GlobalData::PrintAlarm)
//    {
//        sqlQuery = "select PRINTALARM from SYSCONF where rowid = 1;";
//    }
//    else
//    {
//        sqlQuery = "select PRINTERROR from SYSCONF where rowid = 1;";
//    }
//    int state = 0;
//    QSqlQuery query(db);
//    if(query.exec(sqlQuery))
//    {
//        if(query.next())
//        {
//            state = query.value(0).toInt();
//        }
//    }
//    query.finish();
//    query.clear();
//    return state;
//}

bool SQLite::getNetWorkInfo(QSqlDatabase db, QString &ip, int &port)
{
    int used;
    QString sqlQuery = "select USED,IP,PORT from NETWORK where rowid = 1;";
    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        if(query.next())
        {
            used = query.value(0).toInt();
            ip   = query.value(1).toString();
            port = query.value(2).toInt();
        }
    }
    query.finish();
    query.clear();
    return used;
}

bool SQLite::setNetWorkInfo(QSqlDatabase db,QString used, QString ip, QString port)
{
    QString sqlQuery = "update NETWORK set USED = "+used+",IP = '"+ip+"',PORT = "+port+";";
    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

bool SQLite::updateNodeInfo(QSqlDatabase db, QString pass, QString canId, QString used, QString ratio)
{
    QString sqlQuery = "update NODEINFO set USED = "+used+",RATIO = "+ratio+" where PASS = "+pass+" and CANID = "+canId+";";
    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

bool SQLite::insertNode(QSqlDatabase db, QString pass, QString canId, QString used, QString ratio, QString addr)
{
    QString sql = "insert into NODEINFO values("+pass+","+canId+","+used+","+ratio+",'"+addr+"');";
    qDebug()<<sql;
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

SQLite &SQLite::getInstance()
{
    if (NULL == instance)
    {
        QMutexLocker locker(&mutex);

        if (NULL == instance)
        {
            instance = new SQLite();
        }
    }
    return *instance;
}


QSqlDatabase SQLite::createConnection(const QString &connectionName)
{
    // 连接已经创建过了,复用它,而不是重新创建
    if(QSqlDatabase::contains(connectionName))
    {
        QSqlDatabase dbPre = QSqlDatabase::database(connectionName);

        if(testOnBorrow)
        {
            //返回连接前访问数据库，如果连接断开，重新建立连接
            //qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
            QSqlQuery query(dbPre);
            if (query.lastError().type() != QSqlError::NoError && !dbPre.open())
            {
                qDebug() << "Open datatabase error:" << dbPre.lastError().text();
                return QSqlDatabase();
            }
        }
        return dbPre;
    }

    // 创建一个新的连接
    QSqlDatabase db = QSqlDatabase::addDatabase(dataBaseType, connectionName);
    db.setDatabaseName(dataBaseName);
    if (!db.open())
    {
        qDebug() << "Open datatabase error:" << db.lastError().text();
        return QSqlDatabase();
    }
    return db;
}



