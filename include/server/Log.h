#pragma once

/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

#include <stdio.h>

#define SRV_LOG(format, ...) printf("[SERVER] " format "\n", ##__VA_ARGS__);
#define SRV_LOG_ERROR(message) perror("[SERVER] " message);