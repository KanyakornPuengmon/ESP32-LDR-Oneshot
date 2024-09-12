#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#include "esp_adc/adc_cali.h"

void app_main(void)
{
    // 1. config adc_oneshot variables
    int adc_read0;   // ตัวแปรสำหรับเก็บค่าที่อ่านจาก ADC
    int mv_output;   // ตัวแปรสำหรับเก็บค่าแรงดันไฟฟ้าที่แปลงจากค่า ADC

    adc_oneshot_unit_handle_t handle = NULL;  // ตัวแปร handle สำหรับ unit ของ ADC
    adc_oneshot_unit_init_cfg_t init_config1 = {  // โครงสร้างสำหรับกำหนดค่าเริ่มต้นของ ADC
        .unit_id = ADC_UNIT_1,                 // ใช้งาน ADC หน่วยที่ 1
        .ulp_mode = ADC_ULP_MODE_DISABLE,      // ปิดโหมด ULP (Ultra Low Power)
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));  // สร้าง unit ใหม่สำหรับ ADC ด้วยค่า config ที่กำหนด

    //2.config adc channel
    adc_oneshot_chan_cfg_t config = {

        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_0, &config));

    // 3. calibrate adc
    adc_cali_handle_t cali_handle = NULL;  // ตัวแปร handle สำหรับการคาลิเบรต
    adc_cali_line_fitting_config_t cali_config = {  // โครงสร้างสำหรับกำหนดค่าการคาลิเบรต
        .unit_id = ADC_UNIT_1,             // ใช้งาน ADC หน่วยที่ 1
        .atten = ADC_ATTEN_DB_12,          // การลดทอนสัญญาณ (attenuation) 12dB
        .bitwidth = ADC_BITWIDTH_DEFAULT,  // ความละเอียด bit width เริ่มต้นของ ADC
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle));  // สร้าง schema การคาลิเบรตแบบ line fitting

    double lux;
    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_CHANNEL_0, &adc_read0));
        printf("Adc channel-o raw read result %d \n ", adc_read0);
        printf("\n\n");
        adc_cali_raw_to_voltage(cali_handle, adc_read0, &mv_output);
        printf("ADC milivolt output %d \n", mv_output);
        lux = 0.2258*adc_read0 - 338.57; //ยังไม่เสร็จ
        printf("Lux value = %f\n",lux);
        vTaskDelay(500/portTICK_PERIOD_MS);


    }
   
}
