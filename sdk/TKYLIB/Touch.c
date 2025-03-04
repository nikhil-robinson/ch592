/********************************** (C) COPYRIGHT *******************************
 * File Name          : Touch.C
 * Author             : WCH
 * Version            : V1.6
 * Date               : 2021/12/1
 * Description        : ������������
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "Touch.h"

/*********************
 *      DEFINES
 *********************/
#define WAKEUPTIME  50     //Sleep Time = 250 * SLEEP_TRIGGER_TIME(100ms) = 25s

/**********************
 *      VARIABLES
 **********************/
uint8_t TKY_MEMBUF[ TKY_MEMHEAP_SIZE ];
uint8_t wakeUpCount = 0, wakeupflag = 0;
uint16_t keyData = 0, scanData = 0;
volatile TOUCH_S tkyPinAll = {0};

static const TKY_ChannelInitTypeDef my_tky_ch_init[TKY_QUEUE_END] = {TKY_CHS_INIT};

static const uint32_t TKY_Pin[14][2] = {
  {0x00, 0x00000010},//PA4
  {0x00, 0x00000020},//PA5
  {0x00, 0x00001000},//PA12
  {0x00, 0x00002000},//PA13
  {0x00, 0x00004000},//PA14
  {0x00, 0x00008000},//PA15
  {0x00, 0x00000000},//AIN6�����ڣ��ڴ�ռλ
  {0x00, 0x00000000},//AIN7�����ڣ��ڴ�ռλ

  {0x20, 0x00000001},//PB0,AIN8��592X�ͺ���
  {0x20, 0x00000040},//PB6,AIN9��592X�ͺ���

  {0x00, 0x00000040},//PA6,AIN10��592X�ͺ���
  {0x00, 0x00000080},//PA7,AIN11��592X�ͺ���
  {0x00, 0x00000100},//PA8,AIN12
  {0x00, 0x00000200} //PA9,AIN13
};
/**********************
 *  STATIC PROTOTYPES
 **********************/
static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;       /* ����FIFO����,�ṹ�� */
static void touch_InitKeyHard(void);
static void touch_InitKeyVar(void);
static void touch_DetectKey(uint8_t i);
static void touch_Baseinit(void);
static void touch_Channelinit(void);
static uint8_t IsKeyDown1(void);
static uint8_t IsKeyDown2(void);
static uint8_t IsKeyDown3(void);
static uint8_t IsKeyDown4(void);
static uint8_t IsKeyDown5(void);
static uint8_t IsKeyDown6(void);
static uint8_t IsKeyDown7(void);
static uint8_t IsKeyDown8(void);
static uint8_t IsKeyDown9(void);
static uint8_t IsKeyDown10(void);
static uint8_t IsKeyDown11(void);
static uint8_t IsKeyDown12(void);

pIsKeyDownFunc KeyDownFunc[14] =
{
        IsKeyDown1,
        IsKeyDown2,
        IsKeyDown3,
        IsKeyDown4,
        IsKeyDown5,
        IsKeyDown6,
        IsKeyDown7,
        IsKeyDown8,
        IsKeyDown9,
        IsKeyDown10,
        IsKeyDown11,
        IsKeyDown12
};
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/********************************************************************************************************
 * @fn      touch_InitKey
 * 
 * @brief   ��ʼ������. �ú����� tky_Init() ���á�
 *
 * @return  none
 */
void touch_InitKey(void)
{
    touch_InitKeyHard();          /* ��ʼ������Ӳ�� */
    touch_InitKeyVar();           /* ��ʼ���������� */
}

/********************************************************************************************************
 * @fn      touch_PutKey
 * @brief   ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
 * @param   _KeyCode - ��������
 * @return  none
 */
void touch_PutKey(uint8_t _KeyCode)
{
    s_tKey.Buf[s_tKey.Write] = _KeyCode;

    if (++s_tKey.Write  >= KEY_FIFO_SIZE)
    {
        s_tKey.Write = 0;
    }
}

/********************************************************************************************************
 * @fn      touch_GetKey
 * @brief   �Ӱ���FIFO��������ȡһ����ֵ��
 * @param   ��
 * @return  ��������
 */
