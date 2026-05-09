#pragma once
#include <stdbool.h>

#define CLT_NULL_HANDLE NULL

typedef void* clt_handle;

clt_handle clt_alloc();
bool clt_init(clt_handle);
void clt_close(clt_handle);

void clt_tick(clt_handle);