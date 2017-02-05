#include "math.h"

float sin(float theta) {
	// Normalize to the range [0, tau)
	while (theta < 0) theta += M_TAU;
	while (theta >= M_TAU) theta -= M_TAU;

	// Find quadrant and set pos/neg appropriately
	float factor;
	if (theta < M_TAU / 4) {
		factor = 1;
	} else if (theta < M_TAU * 2 / 4) {
		factor = 1;
		theta = M_PI - theta;
	} else if (theta < M_TAU * 3 / 4) {
		factor = -1;
		theta = theta - M_PI;
	} else {
		factor = -1;
		theta = M_TAU - theta;
	}

	// Ensure theta < 45*
	if (theta > M_PI / 4) return factor * cos(M_TAU / 4 - theta);

	// Compute
	#define theta_3 (theta * theta * theta)
	#define theta_5 (theta_3 * theta * theta)
	return factor * (theta - theta_3 / 6 + theta_5 / 120);
}

float cos(float theta) {
	// Normalize to the range [0, tau)
	while (theta < 0) theta += M_TAU;
	while (theta >= M_TAU) theta -= M_TAU;

	// Find quadrant and set pos/neg appropriately
	float factor;
	if (theta < M_TAU / 4) {
		factor = 1;
	} else if (theta < M_TAU * 2 / 4) {
		factor = -1;
		theta = M_PI - theta;
	} else if (theta < M_TAU * 3 / 4) {
		factor = -1;
		theta = theta - M_PI;
	} else {
		factor = 1;
		theta = M_TAU - theta;
	}

	// Ensure theta < 45*
	if (theta > M_PI / 4) return factor * sin(M_TAU / 4 - theta);

	// Compute
	#define theta_2 (theta * theta)
	#define theta_4 (theta_2 * theta_2)
	#define theta_6 (theta_4 * theta_2)
	return factor * (1 - theta_2 / 2 + theta_4 / 24 - theta_6 / 720);
}

float asin(float x) {
	return atan(x / sqrt(1 - x * x));
}

float acos(float x) {
	return atan(sqrt(1 - x * x) / x);
}

float atan(float x) {
	// Restrict to positive values.
	float factor_1 = 1;
	float offset_1 = 0;
	if (x < 0) {
		x = -x;
		factor_1 = -factor_1;
	}

	// Restrict to [0, 1].
	float factor_2 = 1;
	float offset_2 = 0;
	if (x > 1) {
		factor_2 = -1;
		offset_2 = M_PI / 2;
		x = 1 / x;
	}

	// Restrict to [0, 2 - sqrt(3)]
	float factor_3 = 1;
	float offset_3 = 0;
	if (x > 0.268) {
		offset_3 = M_PI / 6;
		x = (SQRT_3 * x - 1) / (SQRT_3 + x);
	}

	// Use the polynomial
	float result_3 = x - x*x*x / 3 + x*x*x*x*x / 5;
	float result_2 = result_3 * factor_3 + offset_3;
	float result_1 = result_2 * factor_2 + offset_2;
	return result_1 * factor_1 + offset_1;
}

float sqrt(float x) {
	float guess = x / 2;
	float epsilon = 0.0001;

	while (abs(guess * guess - x) > epsilon) {
		guess = guess / 2 + x / (2 * guess);
	}

	return guess;
}
