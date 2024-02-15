/*
CAN Messages defined byMJ_Parvardeh for imaginary comunication with CAN. Messages are compatible with SAE j1939 ID assignment
created by MJParvardeh
*/

#ifndef    CAN__J1939_MSG_H
#define    CAN__J1939_MSG_H

#include <stdint.h>

#define CMSIS_OS                    TRUE


typedef union
{
    uint32_t             J1939_PGN_Value;    
    struct
    {
        uint8_t         Extended_DataPage               :1;            /* These three bits are used to optimize message latency for transmission onto the bus only. */
        uint8_t         DataPage                        :1;            /* The Data Page Bit is used in conjunction with the Extended Data Page bit to determine the structure of the CAN identifier of the CAN data frame. */
        uint8_t         PDU_Format                      :8;            /* The PDU Format is an 8-bit field that determines the PDU format and is one of the fields used to determine the Parameter Group Number assigned to the data field.  */            
        uint8_t         PDU_Specific                    :8;            /* The PDU Specific is based on PDU Format. It can be Destination Address or group extension parameter*/        
    } J1939_PGN_s;
} J1939_PGN_t;

/*
 *
 * */
typedef union
{
    uint32_t            J1939_ID_Value;
    struct
    {
        uint8_t        Priority        :3;             /* These three bits are used to optimize message latency for transmission onto the bus only. */
        uint32_t       PGN_Value       :18;            /* Whenever it is necessary to identify a Parameter Group in the data field of a CAN data frame. */
        uint8_t        Source_Address  :8;             /* There shall only be one device on the network with a given source address. */
    } J1939_ID_s;
} J1939_ID_t;

typedef struct
{
    J1939_ID_t         ID;                /* Define ID of J1939 which consist They are priority, extended data page, data page, PDU format, PDU specific (which can be a destination address, group extension, or proprietary), source address. */
    uint8_t            SPN[8];            /* Suspect Parameter Number */
    uint8_t            DLC;               /* CAN data Length */
} J1939_CAN_t;


typedef union
{
    uint8_t                J1939_Simple_Value[10];
    struct
    {
        uint8_t                SPN[8];    /* Suspect Parameter Number */
        uint8_t                PDUS;       /* we use only PDUS in simple version of J1939 ID because other parts of ID(PGN) are the same for our messages. other parts will be chhecked by the filters*/        
        uint8_t                DLC;       /* CAN data Length */
    } J1939_Simple_s;     
} J1939_CAN_Simple_t;








/*
CAN Filtering
list ID =10ff
1111 1111 1111 1XXX X111 1111 1111 =0xFFF87FF
Reference ID=  0x18FF1180
*/
//part1: functions for filter 
#define FILTER_REG(ID,IDE,RTR)             ((ID<<3)|IDE|RTR<<1)
#define FILTER_REGH(REG32)                 ((REG32)/0x10000) 
#define FILTER_REGL(REG32)                 ((REG32)%0x10000)
#define LIST0H_REGISTER                    FILTER_REGH(FILTER_REG(0x18FF1080,CAN_ID_EXT,CAN_RTR_DATA))
#define LIST0L_REGISTER                    FILTER_REGL(FILTER_REG(0x18FF1080,CAN_ID_EXT,CAN_RTR_DATA))
#define MASKH_REGISTER                     FILTER_REGH(FILTER_REG(0xFFFFF0FF,CAN_ID_EXT,CAN_RTR_DATA))
#define MASKL_REGISTER                     FILTER_REGL(FILTER_REG(0xFFFFF0FF,CAN_ID_EXT,CAN_RTR_DATA))
#define IDH_REGISTER                       FILTER_REGH(FILTER_REG(0x18FF1180,CAN_ID_EXT,CAN_RTR_DATA))
#define IDL_REGISTER                       FILTER_REGL(FILTER_REG(0x18FF1180,CAN_ID_EXT,CAN_RTR_DATA))
//part2: Direct configuration
//#define LIST0H_REGISTER                    0xC7F8
//#define LIST0L_REGISTER                    0xAC04
//#define MASKH_REGISTER                     0xFFFF
//#define MASKL_REGISTER                     0x87FF
//#define IDH_REGISTER                       0xC7F8
//#define IDL_REGISTER                       0x8404
/*
//Message Transmission Cylcles 
*/
#define LOW_SPEED_CYCLE                    100
#define HIGH_SPEED_CYCLE                   20
/*
//SAE j1939   definitions 
*/
#define EDP                                0
#define DP                                 0
#define PDF                                0xFF
#define DEFAULT_PRIRITY                    6
#define HIGH_PRIRITY                       3
#define DP                                 0
#define PDF                                0xFF
/*
Source Addresses
*/
#define ECU1_ADD                           0x82
#define ECU2_ADD                           0x80

/*
Variable value definitions
*/
/*
boolean
*/
#define TRUE                               1
#define FALSE                              0 


/*
Direct Message ID definitions
we used broadcasting Format(extended group)
*/
/*
typedef enum 
{
    //ECU1_TX_IDs(can be extended upto 0x18FF0F82)
    ECU1_Highpriority_PARAM_MID              = 0x18FF0082,
    ECU1_Lowpriority_PARAM_MID               = 0x18FF0182,

    //ECU1_RX_IDs(ECU2_TX)(can be extended upto 0x18FF1F82)    
    ECU2_Highpriority_PARAM_MID              = 0x18FF1080,
    ECU2_Lowpriority_PARAM_MID               = 0x18FF1180,

    //other ECU Messeges(can be extended upto 0x18FFFF82)   
} MessageID_t;
*/

