#pragma once

#include "io.h"

typedef struct {
	uint8_t shouldExit;
} ShellState;

void runShell(ShellState* state);
char* prompt(char* promptStr);
