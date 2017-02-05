#pragma once

#include <stdint.h>
#include <float.h>

#define M_PI 3.14159265358979323
#define M_TAU (M_PI * 2)
#define SQRT_3 1.73205081

#define abs(x) (x < 0 ? -x : x)

float sin(float theta);
float cos(float theta);
float asin(float theta);
float acos(float theta);
float atan(float theta);

float sqrt(float x);
