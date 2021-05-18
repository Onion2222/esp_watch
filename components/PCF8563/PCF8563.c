
#include "driver/i2c.h"
#include "esp_system.h"
#include "./PCF8563.h"

#define DEBUG 0

static void PCF8563_send_i2c(uint8_t slave_register, uint8_t data){
    //creation de command link
    i2c_cmd_handle_t cmd_handle_rtc = i2c_cmd_link_create();
    //start
    i2c_master_start(cmd_handle_rtc);
    //adresse
    i2c_master_write_byte(cmd_handle_rtc, I2C_RTC_ADDR_W, 0); //ack oui


    //register & data
    i2c_master_write_byte(cmd_handle_rtc, slave_register, 0); //ack oui
    i2c_master_write_byte(cmd_handle_rtc, data, 0); //ack oui

    //stop
    i2c_master_stop(cmd_handle_rtc);


    i2c_master_cmd_begin(I2C_RTC_PORT, cmd_handle_rtc,5000);

    //supression du command link
    i2c_cmd_link_delete(cmd_handle_rtc);
}


static void PCF8563_read_i2c(uint8_t slave_register, uint8_t *pointer_data){
    

    //creation de command link
    i2c_cmd_handle_t cmd_handle_rtc = i2c_cmd_link_create();
    //start
    i2c_master_start(cmd_handle_rtc);
    #if DEBUG
    printf("> START\n");
    #endif
    
    //adresse
    i2c_master_write_byte(cmd_handle_rtc, I2C_RTC_ADDR_W, 0); //ack oui
    #if DEBUG
    printf("> 0x%02x\n", I2C_RTC_ADDR_W);
    #endif

    //register
    i2c_master_write_byte(cmd_handle_rtc, slave_register, 0); //ack oui
    #if DEBUG
    printf("> 0x%02x\n", slave_register);
    #endif

    //start
    i2c_master_start(cmd_handle_rtc);
    #if DEBUG
    printf("> START\n");
    #endif

    //adresse
    i2c_master_write_byte(cmd_handle_rtc, I2C_RTC_ADDR_R, 0); //ack oui
    #if DEBUG
    printf("> 0x%02x\n", I2C_RTC_ADDR_R);
    #endif

    //read data
    esp_err_t err = i2c_master_read_byte(cmd_handle_rtc, pointer_data, 1);
    #if DEBUG
    printf("< READ\n");
    #endif

    if (err != ESP_OK) {
        printf("ERREUR I2C READ");
    }
    //stop
    i2c_master_stop(cmd_handle_rtc);
    #if DEBUG
    printf("> STOP\n");
    #endif
    
    //lancement de la communication
    i2c_master_cmd_begin(I2C_RTC_PORT, cmd_handle_rtc,5000);
    #if DEBUG
    printf("READ: %02x\n", *pointer_data);
    #endif
    //supression du command link
    i2c_cmd_link_delete(cmd_handle_rtc);

}

esp_err_t PCF8563_init(){

    //configuration du slave RTC
    i2c_config_t i2c_conf_RTC = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_RTC_SDA_IO,         // select GPIO specific to your project
        .sda_pullup_en = true,
        .scl_io_num = I2C_RTC_SCL_IO,         // select GPIO specific to your project
        .scl_pullup_en = true,
        .master.clk_speed = I2C_RTC_FREQ_HZ  // select frequency specific to your project
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };

    //creation du driver
    esp_err_t err = i2c_param_config(I2C_RTC_PORT, &i2c_conf_RTC);
    if (err != ESP_OK) {
        printf("ERREUR I2C 1");
        return err;
    }
    err = i2c_driver_install(I2C_RTC_PORT, i2c_conf_RTC.mode, 0, 0, 0);
    if (err != ESP_OK) {
        printf("ERREUR I2C 2");
        return err;
    }

    //envoie configuration initial
    
    PCF8563_send_i2c(0x00, 0x00);    // 0 0 0 0 0 000 
    
    return ESP_OK;
}


