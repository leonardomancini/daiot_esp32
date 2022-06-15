#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "DHT22.h"
#include "smart_config.h"
#include "nvs_flash.h"
#include "mqtt_task.h"
#include "sntp_time.h"
/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO0: input, pulled up, interrupt disabled, onboard button
 * GPIO2: output, interrupt disabled, onbuard ledterrupt on GPIO4/5
 *
 */

#define GPIO_OUT_0     2
#define GPIO_IN_0      0
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUT_0))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_IN_0))
#define ESP_INTR_FLAG_DEFAULT 0

#define LED_CHANGE_DELAY_MS    250
#define GPIO_OUT_DHT22 4
#define DHT22_READ_TIMER_MS 10000


uint8_t led_state = 0;

static void led_toggle_task(void* arg)
{

    while(true) {
     
       gpio_set_level(GPIO_OUT_0 , led_state);
        

        vTaskDelay(LED_CHANGE_DELAY_MS / portTICK_PERIOD_MS);
        
    }
}




static void counter_task(void* arg)
{
    int cnt = 0;
    while(true) {
        printf("DAIoT Counter_Task - Counts: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
     ESP_ERROR_CHECK( nvs_flash_init() );
    // GPIO OUTPUTS CONFIG
    //zero-initialize the config structure.
    gpio_config_t out_conf = {};
    //disable interrupt
    out_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    out_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    out_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    out_conf.pull_down_en = 0;
    //disable pull-up mode
    out_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&out_conf);

    // GPIO INPUTS CONFIG
    //zero-initialize the config structure.
    gpio_config_t in_conf = {};
    //disable interrupt
    in_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    in_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    in_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //disable pull-down mode
    in_conf.pull_down_en = 0;
    //enable pull-up mode
    in_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&in_conf);

    setDHTgpio(GPIO_OUT_DHT22);
    initialize_sntp();
    initialise_wifi();
    // 5 seg delay
    printf("Waiting 5 sec\n");
    vTaskDelay(5000 / portTICK_RATE_MS);

    
    // 5 seg delay
    printf("Waiting 5 sec\n");
    vTaskDelay(5000 / portTICK_RATE_MS);

    xTaskCreate(mqtt_app_main_task, "mqtt_app_task", 4096 * 8, NULL, 3, NULL);

    xTaskCreate(publicar_temperatura_task, "temp_pub_task", 4096 * 8, NULL, 3, NULL);

    xTaskCreate(led_toggle_task, "led_toggle_task", 4096 * 8, NULL, 3, NULL);


    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    while(1) {
        vTaskDelay(100 / portTICK_RATE_MS);
        // idle
     /*   if (gpio_get_level(GPIO_IN_0) == 0) {

            printf("Button pressed\n");

            if (counter_task_handle != NULL) {
                vTaskDelete(counter_task_handle);
                counter_task_handle = NULL;
                printf("counter_task deleted\n");
            }

        }    */    
    }
}
