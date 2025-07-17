#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <celulas.h>
#include <robomouse.h>
#include "esp_timer.h"

//pinos motores
#define motorFE_STEP_PIN 14
#define motorFE_DIR_PIN 27

#define motorTE_STEP_PIN 5
#define motorTE_DIR_PIN 18

#define motorFD_STEP_PIN 13
#define motorFD_DIR_PIN 12

#define motorTD_STEP_PIN 16
#define motorTD_DIR_PIN 17


#define minVel 450
#define maxVel 150
#define pot 0.55
#define k 0.11



struct MOTOR {
    int Interpolacao;
    int Velocidade;
    int pulseOUTPUT;
    int cont;
    int contSTEP_memory;
};


//funções
void MotorInit();
void moveByLoadCells(LoadCellData CellData);
void SuavizacaoVelocidade();
void motorRun();
void printdebug(LoadCellData CellData);
void onTimer(void* arg); //interrupção

#endif