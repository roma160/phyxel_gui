#pragma once

// PHYXEL ENGINE SETUP
#include "phyxel_config.h"

#define WINDOW_NAME "PhyxelEngine 1.0"
ImVec4 clear_color(0.45f, 0.55f, 0.60f, .00f);
float phyxel_size = 5; // actual pixels per phyxel
bool done = false;
char materialID = 0;
unsigned long long int iteration = 0;
bool play = false;