uint8_t touch_GetKey(void)
{
    uint8_t ret;

    if (s_tKey.Read == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read];

        if (++s_tKey.Read >= KEY_FIFO_SIZE)
        {
            s_tKey.Read = 0;
        }
        return ret;
    }
}

/********************************************************************************************************
 * @fn      touch_GetKeyState
 * @brief   ��ȡ������״̬
 * @param   _ucKeyID - ����ID����0��ʼ
 * @return  1 - ����
 *          0 - δ����
*********************************************************************************************************
*/
uint8_t touch_GetKeyState(KEY_ID_E _ucKeyID)
{
    return s_tBtn[_ucKeyID].State;
}

/********************************************************************************************************
 * @fn      touch_SetKeyParam
 * @brief   ���ð�������
 * @param   _ucKeyID     - ����ID����0��ʼ
 *          _LongTime    - �����¼�ʱ��
 *          _RepeatSpeed - �����ٶ�
 * @return  none
 */
void touch_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
    s_tBtn[_ucKeyID].LongTime = _LongTime;          /* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
    s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;            /* �����������ٶȣ�0��ʾ��֧������ */
    s_tBtn[_ucKeyID].RepeatCount = 0;                       /* ���������� */
}


/********************************************************************************************************
 * @fn      touch_ClearKey
 * @brief   ��հ���FIFO������
 * @param   ��
 * @return  ��������
 */
void touch_ClearKey(void)
{
    s_tKey.Read = s_tKey.Write;
}

/********************************************************************************************************
 * @fn      touch_ScanWakeUp
 * @brief   ����ɨ�軽�Ѻ���
 * @param   ��
 * @return  ��
 */
void touch_ScanWakeUp(void)
{
    wakeUpCount = WAKEUPTIME; //---����ʱ��---
    wakeupflag = 1;           //�óɻ���״̬

    TKY_SetSleepStatusValue( ~tkyPinAll.tkyQueueAll ); //---����0~11ͨ��Ϊ������״̬,Ϊ����������ʱ��������ɨ����׼��---
    CH_LOGI(TAG,"wake up for a while");
    TKY_SaveAndStop();    //---����ؼĴ������б���---
    touch_GPIOSleep();
}

/********************************************************************************************************
 * @fn      touch_ScanEnterSleep
 * @brief   ����ɨ�����ߺ���
 * @param   ��
 * @return  ��
 */
void touch_ScanEnterSleep(void)
{
    TKY_SaveAndStop();    //---����ؼĴ������б���---
    touch_GPIOSleep();
    wakeupflag = 0;       //�ó�˯��״̬:0,����̬:1
    TKY_SetSleepStatusValue( tkyPinAll.tkyQueueAll );
    CH_LOGI(TAG,"Ready to sleep");
}

/********************************************************************************************************
 * @fn      touch_KeyScan
 * @brief   ɨ�����а���������������systick�ж������Եĵ���
 * @param   ��
 * @return  ��
 */
void touch_KeyScan(void)
{
    uint8_t i;
    TKY_LoadAndRun( );                     //---��������ǰ����Ĳ�������---

    keyData = TKY_PollForFilter( );

#if TKY_SLEEP_EN
    if (keyData)
    {
        wakeUpCount = WAKEUPTIME; //---����ʱ��---
    }
#endif

    for (i = 0; i < KEY_COUNT; i++)
    {
        touch_DetectKey(i);
    }
    TKY_SaveAndStop();    //---����ؼĴ������б���---
}

/********************************************************************************************************
 * @fn      touch_GPIOModeCfg
 * @brief   ��������ģʽ����
 * @param   ��
 * @return  ��
 */
void touch_GPIOModeCfg(GPIOModeTypeDef mode)
{
    uint32_t pina = tkyPinAll.PaBit;
    uint32_t pinb = tkyPinAll.PbBit;
    switch(mode)
    {
        case GPIO_ModeIN_Floating:
        	R32_PA_PD_DRV &= ~pina;
        	R32_PA_PU &= ~pina;
        	R32_PA_DIR &= ~pina;
        	R32_PB_PD_DRV &= ~pinb;
        	R32_PB_PU &= ~pinb;
        	R32_PB_DIR &= ~pinb;
            break;

        case GPIO_ModeOut_PP_5mA:
            R32_PA_PU &= ~pina;
            R32_PA_PD_DRV &= ~pina;
            R32_PA_DIR |= pina;
            R32_PB_PU &= ~pinb;
            R32_PB_PD_DRV &= ~pinb;
            R32_PB_DIR |= pinb;
            break;
        default:
            break;
    }
}

