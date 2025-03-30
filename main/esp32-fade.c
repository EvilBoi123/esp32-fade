#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LED_GPIO_1 2  // LED 1 GPIO
#define LED_GPIO_2 5  // LED 2 GPIO
#define FADE_TIME_1 100
#define FADE_TIME_2 2000

void fade_led_task(void *pvParameters) {
    int channel = (int)pvParameters;

    while (1) {
        // Fade In
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, channel, 1023, 
                                     (channel == LEDC_CHANNEL_0) ? FADE_TIME_1 : FADE_TIME_2, 
                                     LEDC_FADE_NO_WAIT);
        vTaskDelay(pdMS_TO_TICKS((channel == LEDC_CHANNEL_0) ? FADE_TIME_1 : FADE_TIME_2));

        // Fade Out
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, channel, 0, 
                                     (channel == LEDC_CHANNEL_0) ? FADE_TIME_1 : FADE_TIME_2, 
                                     LEDC_FADE_NO_WAIT);
        vTaskDelay(pdMS_TO_TICKS((channel == LEDC_CHANNEL_0) ? FADE_TIME_1 : FADE_TIME_2));
    }
}

void app_main() {
    // Configure LEDC Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,  // Use SAME timer for both channels
        .duty_resolution = LEDC_TIMER_10_BIT,  
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configure LEDC Channels
    ledc_channel_config_t ledc_channel[] = {
        {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .timer_sel = LEDC_TIMER_0,
            .intr_type = LEDC_INTR_DISABLE,
            .gpio_num = LED_GPIO_1,
            .duty = 0,
            .hpoint = 0
        },
        {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_1,
            .timer_sel = LEDC_TIMER_0,
            .intr_type = LEDC_INTR_DISABLE,
            .gpio_num = LED_GPIO_2,
            .duty = 0,
            .hpoint = 0
        }
    };

    for (int i = 0; i < 2; i++) {
        ledc_channel_config(&ledc_channel[i]);
    }

    // Enable LEDC fade function
    ledc_fade_func_install(0);

    // Create separate tasks for each LED channel
    xTaskCreate(fade_led_task, "fade_led_0", 2048, (void *)LEDC_CHANNEL_0, 1, NULL);
    xTaskCreate(fade_led_task, "fade_led_1", 2048, (void *)LEDC_CHANNEL_1, 1, NULL);
}


