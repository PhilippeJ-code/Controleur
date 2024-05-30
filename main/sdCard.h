// sdCard.h
//
#pragma once

#define MOUNT_POINT "/sdcard"

void sdCardInit();

char sdCardMount();
char sdCardUnmount();
char sdCardPresent();
