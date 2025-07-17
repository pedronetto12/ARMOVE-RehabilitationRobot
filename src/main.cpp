#include <main.h>

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);// desliga o brownout detector, se nao o modulo fica resetando
  Serial.begin(115200);

  xTaskCreatePinnedToCore(ledTask, "LED", 1024, NULL, 1, NULL, 1);
  CellInit(); //inicializa e configura a leitura das celulas de carga
  MotorInit(); //inicializa as configs dos motores
  MouseInit(); //inicializa as configs do Mouse

  xTaskCreatePinnedToCore(atuador,"celula_carga",1024 * 2, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(ContrMotores,"Ctrlmotores",1024 * 2, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(ContrMouse,"Contrmouse",1024 * 2, NULL, 3, NULL, 1);
  Inicializado = true;
}

void loop() {}

void ContrMotores (void *parameters){
  const TickType_t xDelay = pdMS_TO_TICKS(1);  // 2ms -> ticks
  while(1){
    motorRun();
    vTaskDelay(xDelay);  // Espera 2ms antes de rodar de novo
  }
}

void atuador (void *parameters){
  const TickType_t yDelay = pdMS_TO_TICKS(1);  // 2ms -> ticks
  while(1){
    leituraCelulas();
    vTaskDelay(yDelay);  // Espera 2ms antes de rodar de novo
  }
}

void ContrMouse (void *parameters){
  const TickType_t zDelay = pdMS_TO_TICKS(20);  // 20ms -> ticks
  while(1){
    MoveCursor(getLoadCellValues());
    vTaskDelay(zDelay);  // Espera 20ms antes de rodar de novo
  }
}


void ledTask(void *parameters) {
  const TickType_t delayTime = pdMS_TO_TICKS(300); // 300ms
  pinMode(LED_BUILTIN, OUTPUT);
  while (1) {
    if(Inicializado){
      digitalWrite(LED_BUILTIN, HIGH);
    }else{
      digitalWrite(LED_BUILTIN, HIGH);
      vTaskDelay(delayTime);
      digitalWrite(LED_BUILTIN, LOW);
      vTaskDelay(delayTime);
    }
  }
}