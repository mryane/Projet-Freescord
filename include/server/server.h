#pragma once
#include <stdbool.h>

struct user_t;
typedef void* srv_handle;

#define SRV_NULL_HANDLE NULL

srv_handle srv_alloc();

bool srv_init(srv_handle);
void srv_close(srv_handle);

int srv_get_socket(srv_handle);
int srv_get_write_pipe(srv_handle);
void srv_remove_user(srv_handle, struct user_t*);

void srv_tick(srv_handle handle);
