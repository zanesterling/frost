#pragma once

#define FP_INFINITE (1.0 / 0.0)
#define FP_NAN      ((float)0x7f800000)

#define isfinite(x) ((x) != FP_NAN && (x) != FP_INFINITE && x != -FP_INFINITE)
