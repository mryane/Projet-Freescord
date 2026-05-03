#pragma once
#include <stdbool.h>

typedef void* SRVHandle;

#define SRV_NULL_HANDLE NULL

SRVHandle SRVAlloc();

bool SRVInit(SRVHandle);
void SRVClose(SRVHandle);

void SRVTick(SRVHandle handle);
