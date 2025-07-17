#include <motor.h>

  esp_timer_handle_t periodic_timer;
  //motores
  MOTOR Motor_FE={0,0,0,0,0};
  MOTOR Motor_TE={0,0,0,0,0};
  MOTOR Motor_FD={0,0,0,0,0};
  MOTOR Motor_TD={0,0,0,0,0};

void MotorInit(){
  Serial.println("Inicio do Init Motores");
  //timer interrupção
  const esp_timer_create_args_t periodic_timer_args = {
    .callback = &onTimer,
    .arg = nullptr,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "motor_step_timer"
  };

  esp_timer_create(&periodic_timer_args, &periodic_timer);
  esp_timer_start_periodic(periodic_timer, 50); // 50us

  pinMode(motorFE_STEP_PIN, OUTPUT);
  pinMode(motorFE_DIR_PIN, OUTPUT);

  pinMode(motorTE_STEP_PIN, OUTPUT);
  pinMode(motorTE_DIR_PIN, OUTPUT);

  pinMode(motorFD_STEP_PIN, OUTPUT);
  pinMode(motorFD_DIR_PIN, OUTPUT);

  pinMode(motorTD_STEP_PIN, OUTPUT);
  pinMode(motorTD_DIR_PIN, OUTPUT);

  delay(200);
  Serial.println("Fim do Init Motores");
}

void moveByLoadCells(LoadCellData CellData) {
  // Ajuste das velocidades com interpolação linear
  int speedForward = linearInterpolate(CellData.forward, CellDeadZone, CellMax, minWheelSpeed, maxWheelSpeed);
  int speedSideways = linearInterpolate(CellData.sideways, CellDeadZone, CellMax, minWheelSpeed, maxWheelSpeed);
   
  // Cálculo da decomposição de velocidade dos motores
  Motor_FE.Interpolacao = speedForward + speedSideways;  
  Motor_TE.Interpolacao = speedForward - speedSideways;
  Motor_FD.Interpolacao = speedForward - speedSideways;
  Motor_TD.Interpolacao = speedForward + speedSideways;

  // Normalização para evitar saturação
  int maxSpeed = max(abs(Motor_FE.Interpolacao), max(abs(Motor_TE.Interpolacao), max(abs(Motor_FD.Interpolacao), abs(Motor_TD.Interpolacao))));
  if (maxSpeed > maxWheelSpeed) {
    float scale = (float)maxWheelSpeed / maxSpeed;
    Motor_FE.Interpolacao *= scale;
    Motor_TE.Interpolacao *= scale;
    Motor_FD.Interpolacao *= scale;
    Motor_TD.Interpolacao *= scale;
 }
}


void SuavizacaoVelocidade(){
  // Calculo do Pulso do motor frente esquerda
  if(Motor_FE.Interpolacao > 0) Motor_FE.Velocidade = (minVel-maxVel)* tanh(k * pow(abs(Motor_FE.Interpolacao), pot));
  else  Motor_FE.Velocidade = -(minVel-maxVel)* tanh(k * pow(abs(Motor_FE.Interpolacao), pot));

  if(Motor_FE.Interpolacao > 0) Motor_FE.Velocidade = minVel - Motor_FE.Interpolacao;
  else if(Motor_FE.Interpolacao < 0) Motor_FE.Velocidade = -minVel - Motor_FE.Interpolacao;
  else Motor_FE.Velocidade = 0;

  // Calculo do Pulso do motor traseira esquerda
  if(Motor_TE.Interpolacao > 0) Motor_TE.Velocidade = (minVel-maxVel)* tanh(k * pow(abs(Motor_TE.Interpolacao), pot));
  else  Motor_TE.Velocidade = -(minVel-maxVel)* tanh(k * pow(abs(Motor_TE.Interpolacao), pot));

  if(Motor_TE.Interpolacao > 0) Motor_TE.Velocidade = minVel - Motor_TE.Interpolacao;
  else if(Motor_TE.Interpolacao < 0) Motor_TE.Velocidade = -minVel - Motor_TE.Interpolacao;
  else Motor_TE.Velocidade = 0;

  // Calculo do Pulso do motor frente direita
  if(Motor_FD.Interpolacao > 0) Motor_FD.Velocidade = (minVel-maxVel)* tanh(k * pow(abs(Motor_FD.Interpolacao), pot));
  else  Motor_FD.Velocidade = -(minVel-maxVel)* tanh(k * pow(abs(Motor_FD.Interpolacao), pot));

  if(Motor_FD.Interpolacao > 0) Motor_FD.Velocidade = minVel - Motor_FD.Interpolacao;
  else if(Motor_FD.Interpolacao < 0) Motor_FD.Velocidade = -minVel - Motor_FD.Interpolacao;
  else Motor_FD.Velocidade = 0;

  // Calculo do Pulso do motor traseira direita
  if(Motor_TD.Interpolacao > 0) Motor_TD.Velocidade = (minVel-maxVel)* tanh(k * pow(abs(Motor_TD.Interpolacao), pot));
  else  Motor_TD.Velocidade = -(minVel-maxVel)* tanh(k * pow(abs(Motor_TD.Interpolacao), pot));

  if(Motor_TD.Interpolacao > 0) Motor_TD.Velocidade = minVel - Motor_TD.Interpolacao;
  else if(Motor_TD.Interpolacao < 0) Motor_TD.Velocidade = -minVel - Motor_TD.Interpolacao;
  else Motor_TD.Velocidade = 0;

  Motor_FE.pulseOUTPUT = Motor_FE.Velocidade;
  Motor_TE.pulseOUTPUT = Motor_TE.Velocidade;
  Motor_FD.pulseOUTPUT = -Motor_FD.Velocidade; //negativo pois foram colocados do lado espelhado do esquerdo
  Motor_TD.pulseOUTPUT = -Motor_TD.Velocidade; //negativo pois foram colocados do lado espelhado do esquerdo

  /*
  Motor_FE.pulseOUTPUT = 120;
  Motor_TE.pulseOUTPUT = 120;
  Motor_FD.pulseOUTPUT = -120;
  Motor_TD.pulseOUTPUT = -120;
  */
}

