#include "93C46.h"
#include "stdio.h"
#include "string.h"

#define EEPROM_SIZE 128
#define CS_PINS_COUNT 12



void EEPROM_Select_CS(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    // 모든 CS 핀을 비활성화
    for (int i = 0; i < 12; i++)
    {
        HAL_GPIO_WritePin(cs_pins[i].port, cs_pins[i].pin, GPIO_PIN_RESET); // CS 비활성화 (0)
    }
    // 선택한 CS 핀을 활성화
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET); // CS 활성화 (1)
}

void EEPROM_Write_Enable(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, SPI_HandleTypeDef* hspi)
{
    uint8_t cmd[2] = {0x02, 0x60}; // EWEN 명령어: 1111 1110 0110 0000
    EEPROM_Select_CS(GPIOx, GPIO_Pin); // CS 핀 선택
    HAL_SPI_Transmit(hspi, cmd, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET); // CS 비활성화 (0)
}

void EEPROM_Write_Disable(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, SPI_HandleTypeDef* hspi)
{
    uint8_t cmd[2] = {0x02, 0x00}; // EWDS 명령어: 1111 1110 0000 0000
    printf("EEPROM_Write_Disable CMD: 0x%02X 0x%02X\n", cmd[0], cmd[1]);
    EEPROM_Select_CS(GPIOx, GPIO_Pin); // CS 핀 선택
    HAL_SPI_Transmit(hspi, cmd, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET); // CS 비활성화 (0)
}

void EEPROM_Write_Byte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t address, uint8_t data, SPI_HandleTypeDef* hspi)
{
    uint8_t cmd[3];
    cmd[0] = 0x02; // MSB는 0000 0010로 채움
    cmd[1] = 0x80 | (address & 0x7F); // WRITE 명령어: 1AAAAAAA
    cmd[2] = data;

    EEPROM_Write_Enable(GPIOx, GPIO_Pin, hspi);

    EEPROM_Select_CS(GPIOx, GPIO_Pin); // CS 핀 선택
    HAL_SPI_Transmit(hspi, cmd, 3, HAL_MAX_DELAY); // 3바이트 명령 전송
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET); // CS 비활성화 (0)

    HAL_Delay(20); // 쓰기 완료 대기

    EEPROM_Write_Disable(GPIOx, GPIO_Pin, hspi);
}

uint8_t EEPROM_Read_Byte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t address, SPI_HandleTypeDef* hspi)
{
    uint8_t cmd[2];
    uint8_t data = 0xFF;

    cmd[0] = 0x03; // MSB는 1111 1111로 채움
    cmd[1] = 0x00 | (address & 0x7F); // READ 명령어: 0AAAAAAA

    printf("EEPROM Read CMD: 0x%02X 0x%02X\n", cmd[0], cmd[1]);

    EEPROM_Select_CS(GPIOx, GPIO_Pin); // CS 핀 선택
    HAL_SPI_Transmit(hspi, cmd, 2, HAL_MAX_DELAY); // 2바이트 명령 전송
    HAL_SPI_Receive(hspi, &data, 1, HAL_MAX_DELAY); // 데이터 수신
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET); // CS 비활성화 (0)

    return data;
}

void EEPROM_Erase_Byte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t address, SPI_HandleTypeDef* hspi)
{
    uint8_t cmd[2];
    cmd[0] = 0x03; // MSB는 1111 1111로 채움
    cmd[1] = 0x80 | (address & 0x7F); // ERASE 명령어: 1AAAAAAA

    EEPROM_Write_Enable(GPIOx, GPIO_Pin, hspi);

    EEPROM_Select_CS(GPIOx, GPIO_Pin); // CS 핀 선택
    HAL_SPI_Transmit(hspi, cmd, 2, HAL_MAX_DELAY); // 2바이트 명령 전송
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET); // CS 비활성화 (0)

    HAL_Delay(20); // 쓰기 완료 대기

    EEPROM_Write_Disable(GPIOx, GPIO_Pin, hspi);
}

