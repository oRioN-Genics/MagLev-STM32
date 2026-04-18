#include "pid.h"
#include "stm32f1xx.h"
#include <stdint.h>

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float setpoint) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;

    pid->setpoint = setpoint;

    pid->integral_error = 0.0f;
    pid->prev_error = 0.0f;

    pid->limit_max = 999.0f;
    pid->limit_min = 0.0f;
}

float PID_Compute(PID_Controller *pid, float current_pos) {
    float err = pid->setpoint - current_pos;
    float p_term = err * pid->Kp;

    pid->integral_error += err;
    float i_term = pid->integral_error * pid->Ki;

    float d_term = (err - pid->prev_error) * pid->Kd;

    pid->prev_error = err;

    float output = p_term + i_term + d_term;
    if (output > pid->limit_max) {
        output = pid->limit_max;
    }
    if (output < pid->limit_min) {
        output = pid->limit_min;
    }

    return output;
}