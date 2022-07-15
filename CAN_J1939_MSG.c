#include "CAN_MSG.h"
#include "main.h"

CAN_Messages_t  CAN_Messages;
uint32_t Undefined_IDs_Number;
/*
Section1:Architecture based Functions
*/
#ifdef CMSIS_OS
#include "cmsis_os.h"
/*
CAN RTOS software timer for transmission 
*/
osTimerId_t Send_MSG_Timer_id;
const osTimerAttr_t Send_MSG_Timer_attributes = 
{
    .name = "Send_MSG_Timer"
};



void CAN_RTOS_Timer_Init(void *param)
{
    Send_MSG_Timer_id = osTimerNew(Send_Message_Timer_Callback, osTimerPeriodic, NULL, &Send_MSG_Timer_attributes);
    if (Send_MSG_Timer_id != NULL)  
    {
        //periodic timer created
        osTimerStart (Send_MSG_Timer_id, HIGH_SPEED_CYCLE);
    }             
}
/*
CAN RTOS Receiver Task and RTOS related definitions 
*/

const uint8_t        CAN_MSG_REC_Count                  = 11; /* overall */
const uint8_t        CAN_MSG_REC_Size                   = sizeof(J1939_CAN_Simple_t);
osMessageQueueId_t   CAN_MSG_Receiver_QueueID;
osThreadId_t         CAN_MSG_Receiver_Task;
static uint64_t      CAN_MSG_Receiver_Stack[8];
const osThreadAttr_t CAN_MSG_Receiver_Task_ATTR =
{ 
    .stack_mem  =    &CAN_MSG_Receiver_Stack[0],
    .stack_size =    sizeof(CAN_MSG_Receiver_Stack),
    .priority   =    osPriorityLow2    
};
void CAN_Receive_Task_Init()
{
    
    CAN_MSG_Receiver_QueueID     = osMessageQueueNew(CAN_MSG_REC_Count, CAN_MSG_REC_Size, NULL);     
    CAN_MSG_Receiver_Task        = osThreadNew(CAN_MSG_Thread, NULL, &CAN_MSG_Receiver_Task_ATTR);
       
};

void CAN_MSG_Thread(void *param)
{
    J1939_CAN_Simple_t CAN_MSG_REC; 
    while(1)
    {
        osMessageQueueGet (CAN_MSG_Receiver_QueueID, CAN_MSG_REC.J1939_Simple_Value,0, osWaitForever);        
        switch(CAN_MSG_REC.J1939_Simple_s.PDUS)
        {
            case ECU2_Highpriority_PARAM_PDUS:
                for(int i=0;i<ECU2_Highpriority_DLC;i++)
                    CAN_Messages.ECU2_Highpriority.ECU2_Highpriority_Array[i]      = CAN_MSG_REC.J1939_Simple_Value[i]; 
                    CAN_Messages.Messages_Timeout.ECU2_Highpriority_Timeout         = 0; 
                break;
            case ECU2_Lowpriority_PARAM_PDUS:
                for(int i=0;i<ECU2_Lowpriority_DLC;i++)
                    CAN_Messages.ECU2_Lowpriority.ECU2_Lowpriority_Array[i]        = CAN_MSG_REC.J1939_Simple_Value[i];  
                    CAN_Messages.Messages_Timeout.ECU2_Lowpriority_Timeout         = 0;              
                break;
            default:
                Undefined_IDs_Number++;     /*for debuging purpose*/
                break;
             
        }
        //write function for updating values to 15118 standard values        
    }    
}

#elif FREERTOS_OS


#endif


#ifdef STM32F407xx
#include "stm32f4xx_hal_can.h"

extern CAN_HandleTypeDef hcan1;
#define MSG_CAN    hcan1

uint8_t REC_MSG[10];

//void RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//    CAN_RxHeaderTypeDef RxHeader; 
//    J1939_CAN_Simple_t  MSG_CAN_REC; 
//    J1939_ID_t  J1939_ID;
//    if(HAL_CAN_GetRxMessage(&MSG_CAN, CAN_RX_FIFO0, &RxHeader, MSG_CAN_REC.J1939_Simple_s.SPN)== HAL_OK) 
//    {
//        J1939_ID.J1939_ID_Value         = RxHeader.ExtId;
//        MSG_CAN_REC.J1939_Simple_s.PDUS = (J1939_ID.J1939_ID_s.PGN_Value)%256;  /* Lowest 8 bits */ 
//        MSG_CAN_REC.J1939_Simple_s.DLC  = RxHeader.DLC; 
//        osMessageQueuePut (CAN_MSG_Receiver_QueueID ,MSG_CAN_REC.J1939_Simple_Value, 0, 0);
//    }
//}

