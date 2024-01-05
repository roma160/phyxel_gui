#pragma once

#include "includes.h"

// PHYXEL ENGINE SETUP
#include "phyxel_config.h"

#define WINDOW_NAME "PhyxelEngine 1.0"
ImVec4 clear_color(0.45f, 0.55f, 0.60f, .00f);
float phyxel_size = 5; // actual pixels per phyxel
int brush_size = 2; // in phyxels
ImVec2ih brush_pos(0, 0); // in phyxels

bool done = false;
char materialID = 0;
unsigned long long int iteration = 0;
bool play = false;