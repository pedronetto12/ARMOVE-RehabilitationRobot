#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>
#include <motor.h>
#include <celulas.h>
#include <robomouse.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define LED_BUILTIN 15

TaskHandle_t celula_carga;
TaskHandle_t LED;
TaskHandle_t Ctrlmotores;
TaskHandle_t Contrmouse;

bool Inicializado = false;

//tasks
void ContrMouse (void *parameters);
void ContrMotores (void *parameters);
void atuador (void *parameters);
void ledTask(void *parameters) ;
#endif