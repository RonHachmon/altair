/*
 * collection_task.c
 *
 *  Created on: Apr 2, 2025
 *      Author: 97254
 */


#include "adc.h"
#include "tim.h"
#include "main.h"

#include "collector_task.h"
#include "rgb_led.h"
#include "mod_led.h"
#include "buzzer.h"
#include "dht.h"
#include "adc_part.h"
#include "beacon_data.h"
#include "cmsis_os.h"
#include "DateTime.h"
#include "global_queues.h"


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
	uint16_t whole;
	uint16_t fraction;

	ADC_Part light_sensor;
	uint16_t light_data;

	CelsiusAndHumidity celsuius;
	DateTime datetime;
	BeaconData beacon_data;



	init_DHT(&dht, DHT_GPIO_Port, DHT_Pin, &htim6);
	init_Buzzer(&htim3,TIM_CHANNEL_1);
	ADC_init(&potentiometer, &hadc1);
	ADC_init(&light_sensor, &hadc2);


	led_create(&blue_led, LED_BLUE_GPIO_Port, LED_BLUE_Pin);
	led_create(&red_led, LED_RED_GPIO_Port, LED_RED_Pin);

	uint8_t valid_data_flag = 1;


	RGB_LED_init( RGB_RED_GPIO_Port, RGB_RED_Pin, RGB_GREEN_GPIO_Port, RGB_GREEN_Pin,RGB_BLUE_GPIO_Port, RGB_BLUE_Pin);
	while(1){
		valid_data_flag = 1;

		//led_off(&blue_led);

		led_toggle(&red_led);
		RGB_LED_next_Color();

		RTC_ReadDateTime(&datetime);

		//RTC_PrintDateTime(&datetime);


		if(read_DHT(&dht, &celsuius) == DHT_OK){
			//print_CelsiusAndHumidity(&celsuius);

		}else{
			valid_data_flag = 0;
			printf("DHT fail \r\n");
		}

		if(ADC_read_data(&potentiometer,&pot_data) == HAL_OK){
			//GetVoltageParts(pot_data, &whole, &fraction);
			//printf("Voltage: %d.%03d V\r\n", whole, fraction);
		}
		else{
			valid_data_flag = 0;
			printf("Pot error \r\n");
		}

		if(ADC_read_data(&light_sensor,&light_data) == HAL_OK){
			//printf("Light %d%% \r\n", MapToPercentage(light_data, LIGHT_MAX_VALUE));
		}
		else{
			valid_data_flag = 0;
			printf("Light error \r\n");
		}

		beacon_data.timestamp = datetime_to_timestamp(&datetime);
		beacon_data.last_humid = celsuius.humidity_integral + (celsuius.humidity_fractional / 100.0f);
		beacon_data.last_temp = celsuius.tempature_integral + (celsuius.tempature_fractional / 100.0f);
		beacon_data.last_light = MapToPercentage(light_data, LIGHT_MAX_VALUE);
		beacon_data.mode = 1;
		//g_beacon_data = beacon_data;

		if(valid_data_flag == 1)
		{
			osStatus_t status2 = osMessageQueuePut(beacon_queue, &beacon_data, 0, 0);
			osDelay(setting.delay);
		}
		valid_data_flag = 1;

		//save_to_datetime(&datetime);


		 //printf("------------------------------\r\n");


		buzzer_run(100);
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
