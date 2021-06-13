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

#include <utility>
#include "events_manager.hpp"

namespace plaincraft_core {
	EventsManager::EventsManager()
	{
	}

	EventsManager::EventsManager(EventsManager&& other) noexcept
	{
		events_observers_ = std::move(other.events_observers_);
	}

	EventsManager& EventsManager::operator=(EventsManager& other) noexcept
	{
		events_observers_ = std::move(other.events_observers_);

		return *this;
	}

	void EventsManager::Trigger(const Event& event){
		const auto event_type = event.GetType();
		auto event_observers = events_observers_[event_type];

		for (auto event_observer : event_observers) {
			event_observer->OnEventTriggered(event);
		}
	}

	void EventsManager::Trigger(Event&& event){
		Trigger(event);
	}

	void EventsManager::Subscribe(EventType eventType, std::shared_ptr<EventObserver> observer)
	{
		events_observers_[eventType].push_back(observer);
	}
}