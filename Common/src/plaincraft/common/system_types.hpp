/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Gorka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef PLAINCRAFT_COMMON_SYSTEM_TYPES
#define PLAINCRAFT_COMMON_SYSTEM_TYPES

#define NOMINMAX

#include <algorithm>
#include <cstring>
#include <sstream>
#include <fstream>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <array>
#include <cstdint>
#include <optional>

#include "common.hpp"

namespace plaincraft_common {
	template <typename T>
	auto Max(T&& value) -> T {
		return value;
	}

	template <typename TFirst, typename TSecond, typename ... TArgs>
	auto Max(TFirst first, TSecond second, TArgs... args) -> typename std::common_type<TFirst, TSecond, TArgs...>::type {
		if (first > second) {
			return Max(first, std::forward<TArgs>(args)...);
		}
		else {
			return Max(second, std::forward<TArgs>(args)...);
		}
	}

	template <typename T>
	auto Min(T&& value) -> T {
		return value;
	}

	template <typename TFirst, typename TSecond, typename ... TArgs>
	auto Min(TFirst first, TSecond second, TArgs... args) -> typename std::common_type<TFirst, TSecond, TArgs...>::type {
		if (first < second) {
			return Max(first, std::forward<TArgs>(args)...);
		}
		else {
			return Max(second, std::forward<TArgs>(args)...);
		}
	}

	const auto FLOAT_MAX = std::numeric_limits<float>::max();
}
#endif // PLAINCRAFT_COMMON_SYSTEM_TYPES