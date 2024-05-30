// languages.h
//
#pragma once

#include "stdint.h"

void languageInit(uint16_t language);

extern char strJoystick[];
extern char strFiles[];
extern char strParameters[];
extern char strTools[];
extern char strGrblConnected[];
extern char strGrblDisconnected[];
extern char strSpeed[];
extern char strDistance[];
extern char strRead[];
extern char strOk[];
extern char strError[];
extern char strFilenameTooLong[];
extern char strNoSDReader[];
extern char strNoSDCard[];
extern char strFileReadError[];
extern char strEngraving[];
extern char strBuffer[];
extern char strCancel[];
extern char strEngrave[];
extern char strMeasure[];
extern char strSize[];

extern char strAlarms[10][40];
extern char strErrors[40][40];