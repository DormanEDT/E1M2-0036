/* Includes ------------------------------------------------------------------*/
#include "stm8s_it.h"
#include "stm8s_eol.h"
#include "main.h"

// Global variable
uint8_t timer_counts = 0;
uint8_t timer_10ms_counts = 0;
uint16_t Timeout_EOL = 0;
#ifdef _COSMIC_
/**
  * @brief  Dummy interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(NonHandledInterrupt, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*_COSMIC_*/

/** TRAP interrupt routine */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
}
/** Top Level Interrupt routine  */
INTERRUPT_HANDLER(TLI_IRQHandler, 0)
{
}

/**Auto Wake Up Interrupt routine  */
INTERRUPT_HANDLER(AWU_IRQHandler, 1)
{
}

/** Clock Controller Interrupt routine  */
INTERRUPT_HANDLER(CLK_IRQHandler, 2)
{
}

/** External Interrupt PORTA Interrupt routine  */
INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
{
}

/** External Interrupt PORTB Interrupt routine  */
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
}

/** External Interrupt PORTC Interrupt routine  */
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
}

/** External Interrupt PORTD Interrupt routine */
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
}

/** External Interrupt PORTE Interrupt routine  */
INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
}


/** CAN RX Interrupt routine. */
INTERRUPT_HANDLER(CAN_RX_IRQHandler, 8)
{
	/* Receiver Receives Frame */
    CAN_Receive();
}

/** CAN TX Interrupt routine  */
INTERRUPT_HANDLER(CAN_TX_IRQHandler, 9)
{
}

/** SPI Interrupt routine  */
INTERRUPT_HANDLER(SPI_IRQHandler, 10)
{
}

/** Timer1 Update/Overflow/Trigger/Break Interrupt routine  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
}

/** Timer1 Capture/Compare Interrupt routine  */
INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
{
}

/** Timer2 Update/Overflow/Break Interrupt routine */
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{
}

/** Timer2 Capture/Compare Interrupt routine  */
INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
{
}

/** Timer3 Update/Overflow/Break Interrupt routine. */
INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{

}

/** Timer3 Capture/Compare Interrupt routine  */
INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
{
}


//~ /** UART1 TX Interrupt routine  */
//~ INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
//~ {

//~ }

//~ /** UART1 RX Interrupt routine  */
//~ INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
//~ {

//~ }


/*** @brief  I2C Interrupt routine  */
INTERRUPT_HANDLER(I2C_IRQHandler, 19)
{

}

/** UART3 TX interrupt routine.  */
INTERRUPT_HANDLER(UART3_TX_IRQHandler, 20)
{

 }

/*** @brief  UART3 RX interrupt routine.  */
INTERRUPT_HANDLER(UART3_RX_IRQHandler, 21)
{
}

/** ADC2 interrupt routine.  */
INTERRUPT_HANDLER(ADC2_IRQHandler, 22)
{
}

/** Timer4 Update/Overflow Interrupt routine  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
   // TIM4->SR1 = 0x00;
    TIM4->SR1=(TIM4->SR1)&~(0x01);

    //~ timer_counts++; 
    //~ if (timer_counts == 100)
    //~ {
        //~ timer_counts = 0;
        //~ // Toggle green LED
        //~ GPIOC->ODR ^= 0x04;
    //~ }

    if((Timeout_EOL_Flag==FALSE)&&(++Timeout_EOL>=2000))
    {
        Timeout_EOL = 0;
        Timeout_EOL_Flag=TRUE;
        if(Tester_Present==FALSE)
        {
            // Trigger buzzer and green LED when the dongle gets power
            GPIOC->ODR = 0x06;   
            wait_time(40);
            GPIOC->ODR = 0x00;
        }        
    }


    if(++timer_10ms_counts>=10)
    {   
        timer_10ms_counts = 0;
    
        if(Tester_Present==TRUE)
        {
            switch(gEOL_State)
            {
                case EOL_IDLE: 
                    break;
                
                case EOL_ENTRY:
                    EOL_tester_function();
                    break;
                
                case EOL_EXIT:
                    GPIOC->ODR = 0x00;    //buzzer off and led off
                    gEOL_State = EOL_IDLE;
                    break;
                default:
                    break;
            }
        }
        else
        {
        
        }
    }
}

/** Eeprom EEC Interrupt routine  */
INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
{
}
