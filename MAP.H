#ifndef MAP_H
#define MAP_H

#include "game.h"

extern Waypoint waypoints[];
extern int waypoint_count;

void initMap();
void drawGameBackground();
void drawPath();

#endif