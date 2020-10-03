#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "ZComDef.h"

#define GENERICAPP_ENDPOINT             10

#define GENERICAPP_PROFID               0x0F04
#define GENERICAPP_DEVICEID             0x0001
#define GENERICAPP_DEVICE_VERSION       0
#define GENERICAPP_FLAGS                0
#define GENERICAPP_MAX_CLUSTERS         8
#define GENERICAPP_CLUSTERID            1
#define GENERICAPP_CLUSTERID1           2       //���� + �ƹ�
#define GENERICAPP_CLUSTERID2           3       //���� + ����
#define GENERICAPP_CLUSTERID3           4       //���� + ����
#define GENERICAPP_CLUSTERID4           5       //���� + ����
#define GENERICAPP_CLUSTERID5           6       //��ʪ��
#define GENERICAPP_CLUSTERID6           7       //Ӧ����ť + ����
#define GENERICAPP_CLUSTERID7           8       //PM2.5

extern void GenericApp_Init(byte task_id);
extern UINT16 GenericApp_ProcessEvent(byte task_id, UINT16 events);

#endif
