#pragma once

#define IRQ_PREFACE() asm("pushad\n" ::: "memory", "esp")
#define IRQ_SUFFIX()  asm("popad\nleave\niretd\n" ::: "memory", "esp")
