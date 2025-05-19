 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Private variables ---------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define TIME 0x7FFF

/* Public function prototypes -----------------------------------------------*/
void CLK_Config(void);
void CAN_Config(void);
void GPIO_Config(void);
void Delay (uint16_t nCount);
void wait_time(uint8_t xcount);

#endif /* __MAIN_H__ */