void motorRun(){
  moveByLoadCells(getLoadCellValues());
  SuavizacaoVelocidade();
  //printdebug(getLoadCellValues());
}


void printdebug(LoadCellData CellData){
  Serial.print(Motor_FE.Interpolacao);
  Serial.print(",,");
  Serial.print(CellData.pesoYp);
  Serial.print("    ");

  Serial.print(Motor_TE.Interpolacao);
  Serial.print(",,");
  Serial.print(CellData.pesoYn);
  Serial.print("    ");

  Serial.print(Motor_FD.Interpolacao);
  Serial.print(",,");
  Serial.print(CellData.pesoXp);
  Serial.print("    ");

  Serial.print(Motor_TD.Interpolacao);
  Serial.print(",,");
  Serial.println(CellData.pesoXn);
}

void onTimer(void* arg){

  Motor_FE.cont++;
  if (Motor_FE.cont > Motor_FD.contSTEP_memory) {
    Motor_FE.cont = 0;
    Motor_FD.contSTEP_memory = Motor_FE.pulseOUTPUT;
    if(Motor_FD.contSTEP_memory != 0){
      if(Motor_FD.contSTEP_memory < 0){
        digitalWrite(motorFE_DIR_PIN,LOW); // Direção reversa (LOW)
        Motor_FD.contSTEP_memory *= -1;
      } else {
        digitalWrite(motorFE_DIR_PIN,HIGH);  // Direção frente (HIGH)
      }
    }
  }
  if (Motor_FE.cont == 1) {
    digitalWrite(motorFE_STEP_PIN,HIGH); // Inicio do pulso (HIGH)
  }
  else if (Motor_FE.cont == 2) {
    digitalWrite(motorFE_STEP_PIN,LOW); // Fim do pulso (LOW)
  }

  Motor_TE.cont++;
  if (Motor_TE.cont > Motor_TE.contSTEP_memory) {
    Motor_TE.cont = 0;
    Motor_TE.contSTEP_memory = Motor_TE.pulseOUTPUT;
    if(Motor_TE.contSTEP_memory != 0){
      if(Motor_TE.contSTEP_memory < 0){
        digitalWrite(motorTE_DIR_PIN,LOW); // Direção reversa (LOW)
        Motor_TE.contSTEP_memory *= -1;
      } else {
        digitalWrite(motorTE_DIR_PIN,HIGH);  // Direção frente (HIGH)
      }
    }
  }
  if (Motor_TE.cont == 1) {
    digitalWrite(motorTE_STEP_PIN,HIGH); // Inicio do pulso (HIGH)
  }
  else if (Motor_TE.cont == 2) {
    digitalWrite(motorTE_STEP_PIN,LOW); // Fim do pulso (LOW)
  }

  Motor_FD.cont++;
  if (Motor_FD.cont > Motor_FD.contSTEP_memory) {
    Motor_FD.cont = 0;
    Motor_FD.contSTEP_memory = Motor_FD.pulseOUTPUT;
    if(Motor_FD.contSTEP_memory != 0){
      if(Motor_FD.contSTEP_memory < 0){
        digitalWrite(motorFD_DIR_PIN,LOW); // Direção reversa (LOW)
        Motor_FD.contSTEP_memory *= -1;
      } else {
        digitalWrite(motorFD_DIR_PIN,HIGH);  // Direção frente (HIGH)
      }
    }
  }
  if (Motor_FD.cont == 1) {
    digitalWrite(motorFD_STEP_PIN,HIGH); // Inicio do pulso (HIGH)
  }
  else if (Motor_FD.cont == 2) {
    digitalWrite(motorFD_STEP_PIN,LOW); // Fim do pulso (LOW)
  }

  Motor_TD.cont++;
  if (Motor_TD.cont > Motor_TD.contSTEP_memory) {
    Motor_TD.cont = 0;
    Motor_TD.contSTEP_memory = Motor_TD.pulseOUTPUT;
    if(Motor_TD.contSTEP_memory != 0){
      if(Motor_TD.contSTEP_memory < 0){
        digitalWrite(motorTD_DIR_PIN,LOW); // Direção reversa (LOW)
        Motor_TD.contSTEP_memory *= -1;
      } else {
        digitalWrite(motorTD_DIR_PIN,HIGH);  // Direção frente (HIGH)
      }
    }
  }
  if (Motor_TD.cont == 1) {
    digitalWrite(motorTD_STEP_PIN,HIGH); // Inicio do pulso (HIGH)
  }
  else if (Motor_TD.cont == 2) {
    digitalWrite(motorTD_STEP_PIN,LOW); // Fim do pulso (LOW)
  }


}