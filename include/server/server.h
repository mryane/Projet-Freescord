#pragma once
#include <stdbool.h>

typedef void* srv_handle;

#define SRV_NULL_HANDLE NULL

srv_handle srv_alloc();

bool srv_init(srv_handle);
void srv_close(srv_handle);

void srv_tick(srv_handle handle);
