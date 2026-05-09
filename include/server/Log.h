#pragma once
#include <stdio.h>

#define SRV_LOG(format, ...) printf("[SERVER] " format "\n", ##__VA_ARGS__);
#define SRV_LOG_ERROR(message) perror("[SERVER] " message);