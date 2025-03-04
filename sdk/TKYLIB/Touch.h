#ifndef TOUCH_KEY_H_
#define TOUCH_KEY_H_

#include "CH59x_common.h"
#include "TouchKey_CFG.h"
#include "wchtouch.h"
//�Ƿ����������ݴ�ӡ
#define PRINT_EN 0

#if (PRINT_EN)
  #define dg_log               printf
#else
  #define CH_LOGI(TAG,x,...)
#endif

/************************KEY_FIFO_DEFINE******************************/
#define KEY_COUNT       	TKY_MAX_QUEUE_NUM               // �������� 

#define TKY_SHIELD_PIN      GPIO_Pin_4                      //������������
/*�Ƿ�����TMOS*/
#ifndef TMOS_EN
#define TMOS_EN     0
#endif
/*�Ƿ�֧�ִ������߹���*/
#if TMOS_EN
#define TKY_SLEEP_EN 1
#else
#define TKY_SLEEP_EN 0
#endif

#ifndef TKY_FILTER_MODE
#define TKY_FILTER_MODE FILTER_MODE_3
#endif

#if (TKY_FILTER_MODE == FILTER_MODE_1)
#define TKY_PollForFilter() TKY_PollForFilterMode_1()
#elif (TKY_FILTER_MODE == FILTER_MODE_3)
#define TKY_PollForFilter() TKY_PollForFilterMode_3()
#elif (TKY_FILTER_MODE == FILTER_MODE_5)
#define TKY_PollForFilter() TKY_PollForFilterMode_5()
#elif (TKY_FILTER_MODE == FILTER_MODE_7)
#define TKY_PollForFilter() TKY_PollForFilterMode_7()
#elif (TKY_FILTER_MODE == FILTER_MODE_9)
#define TKY_PollForFilter() TKY_PollForFilterMode_9()
#endif


#if (TKY_FILTER_MODE == FILTER_MODE_7)
#define TKY_MEMHEAP_SIZE    	(KEY_COUNT*TKY_BUFLEN*2)     //�ⲿ�������ݻ���������
#else
#define TKY_MEMHEAP_SIZE   		(KEY_COUNT*TKY_BUFLEN)     	 //�ⲿ�������ݻ���������
#endif


typedef struct
{
	  UINT32 PaBit;     //----��¼A_IO��ӦPIN�ţ������ֵ��----
	  UINT32 PbBit;     //----��¼B_IO��ӦPIN�ţ������ֵ��----
	  UINT16 tkyQueueAll;
	  UINT16 RFU;
}TOUCH_S;

/* ����ID, ��Ҫ����tky_GetKeyState()��������ڲ��� */
typedef enum
{
    KID_K0 = 0,
    KID_K1,
    KID_K2,
    KID_K3,
    KID_K4,
    KID_K5,
    KID_K6,
    KID_K7,
    KID_K8,
    KID_K9,
    KID_K10,
    KID_K11
}KEY_ID_E;

/*
    �����˲�ʱ��50ms, ��λ10ms��
    ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
    ��ʹ������·����Ӳ���˲������˲�����Ҳ���Ա�֤�ɿ��ؼ�⵽�����¼�
*/
#define NORMAL_KEY_MODE 0                //������������ģʽ
#define TOUCH_KEY_MODE  1                //������������ģʽ

#define KEY_MODE    NORMAL_KEY_MODE      //����ģʽ����
#define KEY_FILTER_TIME   2              //�����˲�����
#define KEY_LONG_TIME     0              //��λ����tky_KeyScan()���õļ��ʱ��Ϊ׼�� ������������Ϊ�����¼�

typedef uint8_t (*pIsKeyDownFunc)(void);

/*
    ÿ��������Ӧ1��ȫ�ֵĽṹ�������
*/
typedef struct
{
    /* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
    /* �������µ��жϺ���,1��ʾ���� */
    pIsKeyDownFunc IsKeyDownFunc;
    uint8_t  Count;         //�˲���������
    uint16_t LongCount;     //����������
    uint16_t LongTime;      //�������³���ʱ��, 0��ʾ����ⳤ��
    uint8_t  State;         //������ǰ״̬�����»��ǵ���
    uint8_t  RepeatSpeed;   //������������
    uint8_t  RepeatCount;   //��������������
}KEY_T;

