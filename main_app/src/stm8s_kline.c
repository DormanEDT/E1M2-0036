/* Includes ------------------------------------------------------------------*/
#include "stm8s_Kline.h"
#include "stm8s_uart1.h"
#include "stm8s_eol.h"
/* Variable Declarations **********************************************/
uint8_t kline_tx_length              = RESET;
uint8_t kline_receive_complete_flag  = RESET;
uint8_t rx_count                     = RESET;
uint8_t receive_len                  = RESET;
uint8_t uart_recv_data               = RESET;
uint8_t rcv_data[12]                 = {0};
uint8_t kline_rx_data[12]            = {0};
uint8_t kline_tx_data[12]            = {0};

uint16_t kline_total_rcv_cnt         = RESET;
uint32_t BaudRate                    = 10417;
/* Variable Declarations **********************************************/


/*******************************************************************/
/*                                                                 */
/*   FUNCTION NAME  :   UART_Config                                */
/*                                                                 */
/*   FUNCTION BRIEF :   UART Initialisation                        */
/*                                                                 */
/*                                                                 */
/*   PARAMETERS     :   None                                       */
/*                                                                 */
/*   RETURN VALUES  :   None                                       */
/*                                                                 */
/*******************************************************************/

void UART_Config(void)
{
    u32 BaudRate_brr1 = RESET, BaudRate_brr2 = RESET;
    GPIOA->DDR &= ~((u8)0x10); // Configured as Input
    GPIOA->CR1 |= (u8)0x10;    // Internal pull up
    GPIOA->CR2 &= ~((u8)0x10); // Floating Input
                               // UART1 Settings Related Register (No. of Bits. Parity, etc.
    UART1->CR1 = (u8)0x00;
    // Set Baud Rate
    UART1->BRR1 = (u8)0x00;
    UART1->BRR2 = (u8)0x00;
    /* Set the UART1 BaudRates in BRR1 and BRR2 registers according to UART1_BaudRate value */
    BaudRate_brr1 = ((u32)CPU_FREQUENCY / (BaudRate << (u8)4));
    BaudRate_brr2 = (((u32)CPU_FREQUENCY * 100) / (BaudRate << (u8)4));
    /* Set the fraction of UART1DIV  */
    UART1->BRR2 |= (u8)((u8)(((BaudRate_brr2 - (BaudRate_brr1 * 100)) << (u8)4) / 100) & (u8)0x0F);
    /* Set the MSB mantissa of UART1DIV  */
    UART1->BRR2 |= (u8)((BaudRate_brr1 >> (u8)4) & (u8)0xF0);
    /* Set the LSB mantissa of UART1DIV  */
    UART1->BRR1 |= (u8)BaudRate_brr1;
    // UART1 Interrupt Related Register
    UART1->CR2  = (u8)0x24;
    UART1->CR3  = (u8)0x00;  // LIN Related Register
    UART1->CR4  = (u8)0x00;  // LIN Relared Register
    UART1->CR5  = (u8)0x00;  // Smart Card and IrDA Related
    UART1->GTR  = (u8)0x00;  // Guard Time Register (Smartcard Related)
    UART1->PSCR = (u8)0x00; // Only useful if SmartCard & IrDA is used
}


void kline_transmit(uint8_t *lp_data, uint8_t lp_len)
{
    uint8_t tr_i;

    UART1->CR2 = 0x08;
    for (tr_i = RESET; tr_i < lp_len; tr_i++)
    {
        UART1->DR = lp_data[tr_i];
        while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
        {
        }
    }

    UART1->DR = kline_checksum(lp_data, lp_len);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
    {
    }
   
    while (UART1_GetFlagStatus(UART1_FLAG_TC) == RESET)
    {
    }

    UART1->CR2 |= 0x24;
}



/*******************************************************************/
/*                                                                 */
/*   FUNCTION NAME  :   uart_process_data                          */
/*                                                                 */
/*   FUNCTION BRIEF :   Copying received data buffer               */
/*                                                                 */
/*                                                                 */
/*   PARAMETERS     :   Received buffer address,size of buffer     */
/*                                                                 */
/*   RETURN VALUES  :   None                                       */
/*                                                                 */
/*******************************************************************/

void uart_process_data(uint8_t *BuffPointer, uint8_t rec_length)
{
    uint8_t iLoop = RESET;

    for (iLoop = RESET; iLoop < rec_length; iLoop++)
    {
        kline_rx_data[iLoop] = BuffPointer[iLoop];
    }

    kline_receive_complete_flag = 1;
}

/*******************************************************************/
/*                                                                 */
/*   FUNCTION NAME  :   kline_checksum                             */
/*                                                                 */
/*   FUNCTION BRIEF :   Here 1ms tasks are done                    */
/*                                                                 */
/*                                                                 */
/*   PARAMETERS     :   Data buffer address,size of data buffer    */
/*                                                                 */
/*   RETURN VALUES  :   Checksum                                   */
/*                                                                 */
/*******************************************************************/
uint8_t kline_checksum(uint8_t *p_data, uint8_t len)
{
    uint8_t tr_i;
    uint8_t checksum = RESET;

    for (tr_i = RESET; tr_i < len; tr_i++)
    {
        checksum += p_data[tr_i];
    }

    return (checksum);
}

/*******************************************************************/
/*                                                                 */
/*   FUNCTION NAME  :   INTERRUPT_HANDLER                          */
/*                                                                 */
/*   FUNCTION BRIEF :   UART1 TX Interrupt routine                 */
/*                                                                 */
/*                                                                 */
/*   PARAMETERS     :   None                                       */
/*                                                                 */
/*   RETURN VALUES  :   None                                       */
/*                                                                 */
/*******************************************************************/

INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
    if ((UART1->SR & 0x0E) != RESET)
    {
        (void) UART1->SR;
        (void) UART1->DR;
     }
    UART1->SR = 0x00;
}

/*******************************************************************/
/*                                                                 */
/*   FUNCTION NAME  :   INTERRUPT_HANDLER                          */
/*                                                                 */
/*   FUNCTION BRIEF :   UART1 RX Interrupt routine                 */
/*                                                                 */
/*                                                                 */
/*   PARAMETERS     :   None                                       */
/*                                                                 */
/*   RETURN VALUES  :   None                                       */
/*                                                                 */
/*******************************************************************/

INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
    if ((UART1->SR & 0x0E) == FALSE)
    {
        uart_recv_data = UART1->DR;
        if((Tester_Present==TRUE))
        {
            kline_receive_validate_message2(uart_recv_data);
        }

    }
    else
    {
        (void) UART1->SR;
        //Read DR register will clear the error flags
        (void) UART1->DR;   
       
    }

    UART1->SR = 0x00;
}

/*****************************************************************************
 *
 *	End of file Kline.c
 *
 *****************************************************************************/