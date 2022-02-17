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

#ifndef PLAINCRAFT_COMMON_EVENT_SUBSCRIPTION
#define PLAINCRAFT_COMMON_EVENT_SUBSCRIPTION

#include "event_subscription_base.hpp"

namespace plaincraft_common {

    template<class... Args>
    class EventTrigger;

    template<class T, class... Args>
    class EventSubscription : public EventSubscriptionBase<Args...> {
    private:
        T* target_;
        void (T::*callback_)(Args...);

        friend class EventTrigger<Args>;
    
    public:
        EventSubscription(T* target, void (T::*callback)(Args...)) 
            : target_(target), callback_(callback) {}
        
        void Call(Args... args) final {
            (target_->*callback_)(args...);
        }

        void* GetTarget() final {
            return target_;
        }
    };
}

#endif // PLAINCRAFT_COMMON_EVENT_SUBSCRIPTION