#include "CH59x_common.h"
#include <stdbool.h>
#include "ch_log.h"

#define DevEP0SIZE    0x40
#define MAX_PACKET_SIZE 64

#define TAG "APP/USB"

volatile bool terminal_connected = false;
volatile bool terminal_ctrl = false;
// �豸������
const uint8_t MyDevDescr[] = 
{ 
  
  0x12,
  0x01,
  0x10,
  0x01,
  0x02,
  0x00,
  0x00,
  0x40,
  0xb0, 0xb1,
  0x55, 0x80,
  0x00, 0x30,
  0x01,                 //�����ߵ��ַ���������������ֵ
  0x02,                 //��Ʒ���ַ���������������ֵ
  0x03,                 //��ŵ��ַ���������������ֵ
  0x01                  //�������õ���Ŀ
                             
};
// ����������
const uint8_t MyCfgDescr[] = 
{ 
  0x09,0x02,0x43,0x00,0x02,0x01,0x00,0x80,0x30,

  //����Ϊ�ӿ�0��CDC�ӿڣ�������
  0x09, 0x04,0x00,0x00,0x01,0x02,0x02,0x01,0x00,

  0x05,0x24,0x00,0x10,0x01,
  0x04,0x24,0x02,0x02,
  0x05,0x24,0x06,0x00,0x01,
  0x05,0x24,0x01,0x01,0x00,

  0x07,0x05,0x84,0x03,0x08,0x00,0x01,                       //�ж��ϴ��˵�������

  //����Ϊ�ӿ�1�����ݽӿڣ�������
  0x09,0x04,0x01,0x00,0x02,0x0a,0x00,0x00,0x00,

  0x07,0x05,0x01,0x02,0x40,0x00,0x00,                       //�˵�������
  0x07,0x05,0x81,0x02,0x40,0x00,0x00,                       //�˵�������
                             
};
// ����������
const uint8_t MyLangDescr[] = {0x04, 0x03, 0x09, 0x04};
// ������Ϣ
const uint8_t MyManuInfo[] = {28, 0x03, 'Y', 0, 'U', 0, 'D', 0, 'U', 0, '-', 0, 'R', 0,'O', 0,'B', 0,'O', 0,'T', 0,'I', 0,'C', 0,'S', 0};
// ��Ʒ��Ϣ
const uint8_t MyProdInfo[] = {14, 0x03, 'U', 0, 'S', 0, 'B', 0, 'L', 0, 'O', 0, 'G', 0};


const UINT8 USB_DEV_PARA_CDC_SERIAL_STR[] = "WCH121212TS1";
const UINT8 USB_DEV_PARA_CDC_PRODUCT_STR[] = "USB2.0 To Serial Port";
const UINT8 USB_DEV_PARA_CDC_MANUFACTURE_STR[] = "YUDU-ROBOTICS";
const UINT8 USB_DEV_PARA_VEN_SERIAL_STR[] = "CH-STACK";
const UINT8 USB_DEV_PARA_VEN_PRODUCT_STR[] = "USB2.0 To Serial Port";
const UINT8 USB_DEV_PARA_VEN_MANUFACTURE_STR[] = "YUDU-ROBOTICS";

/**********************************************************/
uint8_t        DevConfig;
uint8_t        SetupReqCode;
uint16_t       SetupReqLen;
const uint8_t *pDescr;

/******** �û��Զ������˵�RAM ****************************************/
__attribute__((aligned(4))) uint8_t EP0_Databuf[64 + 64 + 64]; //ep0(64)+ep4_out(64)+ep4_in(64)
__attribute__((aligned(4))) uint8_t EP1_Databuf[64 + 64];      //ep1_out(64)+ep1_in(64)
__attribute__((aligned(4))) uint8_t EP2_Databuf[64 + 64];      //ep2_out(64)+ep2_in(64)
__attribute__((aligned(4))) uint8_t EP3_Databuf[64 + 64];      //ep3_out(64)+ep3_in(64)

/*********************************************************************
 * @fn      USB_DevTransProcess
 *
 * @brief   USB ���䴦������
 *
 * @return  none
 */
