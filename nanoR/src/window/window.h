#pragma once

#ifdef __linux__
#include "platform/window_linux.h"
using Window = nanoR::WindowLinux;
#endif