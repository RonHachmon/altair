/*
 * collection_task.c
 *
 *  Created on: Apr 2, 2025
 *      Author: 97254
 */


#include "adc.h"
#include "tim.h"
#include "main.h"
#include "cmsis_os.h"

#include "tasks/collector_task.h"

#include "global_queues.h"

#include "rgb_led.h"
#include "mod_led.h"
#include "buzzer.h"
#include "dht.h"
#include "adc_part.h"
#include "DateTime.h"
#include "sensor_data.h"


#define LIGHT_MAX_VALUE 255
void GetVoltageParts(uint16_t adc_value, uint16_t *whole, uint16_t *fraction);
uint8_t MapToPercentage(uint16_t adc_value, uint16_t max_value);

void collection_task(void* context)
{
	CollectorSetting setting = *((CollectorSetting*) context);
	Led blue_led;
	Led red_led;
	DHT dht;

	ADC_Part potentiometer;
	uint16_t pot_data;

	ADC_Part light_sensor;
	uint16_t light_data;

	CelsiusAndHumidity celsuius;
	DateTime datetime;
	SensorData data;



	init_DHT(&dht, DHT_GPIO_Port, DHT_Pin, &htim6);
	init_Buzzer(&htim3,TIM_CHANNEL_1);
	ADC_init(&potentiometer, &hadc1);
	ADC_init(&light_sensor, &hadc2);


	led_create(&blue_led, LED_BLUE_GPIO_Port, LED_BLUE_Pin);
	led_create(&red_led, LED_RED_GPIO_Port, LED_RED_Pin);

	uint8_t valid_data_flag = 1;


	RGB_LED_init( RGB_RED_GPIO_Port, RGB_RED_Pin, RGB_GREEN_GPIO_Port, RGB_GREEN_Pin,RGB_BLUE_GPIO_Port, RGB_BLUE_Pin);
	while(1) {
		valid_data_flag = 1;

		led_toggle(&red_led);
		RGB_LED_next_Color();

		RTC_ReadDateTime(&datetime);

		if(read_DHT(&dht, &celsuius) != DHT_OK){
			valid_data_flag = 0;
			printf("DHT fail \r\n");
		}

		if(ADC_read_data(&potentiometer,&pot_data) != HAL_OK){
			valid_data_flag = 0;
			printf("Pot error \r\n");
		}

		if(ADC_read_data(&light_sensor,&light_data) != HAL_OK){
			valid_data_flag = 0;
			printf("Light error \r\n");
		}

		data.timestamp = datetime_to_timestamp(&datetime);
		data.humid = celsuius.humidity_integral + (celsuius.humidity_fractional / 100.0f);
		data.temp = celsuius.tempature_integral + (celsuius.tempature_fractional / 100.0f);
		data.light = MapToPercentage(light_data, LIGHT_MAX_VALUE);
		data.mode = 1;

		g_latest_sensor_data = data;

		if(valid_data_flag == 1)
		{
			 osMessageQueuePut(g_sensor_queue, &data, 0,0);
			//osStatus_t status2 = osMessageQueuePut(setting.beacon_queue, &beacon_data, 0, 0);

			osDelay(setting.delay);
		}
		valid_data_flag = 1;

		//save_to_datetime(&datetime);


		 //printf("------------------------------\r\n");


		//buzzer_run(100);
	}

}


uint8_t MapToPercentage(uint16_t adc_value, uint16_t max_value) {
    return (adc_value * 100) / max_value;
}

void GetVoltageParts(uint16_t adc_value, uint16_t *whole, uint16_t *fraction) {
    uint32_t voltage_mV = (adc_value * 3300) / 4095;  // Convert to millivolts
    *whole = voltage_mV / 1000;   // Extract whole number (V)
    *fraction = voltage_mV % 1000; // Extract fractional part (mV)
}
