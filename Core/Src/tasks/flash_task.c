/*
 * flash_task.c
 *
 *  Created on: Apr 6, 2025
 *      Author: 97254
 */

#include "tasks/flash_task.h"
#include "main.h"







#define DELAY_ADDRESS        0x08080000
#define MIN_TEMP_ADDRESS     0x08080008
#define MAX_TEMP_ADDRESS     0x08080010
#define MIN_HUMDITY_ADDRESS  0x08080018
#define MIN_LIGHT_ADDRESS    0x08080020
#define SAFE_VOLTAGE_ADDRESS 0x08080028




void reset_flash()
{
	FLASH_EraseInitTypeDef erase;
	uint32_t errorStatus = 0;
	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	erase.Banks = FLASH_BANK_2;
	erase.Page = 0x08080000;
	erase.NbPages = 1;

	uint8_t buffer[8];


	uint16_t delay = 2500;
	uint16_t min_temp = 22;
	uint16_t max_temp = 26;

	uint16_t min_humidity = 30;
	uint16_t max_humidity = 52;

	uint16_t min_light = 50;
	float safe_voltage = 2.2;



    HAL_FLASH_Unlock();

    HAL_StatusTypeDef res = HAL_FLASHEx_Erase(&erase, &errorStatus);
    if (res != HAL_OK) {
        printf("Flash erase failed!\r\n");
        HAL_FLASH_Lock();
        return;
    }

    // Write delay
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &delay, sizeof(delay));
    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, DELAY_ADDRESS, *(uint64_t*)buffer);
    printf("Writing delay... %s\r\n", res == HAL_OK ? "OK" : "FAIL");

    // Write min_temp
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &min_temp, sizeof(min_temp));
    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MIN_TEMP_ADDRESS, *(uint64_t*)buffer);
    printf("Writing min_temp... %s\r\n", res == HAL_OK ? "OK" : "FAIL");

    // Write max_temp
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &max_temp, sizeof(max_temp));
    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MAX_TEMP_ADDRESS, *(uint64_t*)buffer);
    printf("Writing max_temp... %s\r\n", res == HAL_OK ? "OK" : "FAIL");

    // Write min_humidity
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &min_humidity, sizeof(min_humidity));
    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MIN_HUMDITY_ADDRESS, *(uint64_t*)buffer);
    printf("Writing min_humidity... %s\r\n", res == HAL_OK ? "OK" : "FAIL");


    // Write min_light
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &min_light, sizeof(min_light));
    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MIN_LIGHT_ADDRESS, *(uint64_t*)buffer);
    printf("Writing min_light... %s\r\n", res == HAL_OK ? "OK" : "FAIL");

    // Write safe_voltage
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &safe_voltage, sizeof(safe_voltage));
    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, SAFE_VOLTAGE_ADDRESS, *(uint64_t*)buffer);
    printf("Writing safe_voltage... %s\r\n", res == HAL_OK ? "OK" : "FAIL");

    HAL_FLASH_Lock();
}

//typedef struct CollectorSetting{
//	uint16_t delay;
//	uint16_t min_temp;
//	uint16_t max_temp;
//	uint16_t min_humidity;
//	uint16_t max_humidity;
//	uint8_t min_light;
//	float safe_voltage;
//
//}CollectorSetting;

void read_settings(CollectorSetting* collector_settings)
{
    if (collector_settings == NULL){
    	return;
    }

    static uint8_t buffer[8];

    // Read delay
    memcpy(buffer, (void*)DELAY_ADDRESS, sizeof(buffer));
    memcpy(&collector_settings->delay, buffer, sizeof(collector_settings->delay));
    printf("Read delay: %u\r\n", collector_settings->delay);

    // Read min_temp
    memcpy(buffer, (void*)MIN_TEMP_ADDRESS, sizeof(buffer));
    memcpy(&collector_settings->min_temp, buffer, sizeof(collector_settings->min_temp));
    printf("Read min_temp: %u\r\n", collector_settings->min_temp);

    // Read max_temp
    memcpy(buffer, (void*)MAX_TEMP_ADDRESS, sizeof(buffer));
    memcpy(&collector_settings->max_temp, buffer, sizeof(collector_settings->max_temp));
    printf("Read max_temp: %u\r\n", collector_settings->max_temp);

    // Read min_humidity
    memcpy(buffer, (void*)MIN_HUMDITY_ADDRESS, sizeof(buffer));
    memcpy(&collector_settings->min_humidity, buffer, sizeof(collector_settings->min_humidity));
    printf("Read min_humidity: %u\r\n", collector_settings->min_humidity);

    // Read min_light
    memcpy(buffer, (void*)MIN_LIGHT_ADDRESS, sizeof(buffer));
    memcpy(&collector_settings->min_light, buffer, sizeof(collector_settings->min_light));
    printf("Read min_light: %u\r\n", collector_settings->min_light);

    // Read safe_voltage
    memcpy(buffer, (void*)SAFE_VOLTAGE_ADDRESS, sizeof(buffer));
    memcpy(&collector_settings->safe_voltage, buffer, sizeof(collector_settings->safe_voltage));
    printf("Read safe_voltage: %.2f\r\n", collector_settings->safe_voltage);
}


void flash_task(void* context)
{

//	uint8_t buffer[16] = {'R', 'O', 'N', 'A', 'B', 'C', 'D', 'E',
//	                       'F', 'G', 'H', 'I', 'J', 'K', 'L', '\0'};
//
//
//	FLASH_EraseInitTypeDef erase;
//	uint32_t errorStatus = 0;
//	erase.TypeErase = FLASH_TYPEERASE_PAGES;
//	erase.Banks = FLASH_BANK_2;
//	erase.Page = 0x08080000;
//	erase.NbPages = 1;
//
//	HAL_FLASH_Unlock();
//	HAL_StatusTypeDef res =  HAL_FLASHEx_Erase(&erase, &errorStatus);
//	if(res == HAL_OK && errorStatus == 0xFFFFFFFF ){
//		uint64_t *dataToWrite =(uint64_t*) buffer;
//		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 0x08080000, *dataToWrite);
//		if(res == HAL_OK){
//			printf("Success 1:D\r\n");
//
//		}
//		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 0x08080008, *(dataToWrite + 1));
//		if(res == HAL_OK){
//			printf("Success 2:D\r\n");
//
//		}
//
//	}
//	HAL_FLASH_Lock();
//
//	char * data = (char *)(0x08080000);
//	while(*data != '\0'){
//		printf("%c", *data);
//		data += 1;
//	}
//	printf("\r\n");
//
//	char value = *data;
//	uint32_t value2 = *(data + 1);
//
//	printf("Value: %s\r\n", data);
//	printf("Value2 %X \r\n",value2);

}


