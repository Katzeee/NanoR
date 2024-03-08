#pragma once

#ifdef __linux__
#include "platform/window_linux.h"
using Window = nanoR::WindowLinux;
#elif defined(_WIN32)
#include "platform/window_linux.h"
using Window = nanoR::WindowLinux;
#endif