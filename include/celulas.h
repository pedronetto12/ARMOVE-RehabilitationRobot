#ifndef _CELULAS_H_
#define _CELULAS_H_

#include <HX711_ADC.h>

#define HX711_dout_cellXp  19
#define HX711_sck_cellXp 21
#define HX711_dout_cellYp 26
#define HX711_sck_cellYp 25
#define HX711_dout_cellXn 33
#define HX711_sck_cellXn 32
#define HX711_dout_cellYn 22
#define HX711_sck_cellYn 23

#define calValueCellXp 43.53
#define calValueCellYp 43.22
#define calValueCellXn 41.06
#define calValueCellYn 43.50

#define stabilizingtime 1000
#define _tare 1

//parametros de controle
#define CellDeadZone 100
#define CellMax 2000
#define minWheelSpeed 0
#define maxWheelSpeed 400

// Variáveis de controle
extern HX711_ADC LoadCellXp, LoadCellXn, LoadCellYp, LoadCellYn; // (HX711_dout, HX711_sck)

extern volatile bool newDataReadyXp, newDataReadyXn, newDataReadyYp, newDataReadyYn;

struct LoadCellData {
    float pesoXp;
    float pesoXn;
    float pesoYp;
    float pesoYn;
    float forward;
    float sideways;
};

// Funções de leitura
void CellInit();
void leituraCelulas();
float removeNegative(float value);
LoadCellData getLoadCellValues();
float linearInterpolate(float input, float inMin, float inMax, float outMin, float outMax);
void dataReadyISRXp();
void dataReadyISRXn();
void dataReadyISRYp();
void dataReadyISRYn();

#endif