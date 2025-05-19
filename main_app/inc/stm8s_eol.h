/**********************************************************************************/
/*Modifications                                                                   */
/**********************************************************************************/
#include "stm8s.h"

/* Macro Definitions ********************************************************/
#define EOL_CTRL_BUZZER_POS 0
#define EOL_CTRL_G_LED_POS  1
#define EOL_CTRL_R_LED_POS  2
/* Macro Definitions ********************************************************/

typedef enum EOL_Statemachine
{
    EOL_IDLE  = 0,
    EOL_ENTRY = 1,
    EOL_EXIT  = 2
}EOL_State;

/* Variable Extensions ************************************************/

extern uint8_t Tester_Present;
extern uint8_t Timeout_EOL_Flag;
extern EOL_State gEOL_State;
/* Variable Extensions ************************************************/

/* Function Declarations ***********************************************/
void EOL_tester_function(void);
void uart_process_data_2(u8* BuffPointer,u8 rec_length);
void kline_receive_validate_message2(uint8_t rx_data);
/* Function Declarations ***********************************************/

/*****************************************************************************
 *
 *	End of file EOL.h
 *
 *****************************************************************************/