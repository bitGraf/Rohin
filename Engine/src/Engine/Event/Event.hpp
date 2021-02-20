#ifndef _EMS_HPP_
#define _EMS_HPP_

#include <Engine/Core/base.hpp>

#include <functional>
#include <string>

namespace Engine {

    enum class EventType {
        none = 0,
        eveWindowClose, eveWindowResize, eveWindowMove, eveWindowFocus, eveWindowLoseFocus,
        eveKeyPress, eveKeyRelease, eveKeyTyped,
        eveMouseButtonPress, eveMouseButtonRelease, eveMouseMove, eveMouseScroll,
        eveCheckFileMod, eveFileUpdate,
        eventTotal // total number of events
    };

    enum EventFilter {
        none = 0,
        EventFilterGame = 1 << 0,
        EventFilterInput = 1 << 1,
        EventFilterKeyboard = 1 << 2,
        EventFilterMouse = 1 << 3,
        EventFilterMouseButton = 1 << 4
    };

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    class Event
    {
    public:
        virtual ~Event() = default;

        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventFilter category)
        {
            return GetCategoryFlags() & category;
        }
    };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event)
            : m_Event(event)
        {
        }

        // F will be deduced by the compiler
        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled = func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }
    private:
        Event & m_Event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }

}



#endif