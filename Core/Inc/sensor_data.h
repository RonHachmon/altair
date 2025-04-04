/*
 * sensor_data.h
 *
 *  Created on: Apr 1, 2025
 *      Author: 97254
 */

#ifndef SENSOR_DATA_H_
#define SENSOR_DATA_H_

#include <stdint.h>


#define ERROR_MODE 0
#define SAFE_MODE 1
#define OK_MODE 2

typedef struct SensorData{
    uint32_t timestamp;
    float temp;
    float humid;
    float volage;
    uint8_t light;
    uint8_t mode;

} SensorData;

extern SensorData g_latest_sensor_data;


#endif //SENSOR_DATA_H_
