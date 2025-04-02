/*
 * beacon_data.h
 *
 *  Created on: Apr 1, 2025
 *      Author: 97254
 */

#ifndef BEACON_DATA_H_
#define BEACON_DATA_H_

#define OK 0x01
#define LOGGING_ACTIVE 0x02
#define BEACON_ACTIVE 0x04
#define PROVISIONED 0x08
#define RESERVED 0xF0

typedef struct BeaconData{
	uint32_t timestamp;
	float last_temp;
	float last_humid;
	float last_light;
	uint8_t mode;

}BeaconData;

//BeaconData g_beacon_data;

#endif //BEACON_DATA_H_
