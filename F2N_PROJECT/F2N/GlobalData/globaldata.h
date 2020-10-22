#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QObject>

class GlobalData
{
public:
    explicit GlobalData();
    enum WidgetType{
        NodeInfoType,
        NodeManagerType,
        RecordQueryType,
        UserLoginType
    };

    enum StateType{
        dataNormal       = 0x10,    //正常
        dataOverCurrent  = 0x11,    //过流
        dataLackPhase    = 0x12,    //错相
        dataOverVoltage  = 0x13,    //过压
        dataUnderVoltage = 0x14,    //欠压
        dataPowerLost    = 0x15,    //供电中断
        dataCanBusError  = 0x16     //通讯中断
    };
    enum NodeType{
        MODE_UNKWN = 0x00,  //未知类型
        MODE_V3    = 0x02,  //双路三相电压型
        MODE_V     = 0x03,  //六路单相电压型
        MODE_VA3   = 0x04,  //电压电流型
        MODE_DVA   = 0x05,  //直流电压电流型
        MODE_VA    = 0x06,  //单项电压电流
        MODE_VN3   = 0x07,  //三项双路有零
        MODE_VAN3  = 0x08,  //电压电流有零
        MODE_2VAN3 = 0x09   //两路三项电压一路三项电流
    };
    enum NodeState{
        NodeNormal,      //正常
        NodeOverCurrent, //过流
        NodeLackPhase,   //错相
        NodeOverVoltage, //过压
        NodeUnderVoltage,//欠压
        NodePowerLost,   //供电中断
        NodeCanBusError, //通讯中断
        MainPowerError,  //主电故障
        BackPowerError,  //备电故障
        HostDevError     //主机故障
    };
    //用户登录类型
    enum UserType{
        User,
        Admin
    };
    //历史记录列标签
    enum ColumnLabel{
        Pass,
        CanId,
        State,
        Time,
        Area
    };

    enum SoundType{
        NormalSound,
        ErrorSound,
    };
    //数据类型
    enum DataType{
        PowerData,
        ErrorData
    };

};

#endif // GLOBALDATA_H