static uint8_t PCF8563_reg_to_value(uint8_t raw_reg, uint8_t mask_ten,uint8_t mask_unit){
    return ((raw_reg & mask_ten)>>4)*10 + (raw_reg & mask_unit);
}

static uint8_t PCF8563_value_to_reg(uint8_t raw_value){
    return (((raw_value-(raw_value%10))/10)<<4) + (raw_value%10);
}








uint8_t PCF8563_get_seconds(){
    uint8_t raw_seconds=0;
    PCF8563_read_i2c(REG_VL_SECONDS, &raw_seconds);
    return PCF8563_reg_to_value(raw_seconds,0x70,0x0F);
}
void PCF8563_set_seconds(uint8_t raw_seconds){
    PCF8563_send_i2c(REG_VL_SECONDS, PCF8563_value_to_reg(raw_seconds%60));
}

uint8_t PCF8563_get_minutes(){
    uint8_t raw_minutes=0;
    PCF8563_read_i2c(REG_MINUTES, &raw_minutes);
    return  PCF8563_reg_to_value(raw_minutes,0x70,0x0F);
}
void PCF8563_set_minutes(uint8_t raw_minutes){
    PCF8563_send_i2c(REG_MINUTES, PCF8563_value_to_reg(raw_minutes%60));
}

uint8_t PCF8563_get_hours(){
    uint8_t raw_hours=0;
    PCF8563_read_i2c(REG_HOURS, &raw_hours);
    return  PCF8563_reg_to_value(raw_hours,0x30,0x0F);
}
void PCF8563_set_hours(uint8_t raw_hours){
    PCF8563_send_i2c(REG_HOURS, PCF8563_value_to_reg(raw_hours%24));
}

uint8_t PCF8563_get_days(){
    uint8_t raw_days=0;
    PCF8563_read_i2c(REG_DAYS, &raw_days);
    return  PCF8563_reg_to_value(raw_days,0x30,0x0F);
}
void PCF8563_set_days(uint8_t raw_days){
    PCF8563_send_i2c(REG_DAYS, PCF8563_value_to_reg(raw_days));
}

char* weekdays_to_string(uint8_t num_weekdays){
    switch (num_weekdays)
    {
    case Sun:
        return "Dimanche"; //8 lettes max
        break;
    case Mon:
        return "Lundi";
        break;
    case Tue:
        return "Mardi";
        break;
    case Wed:
        return "Mercredi";
        break;
    case Thur:
        return "Jeudi";
        break;
    case Fri:
        return "Vendredi";
        break;
    case Sat:
        return "Samedi";
        break;
    default:
        return "erreur";
        break;
    }
}
char* PCF8563_get_weekdays(){
    uint8_t raw_weekdays=0;
    PCF8563_read_i2c(REG_WEEKDAYS, &raw_weekdays);
    return weekdays_to_string(raw_weekdays & 0x07);
}
void PCF8563_set_weekdays(uint8_t raw_weekdays){
    PCF8563_send_i2c(REG_WEEKDAYS, PCF8563_value_to_reg(raw_weekdays));
}



char* months_to_string(uint8_t num_month){
    switch (num_month)
    {
    case Jan:
        return "Janvier";
        break;
    case Feb:
        return "Fevrier";
        break;
    case Mar:
        return "Mars";
        break;
    case Apr:
        return "Avril";
        break;
    case May:
        return "Mai";
        break;
    case Jun:
        return "Juin";
        break;
    case Jul:
        return "Juillet";
        break;
    case Aug:
        return "Aout";
        break;
    case Sep:
        return "Septembre";
        break;
    case Oct:
        return "Octobre";
        break;
    case Nov:
        return "Novembre";
        break;
    case Dec:
        return "Decembre";
        break;
    default:
        return "erreur";
        break;
    }

}
uint8_t PCF8563_get_months(){
    uint8_t raw_months=0;
    PCF8563_read_i2c(REG_CENTURY_MONTHS, &raw_months);
    return  PCF8563_reg_to_value(raw_months,0x10,0x0F);
}
char* PCF8563_get_months_string(){
    uint8_t raw_months=0;
    PCF8563_read_i2c(REG_CENTURY_MONTHS, &raw_months);
    return months_to_string(PCF8563_reg_to_value(raw_months,0x10,0x0F));
 
}
void PCF8563_set_months(uint8_t raw_months){
    PCF8563_send_i2c(REG_CENTURY_MONTHS, PCF8563_value_to_reg(raw_months));
}

