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

#ifndef PLAINCRAFT_COMMON_EVENT_TRIGGER
#define PLAINCRAFT_COMMON_EVENT_TRIGGER

#include "event_subscription.hpp"
#include "event_trigger_base.hpp"
#include <vector>
#include <memory>

namespace plaincraft_common
{

    template<class... Args>
    class EventTrigger : EventTriggerBase
    {
    private:
        using EventSubscriptionBasePointer = std::unique_ptr<EventSubscriptionBase<Args...>>;
        std::vector<EventSubscriptionBasePointer> subscriptions_;

    public:
        void Trigger(Args... args) 
        {
            for(auto& subscription : subscriptions_) 
            {
                subscription->Call(args...);
            }
        }

        template<class T>
        void AddSubscription(T* target, void (T::*callback)(Args... args))
        {
            auto subscription = std::make_unique<EventSubscription<T, Args...>>(target, callback);
            subscriptions_.push_back(std::move(subscription));
        }
    };
}

#endif // PLAINCRAFT_COMMON_EVENT_TRIGGER