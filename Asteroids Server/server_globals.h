#pragma once
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

inline float GetAngle(float x1, float y1, float x2, float y2) {
	return atan2(y1 - y2, x1 - x2);
}
inline float GetDistance(float x1, float y1, float x2, float y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}