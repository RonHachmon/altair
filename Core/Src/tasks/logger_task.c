/*
 * logger.c
 *
 *  Created on: Apr 1, 2025
 *      Author: 97254
 */


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ff_gen_drv.h"


#include "tasks/logger_task.h"
#include "DateTime.h"
#include "dht.h"
#include "global_queues.h"

#include "sensor_data.h"


#define MAX_DATA_FILES 7
#define FILENAME_SIZE 9

void init_logger()
{
	static FATFS FatFs;
	static uint8_t init_flag = 0;
	osDelay(1000);

	if(init_flag == 0){
		FRESULT fres;
	    fres = f_mount(&FatFs, "/", 1);
	    if (fres != FR_OK) {
	    	printf("f_mount error (%i)\r\n", fres);
	        while(1);
	    }
	}
	init_flag = 1;


	printf("f_mount Success \r\n");

}


void logger_beacon_task(void* context)
{


	//osMessageQueueId_t queue = (osMessageQueueId_t) context;

    FIL fil;
    FRESULT fres;

    char file_names[MAX_DATA_FILES][FILENAME_SIZE] = {0};
    uint8_t file_index = 0;

    fres = f_mkdir("0:/sensor");
    if (fres != FR_OK && fres != FR_EXIST) {
	    	printf("f_open error to directory (%i)\r\n", fres);
	        while(1);
	}

    BYTE writeBuf[128];
    UINT bytesWrote;
    char current_file_name[9];

    char whole_file_path[32];

    while(1){
    	SensorData data;
    	DateTime datetime;
    	uint16_t data_len;
    	osStatus_t status;


		status = osMessageQueueGet(g_sensor_queue, &data, 0, HAL_MAX_DELAY);
    	//status = xQueueReceive(sensor_queue, &data, 5000);

    	 if(status == osOK)
    	 {
			parse_timestamp(data.timestamp, &datetime);
			RTC_DateTimeToString((char*) writeBuf, &datetime);

			strncpy(current_file_name,writeBuf,8);
			current_file_name[8] = '\0';
			//change file
			if(strcmp(file_names[file_index], current_file_name) != 0){


				if(file_names[file_index][0] != '\0')
				{
					//close previoes file
					f_close(&fil);

					file_index = (file_index + 1) % MAX_DATA_FILES;

					//delete file name that about to be overidden
					if(file_names[file_index][0] != '\0'){
						f_unlink(file_names[file_index]);
					}

				}
				strcpy(file_names[file_index], current_file_name);

				//open file
				sprintf(whole_file_path,"0:/sensor/%s", file_names[file_index]);

			    fres = f_open(&fil, whole_file_path, FA_CREATE_ALWAYS | FA_WRITE);
			    if (fres != FR_OK) {
			    	printf("f_open error to write (%i)\r\n", fres);
			        while(1);
			    }
			    printf("f_open Success open %s to write\r\n",whole_file_path);
			}



			data_len = strlen((char*)writeBuf);

			sprintf(writeBuf + data_len, "\r\nTemperature: %f\r\nHumidity: %f\r\nLight: %d\r\nMode: Safe\r\n ---------------\r\n",
					data.temp, data.humid, data.light);

			data_len = strlen((char*)writeBuf);
			fres = f_write(&fil, writeBuf, data_len, &bytesWrote);

			if (fres == FR_OK) {
				//printf("writing total bytes %i:\r\n",bytesWrote);
				//printf("%s\r\n", writeBuf);
			} else {
				printf("f_write error (%i)\r\n", fres);
			}
			f_sync(&fil);
    	 }
    	 else{
    		 printf("failed to get data,Status: %d\r\n", status);
    	 }
    }
}



