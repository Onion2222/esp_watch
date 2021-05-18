
#ifndef _PCF8563_H
#define _PCF8563_H


#include "esp_system.h"

//Param HW
#define I2C_RTC_SDA_IO  21
#define I2C_RTC_SCL_IO  22
#define I2C_RTC_FREQ_HZ 1000
#define I2C_RTC_ADDR_R  0xA3
#define I2C_RTC_ADDR_W  0xA2 //I2C-bus slave address: read A3h and write A2h
#define I2C_RTC_PORT    0



//Registres
#define REG_CONTROL_STATUS_1    0x00
#define REG_CONTROL_STATUS_2    0x01
#define REG_VL_SECONDS          0x02
#define REG_MINUTES             0x03
#define REG_HOURS               0x04
#define REG_DAYS                0x05
#define REG_WEEKDAYS            0x06
#define REG_CENTURY_MONTHS      0x07
#define REG_YEARS               0x08
#define REG_MINUTE_ALARM        0x09
#define REG_HOUR_ALARM          0x0A
#define REG_DAY_ALARM           0x0B
#define REG_WEEKDAY_ALARM       0x0C
#define REG_CLKOUT8CONTROL      0x0D
#define REG_TIMER_CONTROL       0x0E
#define REG_TIMER               0x0F

//Other
#define YEARS_TO_ADD    2000

enum day_of_week {Sun, Mon, Tue, Wed, Thur, Fri, Sat};
enum months {Jan=1, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec};

//===STRUCT===//
struct time {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
};
struct date {
    uint8_t weekdays;
    uint8_t days;
    uint8_t months;
    uint8_t years;
    
};
struct datetime{
    struct time time;
    struct date date;
};


//===INIT===//
esp_err_t PCF8563_init();

//==GET TIME==//
uint8_t PCF8563_get_seconds();
uint8_t PCF8563_get_minutes();
uint8_t PCF8563_get_hours();
uint8_t PCF8563_get_days();
char*   PCF8563_get_weekdays();
uint8_t PCF8563_get_months();
char*   PCF8563_get_months_string();
uint8_t PCF8563_get_years();

void get_datetime(struct datetime *var_datetime);


//==SET TIME==//
void PCF8563_set_seconds(uint8_t raw_seconds);
void PCF8563_set_minutes(uint8_t raw_minutes);
void PCF8563_set_hours(uint8_t raw_hours);
void PCF8563_set_days(uint8_t raw_days);
void PCF8563_set_weekdays(uint8_t raw_weekdays);
void PCF8563_set_months(uint8_t raw_months);
void PCF8563_set_years(uint8_t raw_years);




//==ALARM==//


//===AUTRE==//
char* months_to_string(uint8_t num_month);
char* weekdays_to_string(uint8_t num_weekdays);

#endif