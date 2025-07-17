#include <robomouse.h>

BleMouse bleMouse("ARMOVE", "TCCPedroNetto", 100);

void MouseInit(){
  Serial.println("Inicio do Init Mouse");
  bleMouse.begin();
  Serial.println("Fim do Init Mouse");
}

void MoveCursor(LoadCellData CellData){
int y = linearInterpolate(CellData.forward, CellDeadZone, CellMax, minMouse, maxMouse);
int x = linearInterpolate(CellData.sideways, CellDeadZone, CellMax, minMouse, maxMouse);

  if(bleMouse.isConnected()) {
    bleMouse.move(x,-y,0);
  }
}