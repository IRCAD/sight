/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "viz/scene2d/vec2d.hpp"

#include <Qt>

namespace sight::viz::scene2d::data
{

//-----------------------------------------------------------------------------

/**
 * @brief This class manage events on the scene 2D (mouse event, keyboard event ,
 * ...).
 */
class Event
{
public:

    typedef enum
    {
        no_t,
        Resize,   // change within the size of the widget/view
        Viewport, // change within the size of the sene (contained into the widget/view)
        MouseButtonDoubleClick,
        MouseButtonPress,
        MouseButtonRelease,
        MouseMove,
        MouseWheelUp,
        MouseWheelDown,
        KeyPress,
        KeyRelease,
        EnterEvent,
        LeaveEvent
    } Type;

    typedef enum
    {
        NoButton,
        LeftButton,
        RightButton,
        MidButton
    } Button;

    typedef enum
    {
        NoModifier,
        ShiftModifier,
        ControlModifier,
        AltModifier
    } Modifier;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Event()
    = default;

    [[nodiscard]] bool isAccepted() const;
    void setAccepted(bool _accepted);

    [[nodiscard]] Button getButton() const;
    void setButton(Button _button);

    [[nodiscard]] const vec2d_t& getCoord() const;
    void setCoord(const vec2d_t& _coord);

    [[nodiscard]] int getKey() const;
    void setKey(int _key);

    [[nodiscard]] Modifier getModifier() const;
    void setModifier(Modifier _modifier);

    [[nodiscard]] const vec2d_t& getOldSize() const;
    void setOldSize(const vec2d_t& _old_size);

    [[nodiscard]] const vec2d_t& size() const;
    void setSize(const vec2d_t& _size);

    [[nodiscard]] Type getType() const;
    void setType(Type _type);

private:

    vec2d_t m_coord {};
    Type m_type {no_t};
    Button m_button {NoButton};
    Modifier m_modifier {NoModifier};
    vec2d_t m_size {};
    vec2d_t m_oldSize {};
    bool m_accepted {false};
    int m_key {Qt::Key_unknown};
};

//------------------------------------------------------------------------------

inline bool Event::isAccepted() const
{
    return m_accepted;
}

//------------------------------------------------------------------------------

inline void Event::setAccepted(bool _accepted)
{
    m_accepted = _accepted;
}

//------------------------------------------------------------------------------

inline Event::Button Event::getButton() const
{
    return m_button;
}

//------------------------------------------------------------------------------

inline void Event::setButton(Button _button)
{
    m_button = _button;
}

//------------------------------------------------------------------------------

inline const vec2d_t& Event::getCoord() const
{
    return m_coord;
}

//------------------------------------------------------------------------------

inline void Event::setCoord(const vec2d_t& _coord)
{
    m_coord = _coord;
}

//------------------------------------------------------------------------------

inline int Event::getKey() const
{
    return m_key;
}

//------------------------------------------------------------------------------

inline void Event::setKey(int _key)
{
    m_key = _key;
}

//------------------------------------------------------------------------------

inline Event::Modifier Event::getModifier() const
{
    return m_modifier;
}

//------------------------------------------------------------------------------

inline void Event::setModifier(Modifier _modifier)
{
    m_modifier = _modifier;
}

//------------------------------------------------------------------------------

inline const vec2d_t& Event::getOldSize() const
{
    return m_oldSize;
}

//------------------------------------------------------------------------------

inline void Event::setOldSize(const vec2d_t& _old_size)
{
    m_oldSize = _old_size;
}

//------------------------------------------------------------------------------

inline const vec2d_t& Event::size() const
{
    return m_size;
}

//------------------------------------------------------------------------------

inline void Event::setSize(const vec2d_t& _size)
{
    m_size = _size;
}

//------------------------------------------------------------------------------

inline Event::Type Event::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

inline void Event::setType(Type _type)
{
    m_type = _type;
}

} // namespace sight::viz::scene2d::data
