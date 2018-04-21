// some_Maps.h
#ifndef MAPS_H
#define MAPS_H

#define HCMapArraySize 16384 //128 x 128 pixels
#define HCUnitSize 375 //25x15
#define UnitTransparent 0x2945


/* == Different Map Tiles ==
* 0: Ground, anyone can walk on
* 1: Trees, no cavaliers
* 2: ?
* 3: ?
* 4: Mountain, for now no move
* 5: Wall, no one can walk on

/* ================= MAPS ==============
* Map is the map graphic, Array is the tile type, Start is the starting location by IdNumber
 * temple: 4 enemies, lots of trees and lil house
 * valley: 3 enemies, go around
 * desert: 3 enemies, big mountain
 * other one: 
 * ruin: 5 enemies, boss battle??
 */

const uint8_t templeArray[8][8] = {
	{5,5,5,5,5,5,5,5},
	{1, 0, 1, 1, 5, 0, 0, 5},
	{1, 1, 0, 0, 5, 0, 0, 5},
	{0, 0, 0, 0, 1, 0, 0, 0},
	{0, 1, 1, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
};
const uint8_t templeStart[2][7] = {
	{3, 7}, {4,7}, {5, 7},
	{5,1}, {6,1}, {0, 3}, {0,4}
};

const uint8_t valleyArray[8][8] = {
	{5,5,5,5,5,5,5,5},
	{4, 4, 4, 4, 0, 0, 0, 0},
	{0, 0, 4, 4, 5, 0, 0, 0},
	{0, 0, 4, 4, 1, 0, 0, 0},
	{1, 1, 4, 4, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 0, 0, 1, 1},
};
const uint8_t valleyStart[2][6] = {
	{0, 3}, {0,4}, {1, 4},
	{7,1}, {6,2}, {7,3}
};

const uint8_t desertArray[8][8] = {
	{5,5,5,5,5,5,5,5},
	{5, 5, 0, 0, 0, 0, 0, 0},
	{5, 5, 5, 0, 0, 0, 0, 0},
	{5, 5, 5, 4, 0, 0, 0, 0},
	{5, 5, 5, 0, 0, 0, 0, 0},
	{5, 0, 0, 0, 0, 0, 0, 0},
	{5, 0, 0, 0, 0, 0, 0, 0},
	{5, 0, 0, 0, 0, 0, 0, 0},
};
const uint8_t desertStart[2][8] = {
	{2, 4}, {6,7}, {5,4},
	{4, 1}, {5,2}, {7,7}, {7, 6}, {6, 1}
};

const uint8_t ruinArray[8][8] = {
	{5,5,5,5,5,5,5,5},
	{5, 5, 0, 0, 0, 0, 0, 0},
	{5, 5, 5, 0, 0, 0, 0, 0},
	{5, 5, 5, 4, 0, 0, 0, 0},
	{5, 5, 5, 0, 0, 0, 0, 0},
	{5, 0, 0, 0, 0, 0, 0, 0},
	{5, 0, 0, 0, 0, 0, 0, 0},
	{5, 0, 0, 0, 0, 0, 0, 0},
};
const uint8_t ruinStart[2][8] = {
	{2, 4}, {6,7}, {5,4},
	{4, 1}, {5,2}, {7,7}, {7, 6}, {6, 1}
};

const uint8_t emptyStart[8][8] = {
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
}