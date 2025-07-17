#ifndef _ROBOMOUSE_H_
#define _ROBOMOUSE_H_

#include <BleMouse.h>
#include <celulas.h>

#define maxMouse 10
#define minMouse 0

// Funções
void MouseInit();
void MoveCursor(LoadCellData CellData);
#endif