void touch_IOSetAdcState(uint8_t ch)
{
	(*((PUINT32V)0x400010B4+TKY_Pin[ch][0])) &= ~TKY_Pin[ch][1];
	(*((PUINT32V)0x400010B0+TKY_Pin[ch][0])) &= ~TKY_Pin[ch][1];
	(*((PUINT32V)0x400010A0+TKY_Pin[ch][0])) &= ~TKY_Pin[ch][1];
}

void touch_SingleChDischarge(uint8_t ch){
	(*((PUINT32V)(0x400010A0+TKY_Pin[ch][0]))) |= TKY_Pin[ch][1];
	(*((PUINT32V)(0x400010AC+TKY_Pin[ch][0]))) = TKY_Pin[ch][1];
}

/********************************************************************************************************
 * @fn      touch_GPIOSleep
 * @brief   ���ô�������Ϊ����״̬
 * @param   ��
 * @return  ��
 */
void touch_GPIOSleep(void)
{
    uint32_t pina = tkyPinAll.PaBit;
    uint32_t pinb = tkyPinAll.PbBit;
    R32_PA_PU &= ~pina;
    R32_PA_PD_DRV &= ~pina;
    R32_PA_DIR |= pina;
    R32_PA_CLR |= pina;

    R32_PB_PU &= ~pinb;
    R32_PB_PD_DRV &= ~pinb;
    R32_PB_DIR |= pinb;
    R32_PB_CLR |= pinb;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/********************************************************************************************************
 * @fn      touch_InitKeyHard
 * @brief   ��ʼ����������
 * @param   ��
 * @return  ��
 */
static void touch_InitKeyHard(void)
{
    touch_Baseinit( );
    touch_Channelinit( );
}


/********************************************************************************************************
 * @fn      touch_InitKeyVar
 * @brief   ��ʼ��������������
 * @param   ��
 * @return  ��
 */
static void touch_InitKeyVar(void)
{
    uint8_t i;

    /* �԰���FIFO��дָ������ */
    s_tKey.Read = 0;
    s_tKey.Write = 0;

    /* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
    for (i = 0; i < KEY_COUNT; i++)
    {
        s_tBtn[i].LongTime = KEY_LONG_TIME;             /* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
        s_tBtn[i].Count = KEY_FILTER_TIME / 2;          /* ����������Ϊ�˲�ʱ���һ�� */
        s_tBtn[i].State = 0;                            /* ����ȱʡ״̬��0Ϊδ���� */
        s_tBtn[i].RepeatSpeed = 0;                      /* �����������ٶȣ�0��ʾ��֧������ */
        s_tBtn[i].RepeatCount = 0;                      /* ���������� */
        s_tBtn[i].IsKeyDownFunc = KeyDownFunc[i];       /* �жϰ������µĺ��� */
    }

    /* �����Ҫ��������ĳ�������Ĳ����������ڴ˵������¸�ֵ */
    /* ���磬����ϣ������1���³���1����Զ��ط���ͬ��ֵ */
//    s_tBtn[KID_K1].LongTime = 100;
//    s_tBtn[KID_K1].RepeatSpeed = 5; /* ÿ��50ms�Զ����ͼ�ֵ */

}

/********************************************************************************************************
 * @fn      IsKeyDownX
 * @brief   �жϰ����Ƿ���,�û�����������ʵ�ָú�������
 * @param   ��
 * @return  1 - ����
 *          0 - δ����
 */
