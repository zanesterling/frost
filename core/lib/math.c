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
