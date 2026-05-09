#pragma once
#include <stdio.h>

#define CLT_LOG(format, ...) printf("[CLIENT] " format "\n", ##__VA_ARGS__);
#define CLT_LOG_ERROR(message) perror("[CLIENT] " message);