void USB_DevTransProcess(void)
{
    uint8_t len, chtype;
    uint8_t intflag, errflag = 0;

    intflag = R8_USB_INT_FG;
    if(intflag & RB_UIF_TRANSFER)
    {
        if((R8_USB_INT_ST & MASK_UIS_TOKEN) != MASK_UIS_TOKEN) // �ǿ���
        {
            switch(R8_USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
            {
                case UIS_TOKEN_IN:
                {
                    switch(SetupReqCode)
                    {
                        case USB_GET_DESCRIPTOR:
                            len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen;
                            memcpy(pEP0_DataBuf, pDescr, len);
                            SetupReqLen -= len;
                            pDescr += len;
                            R8_UEP0_T_LEN = len;
                            R8_UEP0_CTRL ^= RB_UEP_T_TOG; // ��ת
                            break;
                        case USB_SET_ADDRESS:
                            R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | SetupReqLen;
                            R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            break;
                        default:
                            R8_UEP0_T_LEN = 0;
                            R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            break;
                    }
                }
                break;

                case UIS_TOKEN_OUT:
                {
                    len = R8_USB_RX_LEN;
                    terminal_connected = terminal_ctrl;
                }
                break;

                case UIS_TOKEN_OUT | 1:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    {
                        R8_UEP1_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        // DevEP1_OUT_Deal(len);
                        CH_LOGI(TAG,"ENDPOINT 1");
                        CH_LOG_BUFFER_HEX(TAG,pEP1_OUT_DataBuf,len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 1:
                    R8_UEP1_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                case UIS_TOKEN_OUT | 2:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    { // ��ͬ�������ݰ�������
                        R8_UEP2_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        // DevEP2_OUT_Deal(len);
                        CH_LOGI(TAG,"ENDPOINT 2");
                        CH_LOG_BUFFER_HEX(TAG,pEP2_OUT_DataBuf,len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 2:
                    R8_UEP2_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                case UIS_TOKEN_OUT | 3:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    { // ��ͬ�������ݰ�������
                        R8_UEP3_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        // DevEP3_OUT_Deal(len);
                        CH_LOGI(TAG,"ENDPOINT 3");
                        CH_LOG_BUFFER_HEX(TAG,pEP3_OUT_DataBuf,len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 3:
                    R8_UEP3_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                case UIS_TOKEN_OUT | 4:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    {
                        R8_UEP4_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        DevEP4_OUT_Deal(len);
                        // CH_LOGI(TAG,"ENDPOINT 4");
                        // CH_LOG_BUFFER_HEX(TAG,pEP4_OUT_DataBuf,len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 4:
                    R8_UEP4_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP4_CTRL = (R8_UEP4_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                default:
                    break;
            }
            R8_USB_INT_FG = RB_UIF_TRANSFER;
        }

        if(R8_USB_INT_ST & RB_UIS_SETUP_ACT) // Setup������
        {
            R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
            SetupReqLen = pSetupReqPak->wLength;
            SetupReqCode = pSetupReqPak->bRequest;
            chtype = pSetupReqPak->bRequestType;

            len = 0;
            errflag = 0;
            if((pSetupReqPak->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
            {
                errflag = 0xFF; /* �Ǳ�׼���� */
            }
            else /* ��׼���� */
            {
                switch(SetupReqCode)
                {
                    case USB_GET_DESCRIPTOR:
                    {
                        switch(((pSetupReqPak->wValue) >> 8))
                        {
                            case USB_DESCR_TYP_DEVICE:
                            {
                                pDescr = MyDevDescr;
                                len = MyDevDescr[0];
                            }
                            break;

                            case USB_DESCR_TYP_CONFIG:
                            {
                                pDescr = MyCfgDescr;
                                len = MyCfgDescr[2];
                            }
                            break;

                            case USB_DESCR_TYP_STRING:
                            {
                                switch((pSetupReqPak->wValue) & 0xff)
                                {
                                    case 1:
                                        pDescr = MyManuInfo;
                                        len = MyManuInfo[0];
                                        break;
                                    case 2:
                                        pDescr = MyProdInfo;
                                        len = MyProdInfo[0];
                                        break;
                                    case 0:
                                        pDescr = MyLangDescr;
                                        len = MyLangDescr[0];
                                        break;
                                    default:
                                        errflag = 0xFF; // ��֧�ֵ��ַ���������
                                        break;
                                }
                            }
                            break;

                            default:
                                errflag = 0xff;
                                break;
                        }
                        if(SetupReqLen > len)
                            SetupReqLen = len; //ʵ�����ϴ��ܳ���
                        len = (SetupReqLen >= DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                        memcpy(pEP0_DataBuf, pDescr, len);
                        pDescr += len;
                    }
                    break;

                    case USB_SET_ADDRESS:
                        SetupReqLen = (pSetupReqPak->wValue) & 0xff;
                        break;

                    case USB_GET_CONFIGURATION:
                        pEP0_DataBuf[0] = DevConfig;
                        if(SetupReqLen > 1)
                            SetupReqLen = 1;
                        break;

                    case USB_SET_CONFIGURATION:
                        DevConfig = (pSetupReqPak->wValue) & 0xff;
                        break;

                    case USB_CLEAR_FEATURE:
                    {
                        if((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) // �˵�
                        {
                            switch((pSetupReqPak->wIndex) & 0xff)
                            {
                                case 0x82:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                                    break;
                                case 0x02:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                                    break;
                                case 0x81:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                                    break;
                                case 0x01:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                                    break;
                                default:
                                    errflag = 0xFF; // ��֧�ֵĶ˵�
                                    break;
                            }
                        }
                        else
                            errflag = 0xFF;
                    }
                    break;

                    case USB_GET_INTERFACE:
                        pEP0_DataBuf[0] = 0x00;
                        if(SetupReqLen > 1)
                            SetupReqLen = 1;
                        break;

                    case USB_GET_STATUS:
                        pEP0_DataBuf[0] = 0x00;
                        pEP0_DataBuf[1] = 0x00;
                        if(SetupReqLen > 2)
                            SetupReqLen = 2;
                        break;

                    default:
                        errflag = 0xff;
                        break;
                }
            }
            if(errflag == 0xff) // �����֧��
            {
                //                  SetupReqCode = 0xFF;
                R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
            }
            else
            {
                if(chtype & 0x80) // �ϴ�
                {
                    len = (SetupReqLen > DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                    SetupReqLen -= len;
                }
                else
                    len = 0; // �´�
                R8_UEP0_T_LEN = len;
                R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // Ĭ�����ݰ���DATA1
            }

            R8_USB_INT_FG = RB_UIF_TRANSFER;
        }
    }
    else if(intflag & RB_UIF_BUS_RST)
    {
        R8_USB_DEV_AD = 0;
        R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_USB_INT_FG = RB_UIF_BUS_RST;
    }
    else if(intflag & RB_UIF_SUSPEND)
    {
        if(R8_USB_MIS_ST & RB_UMS_SUSPEND)
        {
            ;
        } // ����
        else
        {
            ;
        } // ����
        R8_USB_INT_FG = RB_UIF_SUSPEND;
    }
    else
    {
        R8_USB_INT_FG = intflag;
    }
}


/*********************************************************************
 * @fn      DevEP1_OUT_Deal
 *
 * @brief   �˵�1���ݴ���
 *
 * @return  none
 */
void DevEP1_OUT_Deal(uint8_t l)
{ /* �û����Զ��� */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP1_IN_DataBuf[i] = pEP1_OUT_DataBuf[i];
    }
    DevEP1_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP2_OUT_Deal
 *
 * @brief   �˵�2���ݴ���
 *
 * @return  none
 */
void DevEP2_OUT_Deal(uint8_t l)
{ /* �û����Զ��� */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP2_IN_DataBuf[i] = pEP2_OUT_DataBuf[i];
    }
    DevEP2_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP3_OUT_Deal
 *
 * @brief   �˵�3���ݴ���
 *
 * @return  none
 */
void DevEP3_OUT_Deal(uint8_t l)
{ /* �û����Զ��� */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP3_IN_DataBuf[i] = pEP3_OUT_DataBuf[i];
    }
    DevEP3_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP4_OUT_Deal
 *
 * @brief   �˵�4���ݴ���
 *
 * @return  none
 */
void DevEP4_OUT_Deal(uint8_t l)
{ /* �û����Զ��� */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP4_IN_DataBuf[i] = pEP4_OUT_DataBuf[i];
    }
    DevEP4_IN_Deal(l);
}

/*********************************************************************
 * @fn      USB_IRQHandler
 *
 * @brief   USB�жϺ���
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void USB_IRQHandler(void) /* USB�жϷ������,ʹ�üĴ�����1 */
{
    USB_DevTransProcess();
}


UINT8 usb_send_data(UINT8 *p_send_dat, UINT16 send_len)
{
  UINT8 sta = 0;
    for(int i = 0; i < send_len; i++)
    {
        pEP1_IN_DataBuf[i] = p_send_dat[i];
    }
    DevEP1_IN_Deal(send_len);
  return send_len;
}

void usb_write_buffer(const char *str, size_t len)
{

  while (len) {
    size_t l = len;
    if (l > MAX_PACKET_SIZE) {
      l = MAX_PACKET_SIZE;
    }
    // if (!terminal_connected) {
    //     return;
    // }
    l = usb_send_data((UINT8 *)str, l);
    str += l;
    len -= l;
  }
}


void app_usb_init()
{
  pEP0_RAM_Addr = EP0_Databuf;
  pEP1_RAM_Addr = EP1_Databuf;
  pEP2_RAM_Addr = EP2_Databuf;
  pEP3_RAM_Addr = EP3_Databuf;
  USB_DeviceInit();
  PFIC_EnableIRQ(USB_IRQn);
}