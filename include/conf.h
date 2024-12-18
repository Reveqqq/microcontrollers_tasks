struct BlinkParams
{
    uint32 channel_id;
    uint32 start;
    uint32 end;
    uint32 delay_time;
    uint32 duration;
    bool is_pwm;
};

const int time_to_long_pressed = 500;

xTaskHandle blink_wave_handle;
xTaskHandle blink_1_handle;
xTaskHandle blink_2_handle;
xTaskHandle blink_3_handle;
xTaskHandle blink_4_handle;

struct BlinkParams params1 = {
    .start = 0,
    .channel_id = 0,
    .end = 1000,
    .delay_time = 100,
    .duration = 1000,
    .is_pwm = false,
};

struct BlinkParams params2 = {
    .start = 0,
    .channel_id = 1,
    .end = 1000,
    .delay_time = 50,
    .duration = 500,
    .is_pwm = true,
};

struct BlinkParams params3 = {
    .start = 0,
    .channel_id = 2,
    .end = 1000,
    .delay_time = 50,
    .duration = 1000,
    .is_pwm = true,
};

struct BlinkParams params4 = {
    .start = 0,
    .channel_id = 3,
    .end = 1000,
    .delay_time = 100,
    .duration = 500,
    .is_pwm = false,
};

struct BlinkParams params_wave = {
    .start = 0,
    .channel_id = -1,
    .end = 300,
    .delay_time = 50,
    .duration = 1000,
    .is_pwm = true,
};