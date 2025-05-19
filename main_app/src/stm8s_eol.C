/* Includes ------------------------------------------------------------------*/
#include "stm8s_eol.h"
#include "stm8s_can.h"
#include "stm8s_Kline.h"
#include "stm8s_flash.h"

/* Macro Definitions ********************************************************/
#define KLINE_RX_BUF_SIZE 10
/* Macro Definitions ********************************************************/


//Source code Implementation Steps

//variable and prototype declaration
at 0x8240 code const unsigned char date[8]    = {'0','4','0','5','2','0','2','4'};  //MM/DD/YYYY  Apr/05/2024
at 0x8248 code const unsigned char version[8] = "#V-0.50#";     //The string is limited to 8 characters  //0.x version represents the baseline version, not for release  
at 0x8250 code const unsigned char sku_1[8]   = "#FOR01#";      //The string is limited to 8 characters
at 0x8258 code const unsigned char sku_2[8]   = "#123456#";     //The string is limited to 8 characters
at 0x8260 code const unsigned char sku_3[8]   = "#ABCDEF#";     //The string is limited to 8 characters
at 0x8268 code const unsigned char sku_4[8]   = "#abcdef#";     //The string is limited to 8 characters
at 0x8270 code const unsigned char sku_5[8]   = "#FORD#";       //The string is limited to 8 characters
at 0x8278 code const unsigned char sku_6[8]   = "#KEYFOB#";     //The string is limited to 8 characters
at 0x8280 code const unsigned char sku_7[8]   = "#IMMKEY#";     //The string is limited to 8 characters

uint8_t Tester_Present          = RESET;
uint8_t Timeout_EOL_Flag        = RESET;
uint8_t buf[8]                  = {0};
uint8_t kline_buf[8]            = {0};
uint8_t uart_master_data[12]    = {0};

EOL_State gEOL_State = EOL_ENTRY;

