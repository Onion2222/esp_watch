


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
//#include "driver/i2c.h"

#include "../components/PCF8563/PCF8563.h"




void app_main(void){


    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("\n============INFO============\n");
    printf("MODEL:     %s\nREVISION:  %d\nCPU CORES: %d\nCOM:       WiFi%s%s\nFLASH      %dMB/%s",
            (chip_info.model == CHIP_ESP32) ? "ESP32" : "ESP32S2",
            chip_info.revision,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
            spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    printf("\n============================\n\n");


    PCF8563_init();
    /*
    PCF8563_set_seconds( 30);
    PCF8563_set_minutes( 35);
    PCF8563_set_hours( 19);
    PCF8563_set_days( 18);
    PCF8563_set_weekdays( 2);
    PCF8563_set_months( 5);
    PCF8563_set_years( 21);
    */
    

   struct datetime current_datetime;

    for (int i = 120; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i*10);

        get_datetime(&current_datetime);
        printf("\n============HEURE============\n");
        printf("HEURE : [%d:%d:%d]\n", current_datetime.time.hours,current_datetime.time.minutes,current_datetime.time.seconds);
        printf("DATE  : [%s %d %s %d]",weekdays_to_string(current_datetime.date.weekdays),current_datetime.date.days, months_to_string(current_datetime.date.months), current_datetime.date.years);
        printf("\n=============================\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}


