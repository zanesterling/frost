#pragma once

#include <bootinfo.h>
#include <hal.h>
#include <stdio.h>
#include <keyboard.h>
#include <pmem.h>

#include "exception.h"
#include "process.h"
#include "scheduler.h"
#include "shell.h"
#include "vmem.h"

int init();

void memory_summary(multiboot_info* bootinfo);