void EEPROM_Erase_All(SPI_HandleTypeDef* hspi)
{
    uint8_t cmd[2] = {0x02, 0x40}; // ERAL 명령어: 1111 1110 0100 0000

    EEPROM_Select_CS(GPIOA, GPIO_PIN_1); // 특정 CS 핀 선택 (예시)
    HAL_SPI_Transmit(hspi, cmd, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // CS 비활성화 (0)

    HAL_Delay(20); // 쓰기 완료 대기
}

void EEPROM_Write_All(SPI_HandleTypeDef* hspi, uint8_t data)
{
    uint8_t cmd[2] = {0x02, 0x20}; // WRAL 명령어: 1111 1110 0010 0000

    EEPROM_Write_Enable(GPIOA, GPIO_PIN_1, hspi);

    EEPROM_Select_CS(GPIOA, GPIO_PIN_1); // 특정 CS 핀 선택 (예시)
    HAL_SPI_Transmit(hspi, cmd, 2, HAL_MAX_DELAY); // 2바이트 명령 전송
    HAL_SPI_Transmit(hspi, &data, 1, HAL_MAX_DELAY); // 데이터 전송
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // CS 비활성화 (0)

    HAL_Delay(20); // 쓰기 완료 대기

    EEPROM_Write_Disable(GPIOA, GPIO_PIN_1, hspi);
}


void EEPROM_Write_All_Zeros(SPI_HandleTypeDef* hspi)
{
    uint8_t data = 0x00; // Data to write (all zeros)
    const int maxRetries = 5; // Maximum number of retries for each address

    for (int i = 0; i < CS_PINS_COUNT; i++)
    {
        for (uint16_t address = 0; address < EEPROM_SIZE; address++)
        {
            uint8_t readData = 0xFF; // Initialize with a non-zero value for comparison
            int retries = 0;

            // Write and verify until the byte is confirmed as 0 or maxRetries reached
            while (readData != 0x00 && retries < maxRetries)
            {
                // Write 0 to the current address
                EEPROM_Write_Byte(cs_pins[i].port, cs_pins[i].pin, address, data, hspi);
                HAL_Delay(10); // Delay to ensure proper writing

                // Read back the byte to verify
                readData = EEPROM_Read_Byte(cs_pins[i].port, cs_pins[i].pin, address, hspi);

                // Increment the retry counter
                retries++;
            }

            // Optional: Check if max retries were reached without success
            if (retries == maxRetries && readData != 0x00)
            {
                printf("Failed to write 0 at address 0x%02X after %d retries\n", address, maxRetries);
            }
        }
    }
}


/*
void EEPROM_Write_All_Zeros(SPI_HandleTypeDef* hspi)
{
    uint8_t data = 0x00; // Data to write (all zeros)

    for (int i = 0; i < CS_PINS_COUNT; i++)
    {
        for (uint16_t address = 0; address < EEPROM_SIZE; address++)
        {
            EEPROM_Write_Byte(cs_pins[i].port, cs_pins[i].pin, address, data, hspi);
            HAL_Delay(20); // Increase delay to ensure proper writing
        }
    }
}
*/
int EEPROM_value;
uint8_t address, tmp;
int report[CS_PINS_COUNT] = {0}; // Initialize the report array to store results

void EEPROM_Write_Data(uint8_t data, SPI_HandleTypeDef* hspi)
{
    for (int i = 0; i < CS_PINS_COUNT; i++)
    {
        for (uint16_t address = 0; address < EEPROM_SIZE; address++) // 93C46의 전체 주소 범위
        {
            EEPROM_Write_Byte(cs_pins[i].port, cs_pins[i].pin, address, data, hspi);
            HAL_Delay(20); // Increase delay

            EEPROM_value = EEPROM_Read_Byte(cs_pins[i].port, cs_pins[i].pin, address, hspi);
            tmp = data & 0x7f;

            if (tmp != EEPROM_value) // EEPROM value and data value are not the same
            {
                report[i] = (int)i; // Store the i value in hex
                break; // Assuming you want to move to the next cs_pin once a mismatch is found
            }
        }
     }
}

void Blink_Report_Pins(void)
{
    while (1)
    {
        for (int j = 0; j < 12; j++)
        {
            int index = report[j];
            HAL_GPIO_TogglePin(cs_pins[index].port, cs_pins[index].pin); // Toggle the pin
        }
        HAL_Delay(500); // Wait for 500ms
    }
}

// U2부터 하나씩 0으로 쓰고, verify 후 결과를 LED로 표시
// pass_result[i]: 1=합격(LED ON), 0=불합격(LED OFF)
uint8_t erase_pass_result[CS_PINS_COUNT];

void EEPROM_Erase_And_Verify(SPI_HandleTypeDef* hspi)
{
    // 모든 LED OFF
    for (int i = 0; i < CS_PINS_COUNT; i++)
    {
        HAL_GPIO_WritePin(cs_pins[i].port, cs_pins[i].pin, GPIO_PIN_RESET);
        erase_pass_result[i] = 0;
    }

    // U2(index 0)부터 하나씩 처리
    for (int i = 0; i < CS_PINS_COUNT; i++)
    {
        // 현재 칩 LED ON (사용자가 어떤 칩을 프로그램하는지 알 수 있음)
        HAL_GPIO_WritePin(cs_pins[i].port, cs_pins[i].pin, GPIO_PIN_SET);
        HAL_Delay(200);

        // 전체 주소에 0x00 쓰기
        for (uint16_t addr = 0; addr < EEPROM_SIZE; addr++)
        {
            EEPROM_Write_Byte(cs_pins[i].port, cs_pins[i].pin, addr, 0x00, hspi);
        }

        // Verify: 한번만 전체 읽어서 확인
        uint8_t pass = 1;
        for (uint16_t addr = 0; addr < EEPROM_SIZE; addr++)
        {
            uint8_t readData = EEPROM_Read_Byte(cs_pins[i].port, cs_pins[i].pin, addr, hspi);
            if (readData != 0x00)
            {
                pass = 0;
                break;
            }
        }

        if (pass)
        {
            // 합격: LED ON 유지
            erase_pass_result[i] = 1;
            HAL_GPIO_WritePin(cs_pins[i].port, cs_pins[i].pin, GPIO_PIN_SET);
        }
        else
        {
            // 불합격: LED OFF
            erase_pass_result[i] = 0;
            HAL_GPIO_WritePin(cs_pins[i].port, cs_pins[i].pin, GPIO_PIN_RESET);
        }
    }
}
