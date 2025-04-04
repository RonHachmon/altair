#include "adc.h"
#include "tim.h"
#include "main.h"

#include "tasks/collector_task.h"
#include "tasks/event_task.h"

#include "global_queues.h"

#include "rgb_led.h"
#include "mod_led.h"
#include "buzzer.h"
#include "dht.h"
#include "adc_part.h"
#include "DateTime.h"
#include "sensor_data.h"

#define LIGHT_MAX_VALUE 255
#define TRUE 1
#define FALSE 0


static float get_voltage(uint16_t adc_value);
static uint8_t map_to_percentage(uint16_t adc_value, uint16_t max_value);
static uint8_t is_in_range(CollectorSetting* collector_setting, SensorData* sensor);
static uint8_t read_all_sensors(DHT* dht, ADC_Part* pot, ADC_Part* light, CelsiusAndHumidity* ch, uint16_t* pot_val, uint16_t* light_val);
static void init_components(DHT* dht, ADC_Part* pot, ADC_Part* light, Led* red_led, Led* blue_led);
static void process_sensor_data(SensorData* data, const CelsiusAndHumidity* ch, uint16_t pot_val, uint16_t light_val, DateTime* datetime);
static void handle_event_transition(SensorData* data, uint8_t prev_mode);


static Led red_led;
static Led blue_led;

void collection_task(void* context)
{
    CollectorSetting setting = *((CollectorSetting*) context);

    DHT dht;
    ADC_Part potentiometer, light_sensor;
    CelsiusAndHumidity ch;
    DateTime datetime;
    SensorData current_data;
    uint16_t pot_val, light_val;

    init_components(&dht, &potentiometer, &light_sensor, &red_led, &blue_led);

    RGB_LED_Set_Color(COLOR_GREEN);

    uint16_t toggle_buzzer = 0;
    buzzer_start();

    while (1) {

        RTC_ReadDateTime(&datetime);

        if (read_all_sensors(&dht, &potentiometer, &light_sensor, &ch, &pot_val, &light_val)) {
            process_sensor_data(&current_data, &ch, pot_val, light_val, &datetime);

            current_data.mode = is_in_range(&setting, &current_data) ? OK_MODE : ERROR_MODE;

            if (g_latest_sensor_data.mode != current_data.mode) {
                handle_event_transition(&current_data, g_latest_sensor_data.mode);
            }

            g_latest_sensor_data = current_data;
            osMessageQueuePut(g_sensor_queue, &current_data, 0, 0);

            if(toggle_buzzer == 0)
            {
            	buzzer_end();
            }
            else
            {
            	buzzer_start();
            }

            toggle_buzzer = (toggle_buzzer + 1) % 2;

            osDelay(setting.delay);
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == SW1_Pin) {
		led_toggle(&red_led);
	}
}


static void init_components(DHT* dht, ADC_Part* pot, ADC_Part* light, Led* red_led, Led* blue_led)
{
    init_DHT(dht, DHT_GPIO_Port, DHT_Pin, &htim6);
    init_Buzzer(&htim3, TIM_CHANNEL_1);
    ADC_init(pot, &hadc1);
    ADC_init(light, &hadc2);

    led_create(blue_led, LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    led_create(red_led, LED_RED_GPIO_Port, LED_RED_Pin);
    RGB_LED_init(RGB_RED_GPIO_Port, RGB_RED_Pin, RGB_GREEN_GPIO_Port, RGB_GREEN_Pin, RGB_BLUE_GPIO_Port, RGB_BLUE_Pin);

}

static uint8_t read_all_sensors(DHT* dht, ADC_Part* pot, ADC_Part* light, CelsiusAndHumidity* ch, uint16_t* pot_val, uint16_t* light_val)
{
    uint8_t success = TRUE;

    if (read_DHT(dht, ch) != DHT_OK) {
        printf("DHT fail\r\n");
        success = FALSE;
    }

    if (ADC_read_data(pot, pot_val) != HAL_OK) {
        printf("Pot error\r\n");
        success = FALSE;
    }

    if (ADC_read_data(light, light_val) != HAL_OK) {
        printf("Light error\r\n");
        success = FALSE;
    }

    return success;
}

static void process_sensor_data(SensorData* data, const CelsiusAndHumidity* ch, uint16_t pot_val, uint16_t light_val, DateTime* datetime)
{
    data->volage = get_voltage(pot_val);
    data->light = map_to_percentage(light_val, LIGHT_MAX_VALUE);
    data->timestamp = datetime_to_timestamp(datetime);

    data->humid = ch->humidity_integral + (ch->humidity_fractional / 100.0f);
    data->temp = ch->tempature_integral + (ch->tempature_fractional / 100.0f);
}

static void handle_event_transition(SensorData* data, uint8_t prev_mode)
{
    EventData event_data;
    event_data.timestamp = data->timestamp;

    if (prev_mode == OK_MODE) {
    	RGB_LED_Set_Color(COLOR_RED);
        event_data.event = EVENT_OK_TO_ERROR;
    } else {

    	RGB_LED_Set_Color(COLOR_GREEN);
        event_data.event = EVENT_ERROR_TO_OK;
    }

    osMessageQueuePut(g_event_queue, &event_data, 0, 0);
}

static uint8_t is_in_range(CollectorSetting* cs, SensorData* sensor)
{
    return (
        cs->min_humidity <= sensor->humid && cs->max_humidity >= sensor->humid &&
        cs->min_temp <= sensor->temp && cs->max_temp >= sensor->temp &&
        cs->min_light <= sensor->light &&
        cs->safe_voltage <= sensor->volage
    );
}

static uint8_t map_to_percentage(uint16_t adc_value, uint16_t max_value)
{
    return (adc_value * 100) / max_value;
}

static float get_voltage(uint16_t adc_value)
{
    return ((float)adc_value * 3.3f) / 4095.0f;
}