//->->added for OSless test
void RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader; 
    J1939_CAN_Simple_t  CAN_MSG_REC; 
    J1939_ID_t  J1939_ID;
    if(HAL_CAN_GetRxMessage(&MSG_CAN, CAN_RX_FIFO0, &RxHeader, CAN_MSG_REC.J1939_Simple_s.SPN)== HAL_OK) 
    {
        J1939_ID.J1939_ID_Value         = RxHeader.ExtId;
        CAN_MSG_REC.J1939_Simple_s.PDUS = (J1939_ID.J1939_ID_s.PGN_Value)%256;  /* Lowest 8 bits */ 
        CAN_MSG_REC.J1939_Simple_s.DLC  = RxHeader.DLC;  
        switch(CAN_MSG_REC.J1939_Simple_s.PDUS)
        {
            case ECU2_Highpriority_PARAM_PDUS:
                for(int i=0;i<ECU2_Highpriority_DLC;i++)
                    CAN_Messages.ECU2_Highpriority.ECU2_Highpriority_Array[i]      = CAN_MSG_REC.J1939_Simple_Value[i]; 
                    CAN_Messages.Messages_Timeout.ECU2_Highpriority_Timeout         = 0; 
                break;
            case ECU2_Lowpriority_PARAM_PDUS:
                for(int i=0;i<ECU2_Lowpriority_DLC;i++)
                    CAN_Messages.ECU2_Lowpriority.ECU2_Lowpriority_Array[i]        = CAN_MSG_REC.J1939_Simple_Value[i];  
                    CAN_Messages.Messages_Timeout.ECU2_Lowpriority_Timeout         = 0;              
                break;
            default:
                Undefined_IDs_Number++;     /*for debuging purpose*/
                break;            
        }        
    }
}
//->->added for OSless test
void CAN_MSG_Init(void)
{
          
    MX_CAN1_Init();
    MSG_CAN.RxFifo0MsgPendingCallback = RxFifo0MsgPendingCallback;
    CAN_MSG_Filter_Init();
    CAN_MSG_Start_Interrupt();    
    HAL_CAN_Start(&MSG_CAN);        
}


static void CAN_MSG_Filter_Init()
{
    /*
    Design FIFO0 for Highpriority Messages List
    ADD List Mode filter for Bank0
    */
    CAN_FilterTypeDef MSG_CAN_Filter;
    MSG_CAN_Filter.FilterActivation     = CAN_FILTER_ENABLE;
    MSG_CAN_Filter.FilterBank           = 0;
    MSG_CAN_Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    MSG_CAN_Filter.FilterMode           = CAN_FILTERMODE_IDLIST;
    MSG_CAN_Filter.FilterIdHigh         = LIST0H_REGISTER;
    MSG_CAN_Filter.FilterIdLow          = LIST0L_REGISTER;  
    MSG_CAN_Filter.FilterMaskIdHigh     = LIST0H_REGISTER;   
    MSG_CAN_Filter.FilterMaskIdLow      = LIST0L_REGISTER;   
    MSG_CAN_Filter.FilterScale          = CAN_FILTERSCALE_32BIT;
    HAL_CAN_ConfigFilter(&MSG_CAN, &MSG_CAN_Filter);
    /*
    Design FIFO1 for Lowpriority Messages List
    ADD Mask Mode filter for Bank1
    IDs between 0x18ff1080 and 0x18ff1180 are used
    IDs between 0x18ff1180 and 0x18ff1F80 and 0x18ff1380 are Reserved
    */    
    MSG_CAN_Filter.FilterActivation     = CAN_FILTER_ENABLE;
    MSG_CAN_Filter.FilterBank           = 1;
    MSG_CAN_Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    MSG_CAN_Filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    MSG_CAN_Filter.FilterIdHigh         = IDH_REGISTER;
    MSG_CAN_Filter.FilterIdLow          = IDL_REGISTER;  
    MSG_CAN_Filter.FilterMaskIdHigh     = MASKH_REGISTER;   
    MSG_CAN_Filter.FilterMaskIdLow      = MASKL_REGISTER;   
    MSG_CAN_Filter.FilterScale          = CAN_FILTERSCALE_32BIT;
    HAL_CAN_ConfigFilter(&MSG_CAN, &MSG_CAN_Filter);        
}

static void CAN_MSG_Start_Interrupt()
{    
    if(HAL_CAN_ActivateNotification(&MSG_CAN, CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
        HAL_GPIO_WritePin(LD7_GPIO_Port,LD7_Pin,GPIO_PIN_SET|CAN_IT_BUSOFF);
}

void SendCAN_Data(uint8_t CAN_DLC, uint32_t CAN_ID, uint8_t *CAN_BUF)
{
    uint32_t TxMailboxes;
    uint32_t timeout;    
    CAN_TxHeaderTypeDef CAN_MSGHeader;
    CAN_MSGHeader.DLC= CAN_DLC;
    CAN_MSGHeader.RTR= CAN_RTR_DATA;
    CAN_MSGHeader.IDE= CAN_ID_EXT;
    CAN_MSGHeader.ExtId= CAN_ID;
    timeout= SysTick->VAL;    
    while((HAL_CAN_IsTxMessagePending(&MSG_CAN,TxMailboxes)) && ((SysTick->VAL)-timeout<200));    /*Check if previous data sending is finished*/      
    HAL_CAN_AddTxMessage(&MSG_CAN, &CAN_MSGHeader, CAN_BUF, &TxMailboxes);    
}
//following elif for future Devices. use same function name for non static functions
#elif OtherArchitecture
#endif


void Send_CAN_MSG(uint8_t Message_DLC, uint8_t Message_Source, uint8_t Message_PDU_Specific, uint8_t *Message_Array)
{
    uint32_t PGN = (EDP<<17)|(DP<<16)|(PDF<<8)|(Message_PDU_Specific);
    uint32_t ID  = (DEFAULT_PRIRITY<<26)|(PGN<<8)|Message_Source;
    SendCAN_Data(Message_DLC,ID, Message_Array);
}


void Send_Message_Timer_Callback(void *argument)
{  
    static uint8_t low_speed_cnt;
    Send_CAN_MSG(ECU1_Highpriority_DLC, ECU1_ADD, ECU1_Highpriority_PDUS, CAN_Messages.ECU1_Highpriority.ECU1_Highpriority_Array);
    low_speed_cnt++;
    if(low_speed_cnt== (LOW_SPEED_CYCLE/HIGH_SPEED_CYCLE))
    {
        Send_CAN_MSG(ECU1_Lowpriority_DLC, ECU1_ADD, ECU1_Lowpriority_PDUS, CAN_Messages.ECU1_Lowpriority.ECU1_Lowpriority_Array);
        low_speed_cnt= 0;
    }                        
}
    