static uint8_t IsKeyDown1(void)
{
    if (keyData & 0x0001)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown2(void)
{
    if (keyData & 0x0002)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown3(void)
{
    if (keyData & 0x0004)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown4(void)
{
    if (keyData & 0x0008)   return 1;
    else                    return 0;
}


static uint8_t IsKeyDown5(void)
{
    if (keyData & 0x0010)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown6(void)
{
    if (keyData & 0x0020)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown7(void)
{
    if (keyData & 0x0040)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown8(void)
{
    if (keyData & 0x0080)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown9(void)
{
    if (keyData & 0x0100)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown10(void)
{
    if (keyData & 0x0200)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown11(void)
{
    if (keyData & 0x0400)   return 1;
    else                    return 0;
}

static uint8_t IsKeyDown12(void)
{
    if (keyData & 0x0800)   return 1;
    else                    return 0;
}


/********************************************************************************************************
 * @fn      touch_InfoDebug
 * @brief   �������ݴ�ӡ����
 * @param   ��
 * @return  ��
 */
 
void touch_InfoDebug(void)
{
    uint8_t i;
    int16_t data_dispNum[ TKY_MAX_QUEUE_NUM ]={0};
	int16_t bl,vl;

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
#if TKY_FILTER_MODE == FILTER_MODE_1
        bl = TKY_GetCurQueueBaseLine( i );
        vl = TKY_GetCurQueueValue( i );
        if(bl>vl)   data_dispNum[ i ] =  bl-vl ;
        else        data_dispNum[ i ] =  vl-bl ;
#else
        data_dispNum[ i ] = TKY_GetCurQueueValue( i );
#endif
    }

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
        CH_LOGI(TAG,"%04d,", data_dispNum[i]);
    } CH_LOGI(TAG,"");

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
        data_dispNum[ i ] = TKY_GetCurQueueBaseLine( i );
    }

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
        CH_LOGI(TAG,"%04d,", data_dispNum[i]);
    } CH_LOGI(TAG,"");
#if TKY_FILTER_MODE == FILTER_MODE_1
    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
        CH_LOGI(TAG,"%04d,", TKY_GetCurQueueValue( i ));
    }CH_LOGI(TAG,"");
#endif
    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
        CH_LOGI(TAG,"%04d,", TKY_GetCurQueueRealVal( i ));
    }CH_LOGI(TAG,"\r");
#if TKY_FILTER_MODE == FILTER_MODE_7
    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
    	CH_LOGI(TAG,"%04d,", TKY_GetCurQueueValue2( i ));
    }CH_LOGI(TAG,"\r");
    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
    	CH_LOGI(TAG,"%04d,", TKY_GetCurQueueBaseLine2( i ));
    }CH_LOGI(TAG,"\r");
    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
    	CH_LOGI(TAG,"%04d,", TKY_GetCurQueueRealVal2( i ));
    }CH_LOGI(TAG,"\r");
#endif
    CH_LOGI(TAG,"\r");

}

/********************************************************************************************************
 * @fn      touch_DetectKey
 * @brief   ���һ��������������״̬�����뱻�����Եĵ��á�
 * @param   i - �����ṹ����ָ��
 * @return  ��
 */
static void touch_DetectKey(uint8_t i)
{
    KEY_T *pBtn;

/*��������*/
    pBtn = &s_tBtn[i];
    if (pBtn->IsKeyDownFunc()==1)
    {
            if (pBtn->State == 0)
            {
                pBtn->State = 1;
#if !KEY_MODE
                /* ���Ͱ�ť���µ���Ϣ */
                touch_PutKey((uint8_t)(3 * i + 1));
#endif
            }

            /*����������*/
            if (pBtn->LongTime > 0)
            {
                if (pBtn->LongCount < pBtn->LongTime)
                {
                    /* ���Ͱ�ť�����µ���Ϣ */
                    if (++pBtn->LongCount == pBtn->LongTime)
                    {
#if !KEY_MODE
                        pBtn->State = 2;

                        /* ��ֵ���밴��FIFO */
                        touch_PutKey((uint8_t)(3 * i + 3));
#endif
                    }
                }
                else
                {
                    if (pBtn->RepeatSpeed > 0)
                    {
                        if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
                        {
                            pBtn->RepeatCount = 0;
#if !KEY_MODE
                            /* ��������ÿ��pBtn->RepeatSpeed*10ms����1������ */
                            touch_PutKey((uint8_t)(3 * i + 1));
#endif
                        }
                    }
                }
            }
    }
    else
    {
            if (pBtn->State)
            {
#if KEY_MODE
                if(pBtn->State == 1)
                /* ���Ͱ�ť���µ���Ϣ */
                touch_PutKey((uint8_t)(3 * i + 1));
#endif
                pBtn->State = 0;

#if !KEY_MODE
                /* �ɿ�����KEY_FILTER_TIME�� ���Ͱ�ť�������Ϣ */
                touch_PutKey((uint8_t)(3 * i + 2));
#endif
            }

        pBtn->LongCount = 0;
        pBtn->RepeatCount = 0;
    }
}

