#include "stm8s.h"

/**********************************************************************************/
/* Constant definitions                                                           */
/**********************************************************************************/

/* Macro Definitions ********************************************************/
#define KLINE_RX_BUF_SIZE 10
/* Macro Definitions ********************************************************/

/* Variable Extensions ************************************************/
extern uint8_t kline_tx_length;
extern uint8_t kline_receive_complete_flag;
extern uint8_t rx_count;
extern uint8_t receive_len;
extern uint8_t uart_recv_data;
extern uint8_t rcv_data[12];
extern uint8_t kline_rx_data[12];
extern uint8_t kline_tx_data[12];

extern uint16_t kline_total_rcv_cnt ;

extern uint32_t BaudRate;
/* Variable Extensions ************************************************/

/* Function Declarations ***********************************************/
void UART_Config(void);
void kline_wakeup(void);
void kline_transmit(uint8_t *lp_data, uint8_t lp_len);
void kline_receive_validate_message(uint8_t rx_data);
void uart_process_data(uint8_t *BuffPointer, uint8_t rec_length);
void PushToQueue_RxKline(uint8_t *, uint8_t);
uint8_t kline_checksum(uint8_t *p_data, uint8_t len);
extern uint8_t RetrieveFromQueue_RxKline(void);
/* Function Declarations ***********************************************/

/*****************************************************************************
 *
 *	End of file Kline.h
 *
 *****************************************************************************/