/*
    �����ֵ����, ���밴���´���ʱÿ�����İ��¡�����ͳ����¼�

    �Ƽ�ʹ��enum, ����#define��ԭ��
    (1) ����������ֵ,�������˳��ʹ���뿴���������
    (2) �������ɰ����Ǳ����ֵ�ظ���
*/
typedef enum
{
    KEY_NONE = 0,           //0 ��ʾ�����¼� */

    KEY_0_DOWN,             // 1������ 
    KEY_0_UP,               // 1������ 
    KEY_0_LONG,             // 1������ 

    KEY_1_DOWN,             // 2������ 
    KEY_1_UP,               // 2������ 
    KEY_1_LONG,             // 2������ 

    KEY_2_DOWN,             // 3������ 
    KEY_2_UP,               // 3������ 
    KEY_2_LONG,             // 3������ 

    KEY_3_DOWN,             // 4������ 
    KEY_3_UP,               // 4������ 
    KEY_3_LONG,             // 4������ 

    KEY_4_DOWN,             // 5������ 
    KEY_4_UP,               // 5������ 
    KEY_4_LONG,             // 5������ 

    KEY_5_DOWN,             // 6������ 
    KEY_5_UP,               // 6������ 
    KEY_5_LONG,             // 6������ 

    KEY_6_DOWN,             // 7������ 
    KEY_6_UP,               // 7������ 
    KEY_6_LONG,             // 7������ 

    KEY_7_DOWN,             // 8������ 
    KEY_7_UP,               // 8������ 
    KEY_7_LONG,             // 8������ 

    KEY_8_DOWN,             // 9������ 
    KEY_8_UP,               // 9������ 
    KEY_8_LONG,             // 9������ 

    KEY_9_DOWN,             // 0������ 
    KEY_9_UP,               // 0������ 
    KEY_9_LONG,             // 0������ 

    KEY_10_DOWN,            // #������ 
    KEY_10_UP,              // #������ 
    KEY_10_LONG,            // #������ 

    KEY_11_DOWN,            // *������ 
    KEY_11_UP,              // *������ 
    KEY_11_LONG,            // *������ 
}KEY_ENUM;

/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE   64          //�ɸ���ʹ�û�����Ӳ����Ҫ�����޸�*/

typedef struct
{
    uint8_t Buf[KEY_FIFO_SIZE];     // ��ֵ������
    uint8_t Read;                   // ��������ָ��
    uint8_t Write;                  // ������дָ��
}KEY_FIFO_T;

/************************WHEEL_SLIDER_DEFINE****************************/
#define TOUCH_DECIMAL_POINT_PRECISION       (100)
#define TOUCH_WHEEL_ELEMENTS            	(KEY_COUNT)
#define TOUCH_WHEEL_RESOLUTION              (360)
#define TOUCH_OFF_VALUE    					(0xFFFF)

/************************LINE_SLIDER_DEFINE****************************/

/************************TOUCH_PAD_DEFINE****************************/

extern uint8_t wakeupflag; // 0  sleep mode   1  wakeup sta
extern volatile TOUCH_S tkyPinAll;
extern uint16_t keyData, scanData;
extern uint8_t wakeUpCount, wakeupflag;

/* ���ⲿ���õĺ������� */
extern void touch_InitKey(void);
extern void touch_ScanWakeUp(void);
extern void touch_ScanEnterSleep(void);
extern void touch_PutKey(uint8_t _KeyCode);
extern uint8_t touch_GetKey(void);
extern uint8_t touch_GetKeyState(KEY_ID_E _ucKeyID);
extern void touch_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
extern void touch_ClearKey(void);
extern void touch_KeyScan(void);
extern void touch_InfoDebug(void);
extern uint16_t touch_DetectWheelSlider(void);
extern void touch_GPIOModeCfg(GPIOModeTypeDef mode);
extern void touch_GPIOSleep(void);
#endif
