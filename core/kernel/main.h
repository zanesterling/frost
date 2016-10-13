#pragma once

#include <bootinfo.h>
#include <hal.h>
#include <io.h>
#include <keyboard.h>
#include <pmem.h>

#include "exception.h"
#include "shell.h"

int init();

void memory_summary(multiboot_info* bootinfo);
