#include "main.h"
#include "stm8s_tim4.h"
#include "stm8s_flash.h"
#include "stm8s_eol.h"
#include "stm8s_Kline.h"

//////  DEBUG ENABLE/DISABLE    ////////
//#define DEBUG
////////////////////////////////////////

/* This below 0x8100 is mandatory for dorman rights checking - Do not change any thing */
at 0x8100 code const unsigned char dorman[58] = {'(','c',')',' ','C','o','p','y','r','i','g','h','t',' ','2','0','1','7',' ','D','o','r','m','a','n',' ', 'P','r','o','d','u','c','t','s',',','I','n','c', '.','A','l','l',' ','R','i','g','h','t','s',' ','R','e','s','e','r','v','e','d'};
at 0x813A code const unsigned char dorman_ver[70] = "#HSCAN and KLINE Ver-2.0.0,Developed by Dorman Products Inc, Colmar#";
at 0x8181 code const unsigned char nose[40] = "#Up your nose with a rubber hose!#";
    
// Variable for Dorman copyright check
uint8_t test_char_t1[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t dorman_flag = 0;

// Function prototypes
static void check_dorman_rights(void);
static void flash_process(void);
static void Program_Sequence(void);

// Variables for CAN configuration
uint8_t CAN_cofig_baud = 2;

// EEPROM Address and variable
uint32_t EEPROM_address_of_dongle_uses = 0x4100;
uint8_t number_of_dongle_uses = 0;

uint8_t programming_step = 0;
uint8_t number_of_remotes_programmed_to_vehicle = 0;

uint8_t keys_learned = 0;

/* CAN Global Transmit Parameters */
CAN_Id_TypeDef Tx_IDE = CAN_Id_Standard;
CAN_RTR_TypeDef Tx_RTR = CAN_RTR_Data;
uint8_t Tx_DLC = 8;
uint8_t Tx_Data[8] = {0};
uint32_t Tx_Id = 0x7E0;

/*******************************************/
// GPIO to blink LED and Beep
// 0x01 - Unused
// 0x02 - Beep
// 0x04 - Green
// 0x08 - Red
/*******************************************/


void main(void)
{
    //Dorman rights checking ------------------------------------*/
    check_dorman_rights();
    
    /* Clock configuration --------------------------------------*/
    CLK_Config();

    /* GPIO Configuration ---------------------------------------*/
    GPIO_Config();

    /* UART configuration ---------------------------------------*/
    UART_Config();

    /*  TIM4 1ms Timer configuration ----------------------------*/
    TIM4->IER  = 0x01;
    TIM4->CNTR = 0x00;
    TIM4->PSCR = 0x06;
    TIM4->ARR  = 0xF9;
    TIM4->SR1  = 0x00;
    TIM4->CR1  = 0x01;

    /* CAN configuration ----------------------------------------*/
    CAN_Config();

    /* Enable Interrupts ----------------------------------------*/
    enableInterrupts();

    /* Configures Flash and checks to see how many fobs have been programmed  */
    flash_process();


#ifndef DEBUG

    if (number_of_dongle_uses <= 8)
    {
        //~ // Trigger buzzer and green LED when the dongle gets power
        //~ GPIOC->ODR = 0x06;   
        //~ wait_time(40);
        //~ GPIOC->ODR = 0x00;
        programming_step = 1;
        
        while(1)
        { 
            if((l_can_receive_flag==1)&&(Tester_Present==0))
            {
                Program_Sequence();
            }
        }
    }
#endif

#ifdef DEBUG
#endif

}

// Dorman copyright check
static void check_dorman_rights(void)
{
    uint8_t check_char[6] = {'E','p','s','n','b','o'};
    test_char_t1[0] = dorman[19];
    test_char_t1[1] = dorman[20];
    test_char_t1[2] = dorman[21];
    test_char_t1[3] = dorman[22];
    test_char_t1[4] = dorman[23];
    test_char_t1[5] = dorman[24];

    if( (dorman[19] == 'D') && (dorman[20] == 'o') && (dorman[21] == 'r') && (dorman[22] == 'm') && (dorman[23] == 'a') && (dorman[24] == 'n') ) 
    {
        dorman_flag = 1;
    }
    else
    {
        halt();
    }

    if( (test_char_t1[0] == (check_char[0]-1)) && (test_char_t1[1] == (check_char[1]-1)) &&
        (test_char_t1[2] == (check_char[2]-1)) && (test_char_t1[3] == (check_char[3]-1)) &&
        (test_char_t1[4] == (check_char[4]-1)) && (test_char_t1[5] == (check_char[5]-1))
      )
    {
        dorman_flag = 2;
    }
    else
    {
        halt();
    }
}


static void flash_process(void)
{
    /* Define FLASH programming time */
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);

    /* Unlock Data memory */
    FLASH_Unlock(FLASH_MEMTYPE_DATA);

    /* read number_of_dongle_uses of programmed keys*/
    number_of_dongle_uses = FLASH_ReadByte(EEPROM_address_of_dongle_uses);
}

