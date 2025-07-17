#include <celulas.h>

portMUX_TYPE cellMux = portMUX_INITIALIZER_UNLOCKED;

HX711_ADC LoadCellXp(HX711_dout_cellXp, HX711_sck_cellXp);
HX711_ADC LoadCellXn(HX711_dout_cellXn, HX711_sck_cellXn);
HX711_ADC LoadCellYp(HX711_dout_cellYp, HX711_sck_cellYp);
HX711_ADC LoadCellYn(HX711_dout_cellYn, HX711_sck_cellYn);

volatile bool newDataReadyXp = false;  // Variáveis de controle
volatile bool newDataReadyXn = false;
volatile bool newDataReadyYp = false;
volatile bool newDataReadyYn = false;

LoadCellData CellData = {0.0, 0.0, 0.0, 0.0};   // Variáveis para armazenar os valores de cada célula

void CellInit(){
    Serial.println("Inicio do Init Celulas");
    //inicialização celular Xp
    Serial.print("CELULA Xp:");
    LoadCellXp.setSamplesInUse(1);
    LoadCellXp.begin(); //config
    LoadCellXp.start(stabilizingtime, _tare);//zerando a balança
    if (LoadCellXp.getTareTimeoutFlag()) {
        Serial.println("Tempo limite, verifique a fiação MCU>HX711 e as designações dos pinos");
        while (1);
    }else {  
        LoadCellXp.setCalFactor(calValueCellXp); // define novo fator de calibração, os dados brutos são divididos por este valor para converter em dados legíveis
        Serial.println("Startup is complete");
    }
    attachInterrupt(digitalPinToInterrupt(HX711_dout_cellXp), dataReadyISRXp, FALLING);

    //inicialização celular Xn
    Serial.print("CELULA Xn:");
    LoadCellXn.setSamplesInUse(1);
    LoadCellXn.begin(); //config
    LoadCellXn.start(stabilizingtime, _tare);//zerando a balança
    if (LoadCellXn.getTareTimeoutFlag()) {
        Serial.println("Tempo limite, verifique a fiação MCU>HX711 e as designações dos pinos");
        while (1);
    }else {  
        LoadCellXn.setCalFactor(calValueCellXn); // define novo fator de calibração, os dados brutos são divididos por este valor para converter em dados legíveis
        Serial.println("Startup is complete");
    }
    attachInterrupt(digitalPinToInterrupt(HX711_dout_cellXn), dataReadyISRXn, FALLING);

    //inicialização celular Yp
    Serial.print("CELULA Yp:");
    LoadCellYp.setSamplesInUse(1);
    LoadCellYp.begin(); //config
    LoadCellYp.start(stabilizingtime, _tare);//zerando a balança
    if (LoadCellYp.getTareTimeoutFlag()) {
        Serial.println("Tempo limite, verifique a fiação MCU>HX711 e as designações dos pinos");
        while (1);
    }else{  
        LoadCellYp.setCalFactor(calValueCellYp); //define novo fator de calibração, os dados brutos são divididos por este valor para converter em dados legíveis
        Serial.println("Startup is complete");
    }
    attachInterrupt(digitalPinToInterrupt(HX711_dout_cellYp), dataReadyISRYp, FALLING);

    //inicialização celular Yn
    Serial.print("CELULA Yn:");
    LoadCellYn.setSamplesInUse(1);
    LoadCellYn.begin(); //config
    LoadCellYn.start(stabilizingtime, _tare);//zerando a balança
    if (LoadCellYn.getTareTimeoutFlag()) {
        Serial.println("Tempo limite, verifique a fiação MCU>HX711 e as designações dos pinos");
        while (1);
    }else{  
        LoadCellYn.setCalFactor(calValueCellYn); //define novo fator de calibração, os dados brutos são divididos por este valor para converter em dados legíveis
        Serial.println("Startup is complete");
    }
    attachInterrupt(digitalPinToInterrupt(HX711_dout_cellYn), dataReadyISRYn, FALLING);
    delay(200);
    Serial.println("Fim do Init Celulas");
}

void leituraCelulas() {
  portENTER_CRITICAL(&cellMux);
  // Verifica se os dados estão prontos para a célula Xp
  if (newDataReadyXp) {
    newDataReadyXp = 0;  // Reseta a flag
    CellData.pesoXp=LoadCellXp.getData();
  }
  
  // Verifica se os dados estão prontos para a célula Xn
  if (newDataReadyXn) {
    newDataReadyXn = 0;  // Reseta a flag
    CellData.pesoXn=LoadCellXn.getData();
  }

  // Verifica se os dados estão prontos para a célula Yp
  if (newDataReadyYp) {
    newDataReadyYp = 0;  // Reseta a flag
    CellData.pesoYp=LoadCellYp.getData();
  }

  // Verifica se os dados estão prontos para a célula Yn
  if (newDataReadyYn) {
    newDataReadyYn = 0;  // Reseta a flag
    CellData.pesoYn=LoadCellYn.getData();
  }
  portEXIT_CRITICAL(&cellMux);
}

float removeNegative(float value) {
    return (value > 0) ? value : 0;
}

LoadCellData getLoadCellValues() {
  // Aqui deve entrar a lógica de leitura real das células de carga
  // Suponha que estamos atribuindo valores lidos diretamente
  CellData.pesoXp = removeNegative(CellData.pesoXp);
  CellData.pesoXn = removeNegative(CellData.pesoXn);
  CellData.pesoYp = removeNegative(CellData.pesoYp);
  CellData.pesoYn = removeNegative(CellData.pesoYn);

  //cria os pontos no plano cartesiano
  CellData.forward = (CellData.pesoYp - CellData.pesoYn);
  CellData.sideways = (CellData.pesoXp - CellData.pesoXn);
 
  return CellData;  // Retorna os valores das células de carga
}

float linearInterpolate(float input, float inMin, float inMax, float outMin, float outMax) {
    // Se dentro da zona morta, retorna 0 (evita movimentação indesejada)
    if (abs(input) <= inMin) return 0;  
    // Fora da zona morta, inicia a partir de outMin e cresce linearmente
    return (input > 0 ? 1 : -1) * (outMin + ((abs(input) - inMin) / (float)(inMax - inMin)) * (outMax - outMin));
}

// Funções de interrupção para cada célula de carga
void dataReadyISRXp() {
  if (LoadCellXp.update()) {
    portENTER_CRITICAL_ISR(&cellMux);
    newDataReadyXp = 1;
    portEXIT_CRITICAL_ISR(&cellMux);
  }
}
void dataReadyISRXn() {
  if (LoadCellXn.update()) {
    portENTER_CRITICAL_ISR(&cellMux);
    newDataReadyXn = 1;
    portEXIT_CRITICAL_ISR(&cellMux);
  }
}
void dataReadyISRYp() {
  if (LoadCellYp.update()) {
    portENTER_CRITICAL_ISR(&cellMux);
    newDataReadyYp = 1;
    portEXIT_CRITICAL_ISR(&cellMux);
  }
}
void dataReadyISRYn() {
  if (LoadCellYn.update()) {
    portENTER_CRITICAL_ISR(&cellMux);
    newDataReadyYn = 1;
    portEXIT_CRITICAL_ISR(&cellMux);
  }
}