//Implement the below interface in the can application side
void EOL_tester_function(void)
{
    buf[0] = 0xAA; buf[1] = 0xAA; buf[2] = 0xAA; buf[3] = 0xAA;
    buf[4] = 0xAA; buf[5] = 0xAA; buf[6] = 0xAA; buf[7] = 0xAA;

     if(l_can_receive_flag == 1)
    {
        l_can_receive_flag = 0;
    
        if( (_Data[0] == 0x02) && (_Data[1] == 0x10) )
        {
            if(_Data[2] == 0x01)
            {
                buf[0] = 0x02; buf[1] = 0x50; buf[2] = 0x01; buf[3] = 0x00;
                buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;
                CAN_Transmit(0x7E7,0,0,8,&buf);
            }
            else if(_Data[2] == 0x00)
            {
                gEOL_State=EOL_EXIT;
                buf[0] = 0x02; buf[1] = 0x50; buf[2] = 0x00; buf[3] = 0x00;
                buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;
                CAN_Transmit(0x7E7,0,0,8,&buf);
            }
        }

        if( (_Data[0] == 0x02) && (_Data[1] == 0x2F) )
        {
            if((_Data[2] >> EOL_CTRL_BUZZER_POS) & 0x1) //Check for buzzer On/Off cmd
                {
                    GPIOC->ODR |= 1 << 1;
                }
            else
                {
                   GPIOC->ODR &= ~(1 << 1);  
                }
            
            if((_Data[2] >> EOL_CTRL_G_LED_POS) & 0x1) //Check for green led On/Off cmd
                {
                    GPIOC->ODR |= 1 << 2;  
                }
            else
                {
                    GPIOC->ODR &=~(1 << 2);
                }
            
            
            if((_Data[2] >> EOL_CTRL_R_LED_POS) & 0x1) //Check for red_led On/Off cmd
                {
                    GPIOC->ODR |= 1 << 3; 
                }
            else
                {
                    GPIOC->ODR &=~(1 << 3);
                } 
            
            buf[0] = 0x02; buf[1] = 0x6F; buf[2] = (_Data[2] & 0x07);
            CAN_Transmit(0x7E7,0,0,8,&buf);          
        }
        else if( (_Data[0] == 0x02) && (_Data[1] == 0x22) )
        {
            if(_Data[2] == 0x08)
            {
                buf[0] = 0x03; buf[1] = 0x62; buf[2] = 0x08;
                if((GPIOB->IDR & 0x01) == 0x01)  
                {
                    buf[3] = 0x00;   //Switch Position A or KEYFOB LEARN POSITION
                }
                else
                {
                    buf[3] = 0x01;   //Switch Position B or KEY IMM LEARN POSITION
                }        
                CAN_Transmit(0x7E7,0,0,8,&buf);
            }
            else if(_Data[2] == 0x10)
            {
                #if 1
                    buf[0] = 0x07; buf[1] = 0x62; buf[2] = 0x10;    //partnumber check
                    buf[3] = sku_1[1]; buf[4] = sku_1[2]; buf[5] = sku_1[3]; buf[6] = sku_1[4]; buf[7] = sku_1[5];
                    CAN_Transmit(0x7E7,0,0,8,&buf);
                #else  //Multiple Part numbers
                    buf[0] = 0x10; buf[1] = 0x37; buf[2] = 0x62; buf[3] = 0x10;   //partnumber check
                    buf[4] = 'F'; buf[5] = 'L'; buf[6] = 'O'; buf[7] = 'W';
                    CAN_Transmit(0x7E7,0,0,8,&buf);
                    wait_time(2);
            
                    buf[0] = 0x21; buf[1] = 0x01; 
                    buf[2] = sku_1[1]; buf[3] = sku_1[2]; buf[4] = sku_1[3];  //partnumber check
                    buf[5] = sku_1[4]; buf[6] = sku_1[5]; buf[7] = sku_1[6];
                    CAN_Transmit(0x7E7,0,0,8,&buf);  
                    wait_time(2); 
            
                    buf[0] = 0x22; buf[1] = 0x02; 
                    buf[2] = sku_2[1]; buf[3] = sku_2[2]; buf[4] = sku_2[3];  //partnumber check
                    buf[5] = sku_2[4]; buf[6] = sku_2[5]; buf[7] = sku_2[6];
                    CAN_Transmit(0x7E7,0,0,8,&buf);  
                    wait_time(2);  
            
                    buf[0] = 0x23; buf[1] = 0x03; 
                    buf[2] = sku_3[1]; buf[3] = sku_3[2]; buf[4] = sku_3[3];  //partnumber check
                    buf[5] = sku_3[4]; buf[6] = sku_3[5]; buf[7] = sku_3[6];
                    CAN_Transmit(0x7E7,0,0,8,&buf);  
                    wait_time(2);
            
                    buf[0] = 0x24; buf[1] = 0x04; 
                    buf[2] = sku_4[1]; buf[3] = sku_4[2]; buf[4] = sku_4[3];  //partnumber check
                    buf[5] = sku_4[4]; buf[6] = sku_4[5]; buf[7] = sku_4[6];
                    CAN_Transmit(0x7E7,0,0,8,&buf);  
                    wait_time(2);
            
                    buf[0] = 0x25; buf[1] = 0x05; 
                    buf[2] = sku_5[1]; buf[3] = sku_5[2]; buf[4] = sku_5[3];  //partnumber check
                    buf[5] = sku_5[4]; buf[6] = sku_5[5]; buf[7] = sku_5[6];
                    CAN_Transmit(0x7E7,0,0,8,&buf);  
                    wait_time(2);
            
                    buf[0] = 0x26; buf[1] = 0x06; 
                    buf[2] = sku_6[1]; buf[3] = sku_6[2]; buf[4] = sku_6[3];  //partnumber check
                    buf[5] = sku_6[4]; buf[6] = sku_6[5]; buf[7] = sku_6[6];
                    CAN_Transmit(0x7E7,0,0,8,&buf);  
                    wait_time(2);
            
                    buf[0] = 0x27; buf[1] = 0x07; 
                    buf[2] = sku_7[1]; buf[3] = sku_7[2]; buf[4] = sku_7[3];  //partnumber check
                    buf[5] = sku_7[4]; buf[6] = sku_7[5]; buf[7] = sku_7[6];
                    CAN_Transmit(0x7E7,0,0,8,&buf);  
                    wait_time(2);
                #endif
            }
            else if(_Data[2] == 0x20)
            {
                buf[0] = 0x07; buf[1] = 0x62; buf[2] = 0x20;    //Software Version
                buf[3] = version[1];
                buf[4] = version[2];
                buf[5] = version[3];
                buf[6] = version[4];
                buf[7] = version[5];
                CAN_Transmit(0x7E7,0,0,8,&buf);
            }
            else
            {
            }
        }
        else if( (_Data[0] == 0x03) && (_Data[1] == 0x22) && (_Data[2] == 0x40))
        {
            if(_Data[3] == 0x00)
            {
                FLASH_EraseByte(0x4100);
                FLASH_EraseByte(0x4101);
                buf[0] = 0x05; buf[1] = 0x62; buf[2] = 0x40; buf[3] = 0x00;  //Clearing the Dongle used count
                buf[4] = FLASH_ReadByte(0x4101);  //Reading the keys programmed count from the flash location 0x4101
                buf[5] = FLASH_ReadByte(0x4100);  //Reading the fobs programmed count from the flash location 0x4100
                if((buf[3] == 0) && (buf[4] == 0))
                {
                    GPIOC->ODR &=~(1 << 3);
                    GPIOC->ODR |= 1 << 2;
                }
                CAN_Transmit(0x7E7,0,0,8,&buf);
            }
            if(_Data[3] == 0x01)
            {
                buf[0] = 0x05; buf[1] = 0x62; buf[2] = 0x40; buf[3] = 0x01;   //Dongle Used Count
                buf[4] = FLASH_ReadByte(0x4101);  //Reading the keys programmed count from the flash location 0x4101
                buf[5] = FLASH_ReadByte(0x4100);  //Reading the fobs programmed count from the flash location 0x4100
                CAN_Transmit(0x7E7,0,0,8,&buf);
            }
        }
        else
        {
        }
    }
}



