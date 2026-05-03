#pragma once
#include <stdbool.h>

#define CLT_NULL_HANDLE NULL

typedef void* CLTHandle;

CLTHandle CLTAlloc();
bool CLTInit(CLTHandle);
void CLTClose(CLTHandle);

bool CLTTick(CLTHandle);