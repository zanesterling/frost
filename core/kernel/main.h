#pragma once

#include <hal.h>
#include <io.h>
#include <keyboard.h>

#include "exception.h"
#include "shell.h"

int init();

void get_cmd(char* cmd_buf, size_t buflen);
void run_cmd(char* cmd_buf);