/*******************************************************************/
/*                                                                 */
/*   FUNCTION NAME  :   uart_process_data_2                        */
/*                                                                 */
/*   FUNCTION BRIEF :   End of line tester kline process           */
/*                                                                 */
/*                                                                 */
/*   PARAMETERS     :   None                                       */
/*                                                                 */
/*   RETURN VALUES  :   None                                       */
/*                                                                 */
/*******************************************************************/
void uart_process_data_2(u8* BuffPointer,u8 rec_length)
{
    u8 l_data[10];
    u8 l_lenght = 0;
    u8 iLoop=0;
    for(iLoop = 0;iLoop < rec_length;iLoop++)
    {
        uart_master_data[iLoop] = BuffPointer[iLoop];
    }
    uart_master_data[9] = rec_length;

    if( (uart_master_data[0] == 0x82) && (uart_master_data[1] == 0xC1) && (uart_master_data[2] == 0xF0) )
    {
        if( (uart_master_data[3] == 0x21) && (uart_master_data[4] == 0x01))
        {
            l_data[0] = 0x83;
            l_data[1] = 0xF0;
            l_data[2] = 0xC1;
            l_data[3] = 0x61;
            l_data[4] = 0x01;
            if((GPIOB->IDR & 0x01) == 0x01)  
            {
                l_data[5] = 0x00;   //Switch Position A or KEYFOB LEARN POSITION //This is OFF 
            }
            else
            {
                l_data[5] = 0x01;  //Switch Position B or KEY IMM LEARN POSITION //This is ON
            }   
            
        
            //l_data[6] = 0x0B; //Automatically transmits this checksum    
            l_lenght  = 6;
            kline_transmit(l_data, l_lenght);
        }
    }
}


/*******************************************************************/
/*                                                                 */
/*   FUNCTION NAME  :   kline_receive_validate_message2            */
/*                                                                 */
/*   FUNCTION BRIEF :   received data will processed into kline    */
/*                      frame                                      */
/*                                                                 */
/*   PARAMETERS     :   Received Byte                              */
/*                                                                 */
/*   RETURN VALUES  :   None                                       */
/*                                                                 */
/*******************************************************************/

void kline_receive_validate_message2(uint8_t rx_data)
{
    uint8_t rx_cs = 0;

    if (rx_count == (uint8_t)0x00)
    {
        if (((rx_data & 0xF0) == 0x80))
        {
            // Load received data into variable
            rcv_data[rx_count] = rx_data;

            // The length of the k-line message is the first 6 bits of the first byte, plus 3 bytes of address (and 1 CRC byte)
            receive_len = (0x3F & rcv_data[0]) + 3;

            // Increment received data array index

            rx_count++;
        }
        else
        {
            // Else reset received data array index
            rx_count = 0;
            receive_len = 0;
        }
    }

    else if (rx_count == 1)
    {
        // Check if second byte is to the tester
        if ((rx_data == 0xC1))
        {
            // Load received data into variable
            rcv_data[rx_count] = rx_data;

            // Increment received data array index
            rx_count++;
        }
        else
        {
            // Else reset received data array index
            rx_count = 0;
            receive_len = 0;
        }
    }

    else if (rx_count == 2)
    {
        // Check if second byte is from the immobilizer
        if (rx_data == 0xF0)
        {
            // Load received data into variable
            rcv_data[rx_count] = rx_data;

            // Increment received data array index
            rx_count++;
        }
        else
        {
            // Else reset received data array index
            rx_count = 0;
            receive_len = 0;
        }
    }

    else
    {
        if (rx_count <= receive_len)
        {
            rcv_data[rx_count] = rx_data;

            if (rx_count == receive_len)
            {
                rx_cs = kline_checksum(rcv_data, receive_len);

                if (rx_cs == rcv_data[rx_count])
                {
                    uart_process_data_2(rcv_data, receive_len);
                    kline_total_rcv_cnt++;
                }
                rx_count = 0; //reset the receive count, after last byte is received
                receive_len = 0;
            }
            else
            {
                rx_count++;
            }
        }
        else
        {
            rx_count = 0;
            receive_len = 0;
        }
    }
}

/*****************************************************************************
 *
 *	End of file EOL.c
 *
 *****************************************************************************/