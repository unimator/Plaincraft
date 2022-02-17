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

#ifndef PLAINCRAFT_COMMON_CACHE
#define PLAINCRAFT_COMMON_CACHE

#include <string>
#include <map>
#include <memory>

namespace plaincraft_core
{
    template <typename TCacheObject, typename TKey = std::string>
    class Cache
    {
    private:
        std::map<TKey, std::shared_ptr<TCacheObject>> data_;

    public:
        Cache() {}

        Cache(const Cache& other) = delete;
        Cache& operator=(const Cache& other) = delete;

        void Store(TKey key, std::shared_ptr<TCacheObject> value)
        {
            data_[key] = value;
        }

        std::shared_ptr<TCacheObject> Fetch(TKey key)
        {
            return data_[key];
        }

        bool Contains(TKey key)
        {
            return data_.contains(key);
        }

        void Remove(TKey key)
        {
            data_.erase(key);
        }
    };
}

#endif // PLAINCRAFT_COMMON_CACHE