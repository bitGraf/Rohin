#pragma once

#include <Engine/core/Platform.hpp>

/* std library */
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <cmath>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <limits.h>
#include <stdio.h>

#include <inttypes.h>

#include <cstring>
#include <fstream>

#include <time.h>
#include <chrono>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* basic engine needs */
#include "Engine/Core/Base.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Benchmark.hpp"
#include "Engine/Core/DataTypes.hpp"

/* Include Windows sdk */
#ifdef RH_PLATFORM_WINDOWS
	//#include <Windows.h>
#endif