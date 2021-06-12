/* ADC1 lecture
	Medición de un sensor analogo de temperatura LM35, haciendo uso del ADC del ESP32
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "math.h"

#include "esp_newlib.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"

#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/adc_common.h"
#include "esp_adc_cal.h"

/*------Ecuaciones para calcular el ppm-----*/

/* Ecuacion 1
 * note: No registra valores inferiores a 400ppm
 * pow(10, ((volts / 8.5) - pcurve[1]) / pcurve[2] + pcurve[0]);
 */

/*Ecuacion 2
 * nota: esta ecuacion se sacó de la grafica de voltaje vs ppm que
 * se encuentra en el datasheet
 * ppm = (volts - 2658) / (-0.0583)
 */


float CO2Curve = { 2.477, 0.3059, (0.030 / -1.52287)};


void delay_ms(int ms){
	vTaskDelay(ms/portTICK_PERIOD_MS);
}

int  MGGetPercentage(float volts, float *pcurve){
//    if ((volts / 8.5 )>= 0.220) {
//       return -1;
//    } else {
//        return pow(10, ((volts / 8.5) - pcurve[1]) / pcurve[2] + pcurve[0]);
//    }
    float ppm = (volts - 2658.0) / (-0.0583);;
    return ppm / 100;
}

float MGRead(){
    //int i;
    //float v = 0;
    gpio_set_direction(35, GPIO_MODE_INPUT);
    esp_adc_cal_characteristics_t *adc_chars = calloc(1,sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_10, 1114, adc_chars);
    adc1_config_width(ADC_WIDTH_BIT_10);
	adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

        uint32_t reading = adc1_get_raw(ADC1_CHANNEL_7);
        uint32_t voltage = esp_adc_cal_ra w_to_voltage(reading, adc_chars);
        esp_adc_cal_get_voltage(ADC1_CHANNEL_6,  adc_chars, &voltage);
    return voltage;
}

void main_task(void){
    while(1){
        int percentage;
        float volts;
        volts = MGRead();
        printf("voltaje: %d ppm\n", (char)volts);
        percentage = MGGetPercentage(volts, &CO2Curve);
        printf("CO2: %d ppm\n", percentage);
	    delay_ms(5000);
    }
}

void app_main(){


    xTaskCreate(&main_task, "maintask", 20000, NULL, 2, NULL);
}
