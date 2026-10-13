#pragma once
// Windows intrinsics must precede Rack's SIMD compatibility macros.
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#endif
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif
