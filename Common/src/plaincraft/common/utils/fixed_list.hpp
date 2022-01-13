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

#ifndef PLAINCRAFT_COMMON_FIXED_QUEUE
#define PLAINCRAFT_COMMON_FIXED_QUEUE

#include <list>
#include <iterator>

namespace std
{
    template <typename T, int max_length, class Alloc = allocator<T>>
    class FixedList
    {
    private:
        std::list<T, Alloc> values_;

    public:
        void push_front(const T &value)
        {
            if(values_.size() == max_length)
            {
                values_.pop_back();
            }
            values_.push_front(value);
        }
        
        auto begin() noexcept -> decltype(values_.begin())
        {
            return values_.begin();
        }

        auto end() noexcept -> decltype(values_.end())
        {
            return values_.end();
        }
    };
}

#endif // PLAINCRAFT_COMMON_FIXED_QUEUE