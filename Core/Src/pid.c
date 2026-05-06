#include "pid.h"
#include "stm32f1xx.h"
#include <stdint.h>

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float setpoint) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;

    pid->d_filtered = 0.0f;
    pid->setpoint = setpoint;

    pid->integral_error = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;

    pid->limit_max = 999.0f;
    pid->limit_min = 0.0f;
}

float PID_Compute(PID_Controller *pid, float current_pos) {
    float err = pid->setpoint - current_pos;
    float p_term = err * pid->Kp;
    
    float i_term = pid->integral_error * pid->Ki;

    // float d_term = (err - pid->prev_error) * pid->Kd;
    float d_raw = (current_pos - pid->prev_measurement);
    pid->d_filtered = 0.15f * d_raw + 0.85f * pid->d_filtered;
    float d_term = -pid->d_filtered * pid->Kd; 
    pid->prev_measurement = current_pos;
    pid->prev_error = err;

    float output = p_term + i_term + d_term;

    if (output < pid->limit_max && output > pid->limit_min) {
        pid->integral_error += err;
        if (pid->integral_error >  5000.0f) pid->integral_error =  5000.0f;
        if (pid->integral_error < -5000.0f) pid->integral_error = -5000.0f;
    }

    if (output > pid->limit_max) output = pid->limit_max;
    if (output < pid->limit_min) output = pid->limit_min;

    return output;
}