/** Configure system clock to run at 16Mhz */
void CLK_Config(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

/** Configures the CAN ******/
void CAN_Config(void)
{
    CAN_InitStatus_TypeDef status = CAN_InitStatus_Failed;

    /* Filter Parameters */
    CAN_FilterNumber_TypeDef CAN_FilterNumber;
    FunctionalState CAN_FilterActivation;
    CAN_FilterMode_TypeDef CAN_FilterMode;
    CAN_FilterScale_TypeDef CAN_FilterScale;
    uint8_t CAN_FilterID1;
    uint8_t CAN_FilterID2;
    uint8_t CAN_FilterID3;
    uint8_t CAN_FilterID4;
    uint8_t CAN_FilterIDMask1;
    uint8_t CAN_FilterIDMask2;
    uint8_t CAN_FilterIDMask3;
    uint8_t CAN_FilterIDMask4; 

    /* Init Parameters*/
    CAN_MasterCtrl_TypeDef CAN_MasterCtrl;
    CAN_Mode_TypeDef CAN_Mode;
    CAN_SynJumpWidth_TypeDef CAN_SynJumpWidth;
    CAN_BitSeg1_TypeDef CAN_BitSeg1;
    CAN_BitSeg2_TypeDef CAN_BitSeg2;
    uint8_t CAN_Prescaler;  

    /* CAN register init */
    CAN_DeInit();

    /* CAN  init */
    CAN_MasterCtrl=CAN_MasterCtrl_AllDisabled;
    CAN_Mode = CAN_Mode_Normal;
    CAN_SynJumpWidth = CAN_SynJumpWidth_1TimeQuantum;
    CAN_BitSeg1 = CAN_BitSeg1_8TimeQuantum;
    CAN_BitSeg2 = CAN_BitSeg2_7TimeQuantum;
    CAN_Prescaler = CAN_cofig_baud; //1-1Mbps, 2-500kbps, 30-33.333kbps
    status = CAN_Init(CAN_MasterCtrl, CAN_Mode, CAN_SynJumpWidth, CAN_BitSeg1, \
        CAN_BitSeg2, CAN_Prescaler);

    /* CAN filter init */
    CAN_FilterNumber = CAN_FilterNumber_0;
    CAN_FilterActivation = ENABLE;
    CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterScale = CAN_FilterScale_32Bit;
    CAN_FilterID1=0;  
    CAN_FilterID2=0;
    CAN_FilterID3=0;
    CAN_FilterID4=0;
    CAN_FilterIDMask1=0;
    CAN_FilterIDMask2=0;
    CAN_FilterIDMask3=0;
    CAN_FilterIDMask4=0;  
    CAN_FilterInit(CAN_FilterNumber, CAN_FilterActivation, CAN_FilterMode, 
        CAN_FilterScale,CAN_FilterID1, CAN_FilterID2, CAN_FilterID3,
        CAN_FilterID4,CAN_FilterIDMask1, CAN_FilterIDMask2, 
        CAN_FilterIDMask3, CAN_FilterIDMask4);
        
    /* Enable Fifo message pending interrupt*/
    /* Message reception is done by CAN_RX ISR*/
    CAN_ITConfig(CAN_IT_FMP, ENABLE);
}


void GPIO_Config(void)
{
    /* LED and Buzzer setup */
    GPIOC->DDR = 0x0E;
    GPIOC->CR1 = 0x0E;
    GPIOC->CR2 = 0x0E;
    GPIOC->ODR = 0x00;

    /* HS CAN setup, disable SW CAN */
    GPIOA->DDR = 0x40;
    GPIOA->CR1 = 0x40;
    GPIOA->CR2 = 0x40;
    GPIOA->ODR = 0x40;   

   /* Kline set up */

    GPIOD->DDR = 0x01;
    GPIOD->CR1 = 0x01;
    GPIOD->ODR = 0x01; // Set SLP N pin to enable LIN Transceive
}

/** Generates a delay time.  */
void Delay (uint16_t nCount)
{
  /* Decrement nCount value */
  for (; nCount != 0; nCount--);
}

void wait_time(uint8_t xcount)
{
	uint8_t l_ct;
	for(l_ct = 0; l_ct <= xcount; l_ct++)
	{
        Delay(0x7FFF);
	}
}

#ifdef USE_FULL_ASSERT
/********************************************************************/
/* Reports the name of the source file and the source line number   */
/* where the assert_param error has occurred.                       */ 
/********************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif


static void Program_Sequence(void)
{ 
    switch(programming_step)
    {
        // Determine how many keys are currently programmed to vehicle
        case 1:
        {
            // Request data by PID for number of keys programmed
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0x22;
            Tx_Data[2] = 0xC1;
            Tx_Data[3] = 0x04;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(5);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
              
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is a negative response
                if(l_can_data[1] == 0x7F)
                {
                    // Change arbitration ID to 720 and resend message
                    Tx_Id = 0x720;
                }
                
                // Check if received message is positive response to request
                else if((l_can_data[1] == 0x62) && (l_can_data[2] == 0xC1) && (l_can_data[3] == 0x04))
                {
                    number_of_remotes_programmed_to_vehicle = l_can_data[4];
                    
                    programming_step++;
                }
                
            }
            
            break;
        }
        
        // Check for valid OEM key
        case 2:
        {
            // Request data by PID for anti-theft status
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0x22;
            Tx_Data[2] = 0xC1;
            Tx_Data[3] = 0x24;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(5);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
              
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0x62) && (l_can_data[2] == 0xC1) && (l_can_data[3] == 0x24))
                {
                    // Check for master key
                    if ((l_can_data[4] & 0x20) == 0x20)
                    {
                        programming_step++;
                        
                        // Trigger buzzer and green LED when the OEM key is authenticated
                        GPIOC->ODR = 0x06;   
                        wait_time(40);
                        GPIOC->ODR = 0x00;
                    }
                }
                
            }
            
            break;
        }
        
        // Check for valid new key
        case 3:
        {
            // Request data by PID for anti-theft status
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0x22;
            Tx_Data[2] = 0xC1;
            Tx_Data[3] = 0x24;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(5);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
              
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0x62) && (l_can_data[2] == 0xC1) && (l_can_data[3] == 0x24))
                {
                    // Check for master key
                    if ((l_can_data[4] & 0x20) == 0x00)
                    {
                        programming_step++;
                        
                        // Trigger buzzer and green LED when the new key is authenticated
                        GPIOC->ODR = 0x06;   
                        wait_time(40);
                        GPIOC->ODR = 0x00;
                    }
                }
                
            }
            
            break;
        }

        // Check vehicle security access status
        case 4:
        {
            // Request data by PID for security access status
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0x22;
            Tx_Data[2] = 0xC1;
            Tx_Data[3] = 0x15;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(5);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
            
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0x62) && (l_can_data[2] == 0xC1) && (l_can_data[3] == 0x15))
                {
                    // Access denied
                    if (l_can_data[4] == 0x33)
                    {
                        programming_step++;
                    }
                    // Access in progress
                    else if (l_can_data[4] == 0x55)
                    {
                        // Continue access
                        programming_step = 7;
                    }
                    // Access granted
                    else if (l_can_data[4] == 0xAA)
                    {
                        // Move to learn key
                        programming_step = 10;
                        
                        // Stop light blinking by disabling counter
                        TIM4->CR1  = 0x00;
                        
                        // Trigger buzzer and green LED
                        GPIOC->ODR = 0x06;   
                        wait_time(40);
                        GPIOC->ODR = 0x00;
                        wait_time(40);
                        GPIOC->ODR = 0x06;   
                        wait_time(40);
                        GPIOC->ODR = 0x00;
                    }
                }
            }
            
            break;
        }
        
        // Initiate diagnostic session
        case 5:
        {
            // Send diagnostic command to start timed access
            Tx_Data[0] = 0x02;
            Tx_Data[1] = 0x10;
            Tx_Data[2] = 0x87;
            Tx_Data[3] = 0x00;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(5);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
            
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0x50) && (l_can_data[2] == 0x87))
                {
                    // PATS 5 system
                    programming_step++;
                }
                else if ((l_can_data[1] == 0x7F) && (l_can_data[2] == 0x10))
                {
                    // PATS 3/4 system
                    programming_step++;
                }
            }
            
            break;
        }
        
        // Initiate timed security access
        case 6:
        {
            // Send diagnostic command to start timed access
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0xB1;
            Tx_Data[2] = 0x01;
            Tx_Data[3] = 0x9C;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(5);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
            
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0xF1) && (l_can_data[2] == 0x01) && (l_can_data[3] == 0x9C))
                {
                    programming_step++;
                    
                    // Trigger buzzer and green LED
                    GPIOC->ODR = 0x06;   
                    wait_time(40);
                    GPIOC->ODR = 0x00;
                    
                    // Start light blinking by enabling counter
                    TIM4->CR1  = 0x01;
                }
            }
            
            break;
        }
        
        // Check security access status
        case 7:
        {
            // Request data by PID for security access status
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0x22;
            Tx_Data[2] = 0xC1;
            Tx_Data[3] = 0x15;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(40);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
            
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0x62) && (l_can_data[2] == 0xC1) && (l_can_data[3] == 0x15))
                {
                    // Access denied
                    if (l_can_data[4] == 0x33)
                    {
                        // Request timed access again
                        programming_step = 2;
                    }
                    // Access in progress
                    else if (l_can_data[4] == 0x55)
                    {
                        // Do nothing
                    }
                    // Access granted
                    else if (l_can_data[4] == 0xAA)
                    {
                        // Move to learn key
                        programming_step = 10;
                    }
                    
                }
            }
            
            break;
        }
        
        // Erase key
        case 10:
        {
            // Send diagnostic command to erase keys
            Tx_Data[0] = 0x04;
            Tx_Data[1] = 0xB1;
            Tx_Data[2] = 0x00;
            Tx_Data[3] = 0x39;
            Tx_Data[4] = 0x01;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(5);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
            
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0xF1) && (l_can_data[2] == 0x00) && (l_can_data[3] == 0x39))
                {
                    programming_step++;
                }
            }
            
            break;
        }
        
        // Determine how many keys are currently programmed to vehicle
        case 11:
        {
            // Request data by PID for number of keys programmed
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0x22;
            Tx_Data[2] = 0xC1;
            Tx_Data[3] = 0x04;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(40);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
              
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0x62) && (l_can_data[2] == 0xC1) && (l_can_data[3] == 0x04))
                {
                    // Check if the new keyfob/remote has been learned
                    if(l_can_data[4] == 0x00)
                    {
                        programming_step++;
                    }
                }
            }
            
            break;
        }
        
        // Set Service Mode
        case 12:
        {
            // Send diagnostic command to set service mode
            Tx_Data[0] = 0x04;
            Tx_Data[1] = 0xB1;
            Tx_Data[2] = 0xF0;
            Tx_Data[3] = 0x10;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(40);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
            
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0xF1) && (l_can_data[2] == 0xF0) && (l_can_data[3] == 0x10))
                {
                    programming_step++;
                    
                    // Stop light blinking by disabling counter
                    TIM4->CR1  = 0x00;
                    
                    // Beep twice to let operator know to cycle key 1
                    GPIOC->ODR = 0x06;   
                    wait_time(40);
                    GPIOC->ODR = 0x00;
                    wait_time(40);
                    GPIOC->ODR = 0x06;   
                    wait_time(40);
                    GPIOC->ODR = 0x00;
                }
            }
            
            break;
        }
        
        // Determine how many keys are currently programmed to vehicle
        case 13:
        {
            // Request data by PID for number of keys programmed
            Tx_Data[0] = 0x03;
            Tx_Data[1] = 0x22;
            Tx_Data[2] = 0xC1;
            Tx_Data[3] = 0x04;
            Tx_Data[4] = 0x00;
            Tx_Data[5] = 0x00;
            Tx_Data[6] = 0x00;
            Tx_Data[7] = 0x00;
            
            wait_time(40);
            CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
            wait_time(2);
              
            // Check for received message
            if (l_can_receive_flag == 1)
            {
                l_can_receive_flag = 0;
                
                // Check if received message is positive response to request
                if((l_can_data[1] == 0x62) && (l_can_data[2] == 0xC1) && (l_can_data[3] == 0x04))
                {
                    // Check if the new keyfob/remote has been learned
                    if(l_can_data[4] == 0x01)
                    {
                        
                        if (keys_learned == 0)
                        {
                            // Write new amount of fobs/remotes learned to EEPROM
                            //FLASH_ProgramByte(EEPROM_address_of_dongle_uses, number_of_dongle_uses + 1);
                            
                            keys_learned = 1;
                            
                            // Beep once for the first key
                            GPIOC->ODR = 0x06;
                            wait_time(40);
                            GPIOC->ODR = 0x00;
                        }
                    }
                    
                    // Check if the new keyfob/remote has been learned
                    else if(l_can_data[4] == 0x02)
                    {
                        if (keys_learned == 1)
                        {
                            // Write new amount of fobs/remotes learned to EEPROM
                            FLASH_ProgramByte(EEPROM_address_of_dongle_uses, number_of_dongle_uses + 1);
                            
                            keys_learned = 2;
                            
                            programming_step++;
                            
                            // Beep three times for the second key
                            GPIOC->ODR = 0x06;   
                            wait_time(40);
                            GPIOC->ODR = 0x00;
                            wait_time(40);
                            GPIOC->ODR = 0x06;   
                            wait_time(40);
                            GPIOC->ODR = 0x00;
                            wait_time(40);
                            GPIOC->ODR = 0x06;   
                            wait_time(40);
                            GPIOC->ODR = 0x00;
                        }
                    }
                }
            }
            
            break;
        }
        
        default:
        {
            1;
        }
    }
}
