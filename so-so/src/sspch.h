#pragma once

#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#pragma warning(disable: 4996)

#ifdef SS_PLATFORM_WINDOWS
	#include <Windows.h>
#endif // SS_PLATFORM_WINDOWS

#include <iostream>
#include <memory>
#include <utility> 
#include <algorithm>
#include <functional>
#include <cstdint>
#include <vector>
#include <sstream>
#include <string>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include "so-so/Core/core.h"
#include "so-so/Core/Log.h"

#include "glm/glm.hpp"
