#pragma once

/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

#include <stdio.h>

#define CLT_LOG(format, ...) printf("[CLIENT] " format "\n", ##__VA_ARGS__);
#define CLT_LOG_ERROR(message) perror("[CLIENT] " message);