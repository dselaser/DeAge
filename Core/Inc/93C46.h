// 93C46.h
#ifndef __93C46_H
#define __93C46_H

#include "stm32f1xx_hal.h"

void EEPROM_Write_Enable(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, SPI_HandleTypeDef* hspi);
void EEPROM_Write_Disable(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, SPI_HandleTypeDef* hspi);
void EEPROM_Write_Byte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t address, uint8_t data, SPI_HandleTypeDef* hspi);
uint8_t EEPROM_Read_Byte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t address, SPI_HandleTypeDef* hspi);
void EEPROM_Check_All(UART_HandleTypeDef* huart, SPI_HandleTypeDef* hspi);
void EEPROM_Write_All_Zeros(SPI_HandleTypeDef* hspi);

void EEPROM_Set_All_CS_High(void);
void EEPROM_Write_Data(uint8_t data,SPI_HandleTypeDef* hspi);
void Blink_Report_Pins(void);
void EEPROM_Erase_And_Verify(SPI_HandleTypeDef* hspi);

// CS 핀 배열
static struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} cs_pins[] = {
    {GPIOA, GPIO_PIN_1}, {GPIOA, GPIO_PIN_2}, {GPIOA, GPIO_PIN_3},
    {GPIOA, GPIO_PIN_4}, {GPIOA, GPIO_PIN_5}, {GPIOA, GPIO_PIN_6},
    {GPIOA, GPIO_PIN_7}, {GPIOB, GPIO_PIN_9}, {GPIOB, GPIO_PIN_8},
    {GPIOB, GPIO_PIN_3}, {GPIOB, GPIO_PIN_4}, {GPIOB, GPIO_PIN_5}
};

#endif // __93C46_H
