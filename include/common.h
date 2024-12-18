#include "esp_common.h"

uint32_t get_time_ms(void)
{
    return system_get_time() / 1000; // Convert microseconds to milliseconds
}

void delay(int ms)
{
    vTaskDelay(ms / portTICK_RATE_MS);
}

void set_all(int *channels, int size, int value)
{
    for (int i = 0; i < size; i++)
    {
        delay(50);
        pwm_set_duty(value, channels[i]);
    }
    pwm_start();
}