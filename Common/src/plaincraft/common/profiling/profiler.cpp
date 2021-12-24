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

#include "profiler.hpp"
#include <iostream>

namespace plaincraft_common
{
    Profiler Profiler::instance_ = Profiler();

    Profiler::Profiler()
    {
    }

    Profiler& Profiler::GetInstance()
    {
        return instance_;
    }

    void Profiler::Start(ProfileInfo& profile_info)
    {
        profile_info.start_ = std::chrono::high_resolution_clock::now();
    }
   
    void Profiler::End(ProfileInfo& profile_info)
    {
        profile_info.end_ = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(profile_info.end_ - profile_info.start_);

        if(!instance_.descriptions_.count(profile_info.name))
        {
            auto pair = std::pair<std::string, ProfileDescription>(profile_info.name, profile_info.name);
            instance_.descriptions_.insert(pair);
        }
        instance_.descriptions_.at(profile_info.name).SaveValue(duration);
    }

    std::map<std::string, Profiler::ProfileDescription>& Profiler::GetDescriptions() 
    {
        return descriptions_;
    }

    Profiler::ProfileDescription::ProfileDescription(std::string name)
    : name_(name) {}

    void Profiler::ProfileDescription::SaveValue(std::chrono::milliseconds value)
    {
        values_[write_index_] = value;

        write_index_ = (write_index_ + 1) % profiling_history_length;
    }

    std::string Profiler::ProfileDescription::GetName()
    {
        return name_;
    }

    Profiler::ProfileDescription::ProfileValues Profiler::ProfileDescription::GetValues()
    {
        return values_;
    }
}