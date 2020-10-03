#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "ZComDef.h"

#define GENERICAPP_ENDPOINT             10

#define GENERICAPP_PROFID               0x0F04
#define GENERICAPP_DEVICEID             0x0001
#define GENERICAPP_DEVICE_VERSION       0
#define GENERICAPP_FLAGS                0
#define GENERICAPP_MAX_CLUSTERS         8
#define GENERICAPP_CLUSTERID            1       //协调器
#define GENERICAPP_CLUSTERID1           2       //人体 + 灯光
#define GENERICAPP_CLUSTERID2           3       //烟雾 + 风扇
#define GENERICAPP_CLUSTERID3           4       //火焰 + 应急按钮 + 警报
#define GENERICAPP_CLUSTERID4           5       //光敏 + 窗帘
#define GENERICAPP_CLUSTERID5           6       //温湿度
#define GENERICAPP_CLUSTERID6           7       //PM2.5


extern void GenericApp_Init(byte task_id);
extern UINT16 GenericApp_ProcessEvent(byte task_id, UINT16 events);

#endif
