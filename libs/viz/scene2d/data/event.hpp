/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
class event
{
public:

    enum type
    {
        no_t,
        resize,   // change within the size of the widget/view
        viewport, // change within the size of the sene (contained into the widget/view)
        mouse_button_double_click,
        mouse_button_press,
        mouse_button_release,
        mouse_move,
        mouse_wheel_up,
        mouse_wheel_down,
        key_press,
        key_release,
        enter_event,
        leave_event
    };

    enum button
    {
        no_button,
        left_button,
        right_button,
        mid_button
    };

    enum modifier
    {
        no_modifier,
        shift_modifier,
        control_modifier,
        alt_modifier
    };

    /**
     * @brief Constructor
     */
    event()
    = default;

    [[nodiscard]] bool is_accepted() const;
    void set_accepted(bool _accepted);

    [[nodiscard]] button get_button() const;
    void set_button(button _button);

    [[nodiscard]] const vec2d_t& get_coord() const;
    void set_coord(const vec2d_t& _coord);

    [[nodiscard]] int get_key() const;
    void set_key(int _key);

    [[nodiscard]] modifier get_modifier() const;
    void set_modifier(modifier _modifier);

    [[nodiscard]] const vec2d_t& get_old_size() const;
    void set_old_size(const vec2d_t& _old_size);

    [[nodiscard]] const vec2d_t& size() const;
    void set_size(const vec2d_t& _size);

    [[nodiscard]] type type() const;
    void set_type(enum type _type);

private:

    vec2d_t m_coord {};
    enum type m_type {no_t};
    button m_button {no_button};
    modifier m_modifier {no_modifier};
    vec2d_t m_size {};
    vec2d_t m_old_size {};
    bool m_accepted {false};
    int m_key {Qt::Key_unknown};
};

//------------------------------------------------------------------------------

inline bool event::is_accepted() const
{
    return m_accepted;
}

//------------------------------------------------------------------------------

inline void event::set_accepted(bool _accepted)
{
    m_accepted = _accepted;
}

//------------------------------------------------------------------------------

inline event::button event::get_button() const
{
    return m_button;
}

//------------------------------------------------------------------------------

inline void event::set_button(button _button)
{
    m_button = _button;
}

//------------------------------------------------------------------------------

inline const vec2d_t& event::get_coord() const
{
    return m_coord;
}

//------------------------------------------------------------------------------

inline void event::set_coord(const vec2d_t& _coord)
{
    m_coord = _coord;
}

//------------------------------------------------------------------------------

inline int event::get_key() const
{
    return m_key;
}

//------------------------------------------------------------------------------

inline void event::set_key(int _key)
{
    m_key = _key;
}

//------------------------------------------------------------------------------

inline event::modifier event::get_modifier() const
{
    return m_modifier;
}

//------------------------------------------------------------------------------

inline void event::set_modifier(modifier _modifier)
{
    m_modifier = _modifier;
}

//------------------------------------------------------------------------------

inline const vec2d_t& event::get_old_size() const
{
    return m_old_size;
}

//------------------------------------------------------------------------------

inline void event::set_old_size(const vec2d_t& _old_size)
{
    m_old_size = _old_size;
}

//------------------------------------------------------------------------------

inline const vec2d_t& event::size() const
{
    return m_size;
}

//------------------------------------------------------------------------------

inline void event::set_size(const vec2d_t& _size)
{
    m_size = _size;
}

//------------------------------------------------------------------------------

inline enum event::type event::type() const
{
    return m_type;
}

//------------------------------------------------------------------------------

inline void event::set_type(enum type _type)
{
    m_type = _type;
}

} // namespace sight::viz::scene2d::data
