bool is_blinking = false;
bool is_waving = false;

void blink_pwm(void *pvParameters)
{
    struct BlinkParams *params = (struct BlinkParams *)pvParameters;
    int now = params->start;

    int channel_id = params->channel_id;
    int start = params->start;
    int end = params->end;
    int delay_time = params->delay_time;
    int duration = params->duration;

    int step = floor((end - start) / (duration / delay_time));

    while (now < end)
    {
        pwm_set_duty(now, channel_id);
        pwm_start();
        delay(delay_time);
        now += step;
    }

    pwm_set_duty(end, channel_id);
    pwm_start();
    delay(delay_time);

    while (now > start)
    {
        pwm_set_duty(now, channel_id);
        pwm_start();
        delay(delay_time);
        now -= step;
    }

    pwm_set_duty(start, channel_id);
    pwm_start();
    delay(delay_time);
}

void blink_wave()
{
    int channel_id = 0;

    while (1)
    {
        params_wave.channel_id = channel_id;
        blink_pwm(&params_wave);
        channel_id = (channel_id + 1) % 4;
    }
}

void blink_task(void *pvParameters)
{
    struct BlinkParams *params = (struct BlinkParams *)pvParameters;
    int now = params->start;

    int channel_id;
    int start;
    int end;
    int delay_time;
    int duration;

    bool is_pwm = params->is_pwm;

    if (is_pwm)
    {
        while (1)
            blink_pwm(pvParameters);
    }
    else
    {
        while (1)
        {
            channel_id = params->channel_id;
            start = params->start;
            end = params->end;
            delay_time = params->delay_time;
            duration = params->duration;

            pwm_set_duty(end, channel_id);
            pwm_start();

            delay(delay_time);

            pwm_set_duty(start, channel_id);
            pwm_start();

            delay(duration);
        }
    }
}

void stop_blink_tasks()
{
    vTaskSuspend(blink_1_handle);
    vTaskSuspend(blink_2_handle);
    vTaskSuspend(blink_3_handle);
    vTaskSuspend(blink_4_handle);
    is_blinking = false;
}

void create_blink_tasks()
{
    xTaskCreate(&blink_task, "blink_task_1", 128, (void *)&params1, 3, &blink_1_handle);
    xTaskCreate(&blink_task, "blink_task_2", 128, (void *)&params2, 3, &blink_2_handle);
    xTaskCreate(&blink_task, "blink_task_3", 128, (void *)&params3, 3, &blink_3_handle);
    xTaskCreate(&blink_task, "blink_task_4", 128, (void *)&params4, 3, &blink_4_handle);
    stop_blink_tasks();
    set_all((int[]){0, 1, 2, 3}, 4, 0);
}

void resume_blink_tasks()
{
    vTaskResume(blink_1_handle);
    vTaskResume(blink_2_handle);
    vTaskResume(blink_3_handle);
    vTaskResume(blink_4_handle);
    is_blinking = true;
}

void stop_blink_wave_task()
{
    vTaskSuspend(blink_wave_handle);
    is_waving = false;
}

void create_blink_wave_task()
{
    xTaskCreate(&blink_wave, "blink_wave", 128, NULL, 3, &blink_wave_handle);
    stop_blink_wave_task();
    set_all((int[]){0, 1, 2, 3}, 4, 0);
}

void resume_blink_wave_task()
{
    vTaskResume(blink_wave_handle);
    is_waving = true;
}