/********************************************************************************************************
 * @fn      touch_Baseinit
 * @brief   �����������ʼ��
 * @param   ��
 * @return  ��
 */
static void touch_Baseinit(void)
{
    TKY_BaseInitTypeDef TKY_BaseInitStructure = {0};
    for(uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++)  //��ʼ��tkyPinAll��tkyQueueAll����
    {
    	if(TKY_Pin[my_tky_ch_init[i].channelNum][0] == 0x00)
    	{
    		tkyPinAll.PaBit |= TKY_Pin[my_tky_ch_init[i].channelNum][1];
    	}
    	else if(TKY_Pin[my_tky_ch_init[i].channelNum][0] == 0x20)
      {
    		tkyPinAll.PbBit |= TKY_Pin[my_tky_ch_init[i].channelNum][1];
    	}
    	tkyPinAll.tkyQueueAll |= 1<<i;
    }
    CH_LOGI(TAG,"tP : %08x,%08x; tQ : %04x",tkyPinAll.PaBit,tkyPinAll.PbBit,tkyPinAll.tkyQueueAll);
    R8_TKEY_CFG|=RB_TKEY_CURRENT;
#if (TKY_SHIELD_EN)&&((TKY_FILTER_MODE != FILTER_MODE_9))
    tkyPinAll.PaBit |= TKY_SHIELD_PIN;
#endif

    touch_GPIOSleep();  //�������д���pin��

#if (TKY_SHIELD_EN)&&((TKY_FILTER_MODE != FILTER_MODE_9))
    tkyPinAll.PaBit &= ~TKY_SHIELD_PIN;
    GPIOA_ModeCfg(TKY_SHIELD_PIN, GPIO_ModeIN_Floating);//Shield Pin�� only for CH58x series
#endif
    //----------���������������ó�ʼ��--------
    TKY_BaseInitStructure.filterMode = TKY_FILTER_MODE;
#if (TKY_FILTER_MODE != FILTER_MODE_9)
    TKY_BaseInitStructure.shieldEn = TKY_SHIELD_EN;
#else
    TKY_BaseInitStructure.shieldEn = 0;
#endif
    TKY_BaseInitStructure.singlePressMod = TKY_SINGLE_PRESS_MODE;
    TKY_BaseInitStructure.filterGrade = TKY_FILTER_GRADE;
    TKY_BaseInitStructure.maxQueueNum = TKY_MAX_QUEUE_NUM;
    TKY_BaseInitStructure.baseRefreshOnPress = TKY_BASE_REFRESH_ON_PRESS;
    //---���߸����ٶȣ�baseRefreshSampleNum��filterGrade������߸����ٶȳɷ��ȣ����߸����ٶȻ������ṹ��أ���ͨ������GetCurQueueBaseLine���۲�---
    TKY_BaseInitStructure.baseRefreshSampleNum = TKY_BASE_REFRESH_SAMPLE_NUM;
    TKY_BaseInitStructure.baseUpRefreshDouble = TKY_BASE_UP_REFRESH_DOUBLE;
    TKY_BaseInitStructure.baseDownRefreshSlow = TKY_BASE_DOWN_REFRESH_SLOW;
    TKY_BaseInitStructure.tkyBufP = TKY_MEMBUF;
    TKY_BaseInit( TKY_BaseInitStructure );
}

/********************************************************************************************************
 * @fn      touch_Channelinit
 * @brief   ����ͨ����ʼ��
 * @param   ��
 * @return  ��
 */
