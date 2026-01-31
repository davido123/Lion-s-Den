#include "EventSystem.h"
#include <algorithm>

EventSystem& EventSystem::GetInstance() {
    static EventSystem instance;
    return instance;
}

EventSystem::EventID EventSystem::Subscribe(const std::string& eventName, EventCallback callback) {
    EventID id = _nextID++;
    _listeners[eventName].push_back({ id, callback });
    return id;
}

void EventSystem::Unsubscribe(const std::string& eventName, EventID id) {
    auto it = _listeners.find(eventName);
    if (it != _listeners.end()) {
        auto& subs = it->second;
        subs.erase(
            std::remove_if(subs.begin(), subs.end(),
                [id](const Subscription& sub) { return sub.id == id; }),
            subs.end()
        );
    }
}

void EventSystem::Emit(const std::string& eventName, void* data) {
    auto it = _listeners.find(eventName);
    if (it != _listeners.end()) {
        for (const auto& sub : it->second) {
            sub.callback(data);
        }
    }
}

void EventSystem::Clear() {
    _listeners.clear();
    _nextID = 1;
}
