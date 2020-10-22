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
    if (query.exec(sqlQuery)) {
        if (query.next()) {
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
    if (query.exec(sqlQuery)) {
        if (query.next()) {
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
    if (query.exec(sqlQuery)) {
        if (query.next()) {
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
    QString sqlQuery = "select PASS,CANID,TYPE,AREA from NODEINFO where PASS = "+pass+" and USED = 1 order by CANID asc;";
    //qDebug()<<"sqlQuery : "<<sqlQuery;
    QSqlQuery query(db);
    if (query.exec(sqlQuery)) {
        while (query.next()) {
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
    sqlQuery = "delete from RECORD where PASS = "+passStr+" and CANID ="+canIdStr+" and STATE ="+stateStr+" and TIME = "+timeStr+";";
    //qDebug()<<sqlQuery;
    QSqlQuery query(db);
    if (!query.exec(sqlQuery)) {
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
    if (!query.exec(sqlQuery)) {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}



bool SQLite::insertNode(QSqlDatabase db, QString pass, QString canId, QString type, QString used, QString addr)
{
    QString sql = "insert into NODEINFO values("+pass+","+canId+","+type+","+used+",'"+addr+"');";
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

bool SQLite::insertRecord(QSqlDatabase db, int pass, int canId, int type, int state, int value, uint time,QString area)
{
    QString passStr  = QString::number(pass);
    QString canIdStr = QString::number(canId);
    QString typeStr  = QString::number(type);
    QString stateStr = QString::number(state);
    QString valueStr = QString::number(value);
    QString timeStr  = QString::number(time);
    QString sql = "insert into RECORD values(";
    sql += passStr+","+canIdStr+","+typeStr+","+stateStr+","+valueStr+","+timeStr+",'"+area+"');";
    //qDebug()<<sql;
    QSqlQuery query(db);
    if (!query.exec(sql)) {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

QList<QPair<int,int> > SQLite::getNodeList(QSqlDatabase db, int pass)
{
    QPair<int,int> nodePair; QList<QPair<int,int> > nodeList;    nodeList.clear();
    QString sqlQuery = "select CANID,TYPE from NODEINFO where PASS = "+QString::number(pass)+" and USED = 1 order by CANID asc;";
    //qDebug()<<"sqlQuery : "<<sqlQuery;
    QSqlQuery query(db);
    if (query.exec(sqlQuery)) {
        while(query.next()) {
            nodePair.first  = query.value(0).toInt();
            nodePair.second = query.value(1).toInt();
            nodeList.append(nodePair);
        }
    }
    query.finish();
    query.clear();
    return nodeList;
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