static void touch_Channelinit(void)
{

    uint8_t error_flag = 0;
    uint16_t chx_mean = 0;

    for(uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
    	TKY_CHInit(my_tky_ch_init[i]);
    }

#if (TKY_FILTER_MODE != FILTER_MODE_9)
    for(uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {

    	chx_mean = TKY_GetCurChannelMean(my_tky_ch_init[i].channelNum, my_tky_ch_init[i].chargeTime,
										 my_tky_ch_init[i].disChargeTime, 1000);

    	if(chx_mean < 3400 || chx_mean > 3800)
    	{
    		error_flag = 1;
    	}
    	else
    	{
    		TKY_SetCurQueueBaseLine(i, chx_mean);
    	}
    	CH_LOGI(TAG,"queue : %d ch : %d , mean : %d",i,my_tky_ch_init[i].channelNum,chx_mean);

    }
    //��ŵ����ֵ�쳣������У׼����ֵ
    if(error_flag != 0)
    {
    	touch_GPIOSleep();  //�������д���pin��
        CH_LOGI(TAG,"Charging parameters error, preparing for recalibration ...");
        uint8_t charge_time;
        for (uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++) {       //���������������ADCͨ��ת��
          charge_time = 0,chx_mean = 0;
          touch_IOSetAdcState(my_tky_ch_init[i].channelNum);
          while (1)
          {
              chx_mean = TKY_GetCurChannelMean(my_tky_ch_init[i].channelNum, charge_time,3, 1000);

//              CH_LOGI(TAG,"testing .... chg : %d, baseline : %d",charge_time,chx_mean);//��ӡ����ֵ

              if ((charge_time == 0) && ((chx_mean > 3800))) {//������С������
                  CH_LOGI(TAG,"Error, %u KEY%u Too small Cap,Please check the hardware !\r",chx_mean,i);
                  break;
              }
              else {
                  if ((chx_mean > 3200) &&(chx_mean < 3800)) {//����������
                      TKY_SetCurQueueBaseLine(i, chx_mean);
                      TKY_SetCurQueueChargeTime(i,charge_time,3);
                      CH_LOGI(TAG,"channel:%u, chargetime:%u,BaseLine:%u\r",
                            i, charge_time, chx_mean);
                      break;
                  }else if(chx_mean >= 3800)
                  {
                	  TKY_SetCurQueueBaseLine(i, TKY_GetCurChannelMean(my_tky_ch_init[i].channelNum, charge_time-1,3, 20));
                	  TKY_SetCurQueueChargeTime(i,charge_time-1,3);
                	  CH_LOGI(TAG,"Warning,channel:%u Too large Current, chargetime:%u,BaseLine:%u\r",
                	                              i, charge_time, chx_mean);
                	  break;
                  }
                  charge_time++;
                  if (charge_time > 0x1f) {    //������������
                      CH_LOGI(TAG,"Error, Chargetime Max,KEY%u Too large Cap,Please check the hardware !\r",i);
                      break;
                  }
              }
          }
          touch_SingleChDischarge(my_tky_ch_init[i].channelNum);
//          GPIOA_ModeCfg(TKY_Pin[my_tky_ch_init[i].channelNum],GPIO_ModeIN_Floating);
        }
    }
#endif
#if (TKY_FILTER_MODE == FILTER_MODE_9) ||(TKY_FILTER_MODE == FILTER_MODE_7)
#if TKY_SHIELD_EN
    TKY_ChannelInitTypeDef TKY_ChannelInitStructure = {0};
	//--------�Դ���ͨ��0���г�ʼ��������Ϊ�������е�13λ----------
	TKY_ChannelInitStructure.queueNum = 12;
	TKY_ChannelInitStructure.channelNum = 0;
	TKY_ChannelInitStructure.threshold = 40; //---�ż���ֵ��PCB����أ������ʵ���������---
	TKY_ChannelInitStructure.threshold2 = 30;
	TKY_ChannelInitStructure.sleepStatus = 1;
	TKY_ChannelInitStructure.baseLine = 600;
	TKY_CHInit( TKY_ChannelInitStructure );
#endif
    //�˲���9ģʽ����Ҫʹ�õ�������У׼����ֵ
    TKY_CaliCrowedModBaseLine(0, 1000);
    for (uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++)
    {
#if(TKY_FILTER_MODE == FILTER_MODE_7)
    	TKY_SetCurQueueThreshold2(i, my_tky_ch_init[i].threshold, my_tky_ch_init[i].threshold2);
#elif(TKY_FILTER_MODE == FILTER_MODE_9)
    	TKY_SetCurQueueThreshold(i, my_tky_ch_init[i].threshold, my_tky_ch_init[i].threshold2);
#endif
		CH_LOGI(TAG,"%u  key:%u -> thresholdUp:%u;  thresholdDown:%u;\r",TKY_GetCurQueueBaseLine(i),i,
			   my_tky_ch_init[i].threshold, my_tky_ch_init[i].threshold2);
    }
#endif
    TKY_SaveAndStop();
}

/********************************************************************************************************
 * @fn      touch_DetectWheelSlider
 * @brief   �����������ݴ���
 * @param   ��
 * @return  ��
 */
uint16_t touch_DetectWheelSlider(void)
{
	uint8_t  loop;
	uint8_t  max_data_num;
	uint16_t d1;
	uint16_t d2;
	uint16_t d3;
	uint16_t wheel_rpos;
	uint16_t dsum;
	int16_t dval;
	uint16_t unit;
	uint16_t wheel_data[TOUCH_WHEEL_ELEMENTS] = {0};
	uint8_t num_elements=TOUCH_WHEEL_ELEMENTS;
	uint16_t p_threshold = 60;

	if (num_elements < 3)
	{
		return 0;
	}

	for (loop = 0; loop < num_elements; loop++)
	{
		dval = TKY_GetCurQueueValue( loop );
		if(dval>0)
		{
			wheel_data[ loop ] = (uint16_t)dval;
		}
		else {
			wheel_data[ loop ] = 0;
		}
	}
	/* Search max data in slider */
	max_data_num = 0;
	for (loop = 0; loop < (num_elements - 1); loop++)
	{
		if (wheel_data[max_data_num] < wheel_data[loop + 1])
		{
			max_data_num = (uint8_t) (loop + 1);
		}
	}
	/* Array making for wheel operation          */
	/*    Maximum change CH_No -----> Array"0"    */
	/*    Maximum change CH_No + 1 -> Array"2"    */
	/*    Maximum change CH_No - 1 -> Array"1"    */
	if (0 == max_data_num)
	{
		d1 = (uint16_t) (wheel_data[0] - wheel_data[num_elements - 1]);
		d2 = (uint16_t) (wheel_data[0] - wheel_data[1]);
		dsum = (uint16_t) (wheel_data[0] + wheel_data[1] + wheel_data[num_elements - 1]);
	}
	else if ((num_elements - 1) == max_data_num)
	{
		d1 = (uint16_t) (wheel_data[num_elements - 1] - wheel_data[num_elements - 2]);
		d2 = (uint16_t) (wheel_data[num_elements - 1] - wheel_data[0]);
		dsum = (uint16_t) (wheel_data[0] + wheel_data[num_elements - 2] + wheel_data[num_elements - 1]);
	}
	else
	{
		d1 = (uint16_t) (wheel_data[max_data_num] - wheel_data[max_data_num - 1]);
		d2 = (uint16_t) (wheel_data[max_data_num] - wheel_data[max_data_num + 1]);
		dsum = (uint16_t) (wheel_data[max_data_num + 1] + wheel_data[max_data_num] + wheel_data[max_data_num - 1]);
	}

	if (0 == d1)
	    {
	        d1 = 1;
	    }
	    /* Constant decision for operation of angle of wheel    */
	    if (dsum > p_threshold)
	    {
	        d3 = (uint16_t) (TOUCH_DECIMAL_POINT_PRECISION + ((d2 * TOUCH_DECIMAL_POINT_PRECISION) / d1));

	        unit       = (uint16_t) (TOUCH_WHEEL_RESOLUTION / num_elements);
	        wheel_rpos = (uint16_t) (((unit * TOUCH_DECIMAL_POINT_PRECISION) / d3) + (unit * max_data_num));

	        /* Angle division output */
	        /* diff_angle_ch = 0 -> 359 ------ diff_angle_ch output 1 to 360 */
	        if (0 == wheel_rpos)
	        {
	            wheel_rpos = TOUCH_WHEEL_RESOLUTION;
	        }
	        else if ((TOUCH_WHEEL_RESOLUTION + 1) < wheel_rpos)
	        {
	            wheel_rpos = 1;
	        }
	        else
	        {
	            /* Do Nothing */
	        }
	    }
	    else
	    {
	        wheel_rpos = TOUCH_OFF_VALUE;
	    }

	return wheel_rpos;
}