uint8_t PCF8563_get_years(){
    uint8_t raw_years=0;
    PCF8563_read_i2c(REG_YEARS, &raw_years);
    return  PCF8563_reg_to_value(raw_years,0xF0,0x0F);
}
void PCF8563_set_years(uint8_t raw_years){
    PCF8563_send_i2c(REG_YEARS, PCF8563_value_to_reg(raw_years));
}






void get_datetime(struct datetime *var_datetime){

    //creation de command link
    i2c_cmd_handle_t cmd_handle_rtc = i2c_cmd_link_create();
    //start
    i2c_master_start(cmd_handle_rtc);
    #if DEBUG
    printf("> START\n");
    #endif
    
    //adresse
    i2c_master_write_byte(cmd_handle_rtc, I2C_RTC_ADDR_W, 0); //ack oui
    #if DEBUG
    printf("> 0x%02x\n", I2C_RTC_ADDR_W);
    #endif

    //register
    i2c_master_write_byte(cmd_handle_rtc, REG_VL_SECONDS, 0); //ack oui
    #if DEBUG
    printf("> 0x%02x\n",REG_VL_SECONDS);
    #endif

    //start
    i2c_master_start(cmd_handle_rtc);
    #if DEBUG
    printf("> START\n");
    #endif

    //adresse
    i2c_master_write_byte(cmd_handle_rtc, I2C_RTC_ADDR_R, 0); //ack oui
    #if DEBUG
    printf("> 0x%02x\n", I2C_RTC_ADDR_R);
    #endif

    
    //lecture attention utilisation de pointer bordelique
    i2c_master_read_byte(cmd_handle_rtc, &(var_datetime->time.seconds), 0);
    i2c_master_read_byte(cmd_handle_rtc, &(var_datetime->time.minutes), 0);
    i2c_master_read_byte(cmd_handle_rtc, &(var_datetime->time.hours), 0);

    i2c_master_read_byte(cmd_handle_rtc, &(var_datetime->date.days), 0);
    i2c_master_read_byte(cmd_handle_rtc, &(var_datetime->date.weekdays), 0);
    i2c_master_read_byte(cmd_handle_rtc, &(var_datetime->date.months), 0);
    i2c_master_read_byte(cmd_handle_rtc, &(var_datetime->date.years), 1);

    //read data
    #if DEBUG
    printf("< READ x 3\n");
    #endif


    //stop
    i2c_master_stop(cmd_handle_rtc);
    #if DEBUG
    printf("> STOP\n");
    #endif
    i2c_master_cmd_begin(I2C_RTC_PORT, cmd_handle_rtc,5000);
    


    //traitement valeurs:
    var_datetime->time.seconds=PCF8563_reg_to_value(var_datetime->time.seconds,0x70,0x0F);
    var_datetime->time.minutes=PCF8563_reg_to_value(var_datetime->time.minutes,0x70,0x0F);
    var_datetime->time.hours=PCF8563_reg_to_value(var_datetime->time.hours,0x30,0x0F);

    var_datetime->date.days=PCF8563_reg_to_value(var_datetime->date.days,0x30,0x0F);
    var_datetime->date.weekdays=var_datetime->date.weekdays & 0x07;
    var_datetime->date.months=PCF8563_reg_to_value(var_datetime->date.months,0x10,0x0F);
    var_datetime->date.years=PCF8563_reg_to_value(var_datetime->date.years,0xF0,0x0F);



    //supression du command link
    i2c_cmd_link_delete(cmd_handle_rtc);
}
