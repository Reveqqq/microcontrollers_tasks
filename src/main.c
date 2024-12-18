#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "math.h"

#include "common.h"
#include "init.h"
#include "conf.h"
#include "blink_funcs.h"

int MODE = 0;

void button_up_short()
{
    if (is_blinking)
    {
        // Обновляем канал для всех параметров
        const int max_channels = 4;
        params1.channel_id = (params1.channel_id + 1) % max_channels;
        params2.channel_id = (params2.channel_id + 1) % max_channels;
        params3.channel_id = (params3.channel_id + 1) % max_channels;
        params4.channel_id = (params4.channel_id + 1) % max_channels;
    }
    else if (is_waving)
    {
        // Параметры длительности волны
        int durations[] = {1000, 500, 300, 100};
        const int num_durations = sizeof(durations) / sizeof(durations[0]);
        for (int i = 0; i < num_durations; i++)
        {
            if (params_wave.duration == durations[i])
            {
                params_wave.duration = durations[(i + 1) % num_durations];
                break;
            }
        }
    }
    printf("button up short\n");
}

void button_up_long()
{
    MODE = (MODE + 1) % 2;
    printf("button up long\n");
}

void button_daemon()
{
    // 0 - pressed, 16 - released

    uint32_t gpio_status_last = gpio_input_get() & inp_bit_1;
    uint32_t time = get_time_ms();

    while (1)
    {
        uint32_t gpio_status = gpio_input_get() & inp_bit_1;

        if (gpio_status_last != 0 && gpio_status == 0)
        {
            // нажали кнопку
            time = get_time_ms();
        }

        else if (gpio_status_last == 0 && gpio_status != 0)
        {
            // сколько прошло времени
            uint32_t time_diff = get_time_ms() - time;

            printf("time diff: %d\n", time_diff);

            if (time_diff > 2500)
            {
                // останавливаем остальные потоки
                if (is_blinking)
                    stop_blink_tasks();
                else if (is_waving)
                    stop_blink_wave_task();

                // проставляем нулевой шим, чтобы все лампочки перестали гореть
                set_all((int[]){0, 1, 2, 3}, 4, 0);
            }

            else if (time_diff > time_to_long_pressed)
                button_up_long(); // переключаем режим

            else
                button_up_short();
        }

        gpio_status_last = gpio_status;
        delay(50);
    }
}

void blink_daemon()
{
    int MODE_PAST = MODE;

    create_blink_tasks();
    create_blink_wave_task();

    while (1)
    {
        if (MODE_PAST == MODE)
        {
            delay(50);
            continue;
        }

        MODE_PAST = MODE;
        printf("MODE: %d\n", MODE);

        if (is_blinking)
        {
            stop_blink_tasks();
            set_all((int[]){0, 1, 2, 3}, 4, 0);
            resume_blink_wave_task();

            is_blinking = false;
            is_waving = true;
        }
        else if (is_waving)
        {
            stop_blink_wave_task();
            set_all((int[]){0, 1, 2, 3}, 4, 0);
            resume_blink_tasks();

            is_blinking = true;
            is_waving = false;
        }
        else
        {
            resume_blink_tasks();
            is_blinking = true;
        }
    }
}

void user_init(void)
{
    init_uart(); // устанавливаем частоту для передачи данных между пк и мк
    init_pwm();  // настройка шима
    init_inp();

    // создаем потоки на опрос кнопки и таски для мерцания лампочек
    xTaskCreate(&button_daemon, "button_daemon", 256, NULL, 1, NULL);
    xTaskCreate(&blink_daemon, "blink_daemon", 1024, NULL, 2, NULL);
}