/* Includes ------------------------------------------------------------------*/
#include "stm8s_exti.h"

/** Deinit external interrupt control registers to default reset value  */
void EXTI_DeInit(void)
{
    EXTI->CR1 = EXTI_CR1_RESET_VALUE;
    EXTI->CR2 = EXTI_CR2_RESET_VALUE;
}

/** Set the external interrupt sensitivity of the selected port.  */
void EXTI_SetExtIntSensitivity(EXTI_Port_TypeDef Port, EXTI_Sensitivity_TypeDef SensitivityValue)
{
    /* Check function parameters */
    assert_param(IS_EXTI_PORT_OK(Port));
    assert_param(IS_EXTI_SENSITIVITY_OK(SensitivityValue));

    /* Set external interrupt sensitivity */
    switch (Port)
    {
    case EXTI_PORT_GPIOA:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PAIS);
        EXTI->CR1 |= (uint8_t)(SensitivityValue);
        break;
    case EXTI_PORT_GPIOB:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PBIS);
        EXTI->CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 2);
        break;
    case EXTI_PORT_GPIOC:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PCIS);
        EXTI->CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 4);
        break;
    case EXTI_PORT_GPIOD:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PDIS);
        EXTI->CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 6);
        break;
    case EXTI_PORT_GPIOE:
        EXTI->CR2 &= (uint8_t)(~EXTI_CR2_PEIS);
        EXTI->CR2 |= (uint8_t)(SensitivityValue);
        break;
    default:
        break;
    }
}

/** Set the TLI interrupt sensitivity.  */
void EXTI_SetTLISensitivity(EXTI_TLISensitivity_TypeDef SensitivityValue)
{
    /* Check function parameters */
    assert_param(IS_EXTI_TLISENSITIVITY_OK(SensitivityValue));

    /* Set TLI interrupt sensitivity */
    EXTI->CR2 &= (uint8_t)(~EXTI_CR2_TLIS);
    EXTI->CR2 |= (uint8_t)(SensitivityValue);
}

/** Get the external interrupt sensitivity of the selected port.  */
EXTI_Sensitivity_TypeDef EXTI_GetExtIntSensitivity(EXTI_Port_TypeDef Port)
{
    uint8_t value = 0;

    /* Check function parameters */
    assert_param(IS_EXTI_PORT_OK(Port));

    switch (Port)
    {
    case EXTI_PORT_GPIOA:
        value = (uint8_t)(EXTI->CR1 & EXTI_CR1_PAIS);
        break;
    case EXTI_PORT_GPIOB:
        value = (uint8_t)((uint8_t)(EXTI->CR1 & EXTI_CR1_PBIS) >> 2);
        break;
    case EXTI_PORT_GPIOC:
        value = (uint8_t)((uint8_t)(EXTI->CR1 & EXTI_CR1_PCIS) >> 4);
        break;
    case EXTI_PORT_GPIOD:
        value = (uint8_t)((uint8_t)(EXTI->CR1 & EXTI_CR1_PDIS) >> 6);
        break;
    case EXTI_PORT_GPIOE:
        value = (uint8_t)(EXTI->CR2 & EXTI_CR2_PEIS);
        break;
    default:
        break;
    }

    return((EXTI_Sensitivity_TypeDef)value);
}

/** Get the TLI interrupt sensitivity.  */
EXTI_TLISensitivity_TypeDef EXTI_GetTLISensitivity(void)
{

    uint8_t value = 0;

    /* Get TLI interrupt sensitivity */
    value = (uint8_t)(EXTI->CR2 & EXTI_CR2_TLIS);

    return((EXTI_TLISensitivity_TypeDef)value);
}
