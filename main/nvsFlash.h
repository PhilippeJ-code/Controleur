// nvsFlash.h
// 
#pragma once

#include "stdint.h"

void nvsInit();
uint16_t nvsRead(char* name);
void nvsWrite(char* name, uint16_t value);
