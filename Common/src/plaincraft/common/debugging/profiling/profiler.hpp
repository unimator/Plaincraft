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

#ifndef PLAINCRAFT_COMMON_PROFILER
#define PLAINCRAFT_COMMON_PROFILER

#include "../../utils/fixed_list.hpp"
#include <string>
#include <chrono>
#include <array>
#include <map>

namespace plaincraft_common
{
    constexpr uint32_t profiling_history_length = 60 * 10;

    class Profiler final
    {
    private:
        static Profiler instance_;

        Profiler();

    public:
        struct ProfileInfo
        {
            friend class Profiler;

            std::string name;

        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start_;
            std::chrono::time_point<std::chrono::high_resolution_clock> end_;
        };

        class ProfileDescription
        {
            friend class Profiler;
        public:
            using ProfileValues = std::FixedList<std::chrono::milliseconds, profiling_history_length>;

        private:
            std::string name_;
            ProfileValues values_;

            ProfileDescription(std::string name);
            void SaveValue(std::chrono::milliseconds value);

        public:
            std::string GetName();
            ProfileValues& GetValues();
        };

    private:
        std::map<std::string, ProfileDescription> descriptions_;

    public:
        std::map<std::string, ProfileDescription>& GetDescriptions();

        static Profiler& GetInstance();

        static void Start(ProfileInfo &profile_info);
        static void End(ProfileInfo &profile_info);
    };

#ifndef NDEBUG
#define MEASURE(name_param, code) \
    {                             \
    Profiler::ProfileInfo info;   \
    info.name = name_param;       \
    Profiler::Start(info);        \
    code                          \
    Profiler::End(info);          \
    }
#else
#define MEASURE(name, code) code
#endif // NDEBUG

}

#endif // PLAINCRAFT_COMMON_PROFILER