/*
PDU Specific definition for Messages 
*/
typedef enum 
{
    //ECU1_TX_IDs(can be extended upto 0xF)
    ECU1_Highpriority_PARAM_PDUS                      = 0x00,
    ECU1_Lowpriority_PARAM_PDUS                             ,

    //ECU1_RX_IDs(ECU2_TX)    
    ECU2_Highpriority_PARAM_PDUS                       = 0x10,
    ECU2_Lowpriority_PARAM_PDUS                        
              
} MessagePDUS_t;

/*
Data Length Controller definition for Messages 
*/
typedef enum 
{
    //ECU1_TX_IDs
    ECU1_Highpriority_DLC                                  = 3,
    ECU1_Lowpriority_DLC                           	       = 8,
    //ECU1_RX_IDs(ECU2_TX)    
    ECU2_Highpriority_DLC                                  = 3,
    ECU2_Lowpriority_DLC                                   = 8,   
} MessageDLC_t;



/*
Messages SPN detail definitions
Part1 ECU1 Messages
Source Address 0x82
*/


/*
ECU1 High priority Message with 0x18FF0082 ID
*/

typedef union
{    
    uint8_t                 ECU1_Highpriority_Array[ECU1_Highpriority_DLC];    
    struct 
    {
        uint16_t            ECU1_Highpriority_Parm1             :16;
        uint8_t             ECU1_Highpriority_Parm2             :4;
        uint8_t             ECU1_Highpriority_Parm3             :1;
        uint8_t             ECU1_Highpriority_Parm4             :2;
        uint8_t             ECU1_Highpriority_Parm5             :1;                              
    } ECU1_Highpriority_s;        
    
} ECU1_Highpriority_t;


/*
ECU1 Low priority Message with 0x18FF0182 ID
*/
typedef union
{    
    uint8_t                 ECU1_Lowpriority_Array[ECU1_Lowpriority_DLC];    
    struct
    {
        uint64_t             ECU1_Lowpriority_Parm1            :8;
        uint64_t             ECU1_Lowpriority_Parm2            :8;
        uint64_t             ECU1_Lowpriority_Parm3            :8;
        uint64_t             ECU1_Lowpriority_Parm4            :4;
        uint64_t             ECU1_Lowpriority_Parm5            :4;
        uint64_t             ECU1_Lowpriority_Parm6            :16;
        uint64_t             ECU1_Lowpriority_Parm7            :16;                       
    } ECU1_Lowpriority_s;        
    
} ECU1_Lowpriority_t;

/*
Messages SPN detail definitions
Part2 ECU2 Messages
Source Address 0x80 
*/


/*
ECU2 High priority Message with 0x18FF1080 ID
*/

typedef union
{    
    uint8_t                 ECU2_Highpriority_Array[ECU2_Highpriority_DLC];    
    struct 
    {
        uint16_t            ECU2_Highpriority_Parm1             :16;
        uint8_t             ECU2_Highpriority_Parm2             :4;
        uint8_t             ECU2_Highpriority_Parm3             :1;
        uint8_t             ECU2_Highpriority_Parm4             :2;
        uint8_t             ECU2_Highpriority_Parm5             :1;                              
    } ECU2_Highpriority_s;        
    
} ECU2_Highpriority_t;


/*
ECU2 Low priority Message with 0x18FF1180 ID
*/
typedef union
{    
    uint8_t                 ECU2_Lowpriority_Array[ECU2_Lowpriority_DLC];    
    struct
    {
        uint64_t             ECU2_Lowpriority_Parm1            :8;
        uint64_t             ECU2_Lowpriority_Parm2            :8;
        uint64_t             ECU2_Lowpriority_Parm3            :8;
        uint64_t             ECU2_Lowpriority_Parm4            :4;
        uint64_t             ECU2_Lowpriority_Parm5            :4;
        uint64_t             ECU2_Lowpriority_Parm6            :16;
        uint64_t             ECU2_Lowpriority_Parm7            :16;                       
    } ECU2_Lowpriority_s;        
    
} ECU2_Lowpriority_t;

/*
Timeout for each received message(ECU2)
*/
typedef struct
{
    uint8_t                 CAN_Receiver_Timeout;
    uint8_t                 ECU2_Highpriority_Timeout;
    uint8_t                 ECU2_Lowpriority_Timeout;    
} Messages_Timeout_t;

/*
Messages typedef containing all Messages
*/
typedef struct
{
    /*
    ECU1 Messages
    */
    ECU1_Highpriority_t             ECU1_Highpriority;
    ECU1_Lowpriority_t              ECU1_Lowpriority_t;

    /*
    ECU2 Messages
    */
    ECU2_Highpriority_t             ECU2_Highpriority;
    ECU2_Lowpriority_t              ECU1_Lowpriority;
    /*
    Timeout for received Messages
    */
    Messages_Timeout_t              Messages_Timeout;
} CAN_Messages_t;




/*
Functions prototype
Section1:Architecture based Functions
*/

#ifdef CMSIS_OS
#include "cmsis_os.h"
void CAN_RTOS_Timer_Init(void *param);
void CAN_Receive_Task_Init(void);
void CAN_MSG_Thread(void *param);
#elif FREERTOS_OS
#endif

/*
Functions prototype
Section2:non Architecture based Functions
*/
void SendCAN_Data(uint8_t CAN_DLC, uint32_t CAN_ID, uint8_t *CAN_BUF);
void CAN_MSG_Init(void);
static void CAN_MSG_Filter_Init(void);
static void CAN_MSG_Start_Interrupt(void);
void SendCAN_Data(uint8_t CAN_DLC, uint32_t CAN_ID, uint8_t *CAN_BUF);
void Send_CAN_MSG(uint8_t Message_DLC, uint8_t Message_Source, uint8_t Message_PDU_Specific, uint8_t *Message_Array);
void Send_Message(void);
void Send_Message_Timer_Callback(void *argument);

#endif



