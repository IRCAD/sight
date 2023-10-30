/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "event.hpp"

#include <core/com/signal_base.hpp>
#include <core/com/signals.hpp>

#include <viz/scene3d/window_interactor.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::signals::key_t event::TRIGGERED = "triggered";

using interaction_info = sight::viz::scene3d::window_interactor::interaction_info;

enum class mouse_buttons : std::uint8_t
{
    none   = 0,
    left   = 0 << 1,
    middle = 0 << 2,
    right  = 0 << 3
};

//------------------------------------------------------------------------------

std::uint8_t& operator|=(std::uint8_t& _a, mouse_buttons _b)
{
    return _a |= static_cast<std::uint8_t>(_b);
}

//------------------------------------------------------------------------------

std::uint8_t operator&(std::uint8_t _a, mouse_buttons _b)
{
    return _a & static_cast<std::uint8_t>(_b);
}

event::event()
{
    new_signal<triggered_signal_t>(TRIGGERED);
}

//------------------------------------------------------------------------------

bool event::check(
    interaction_info::interaction_enum _type,
    std::optional<mouse_button> _button,
    std::optional<modifier> _modifiers,
    std::optional<int> _key
)
{
    bool ok = m_filters.empty();
    for(const filter& filter : m_filters)
    {
        if(std::ranges::find(filter.type, _type) == filter.type.end())
        {
            continue;
        }

        if(_button)
        {
            mouse_buttons mouse_button = mouse_buttons::none;
            if(*_button == left)
            {
                mouse_button = mouse_buttons::left;
            }
            else if(*_button == middle)
            {
                mouse_button = mouse_buttons::middle;
            }
            else if(*_button == right)
            {
                mouse_button = mouse_buttons::right;
            }

            if(!(filter.buttons == 0 || ((filter.buttons & mouse_button) != 0U)))
            {
                continue;
            }
        }
        else if(filter.buttons != 0)
        {
            continue;
        }

        if(!((!filter.modifiers && !_modifiers)
             || (filter.modifiers && _modifiers && *filter.modifiers == *_modifiers)))
        {
            continue;
        }

        if(!((filter.keys.empty() && !_key) || (_key && std::ranges::find(filter.keys, _key) != filter.keys.end())))
        {
            continue;
        }

        ok = true;
    }

    return ok;
}

//------------------------------------------------------------------------------

void event::mouse_move_event(mouse_button _button, modifier _mods, int _x, int _y, int _dx, int _dy)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::mousemove;
    if(check(interaction_type, _button, _mods, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.button           = _button;
        info.modifiers        = _mods;
        info.x                = _x;
        info.y                = _y;
        info.dx               = _dx;
        info.dy               = _dy;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::wheel_event(modifier _mods, double _angle_delta, int _x, int _y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::wheelmove;
    if(check(interaction_type, {}, _mods, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.modifiers        = _mods;
        info.delta            = _angle_delta;
        info.x                = _x;
        info.y                = _y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::button_release_event(mouse_button _button, modifier _mods, int _x, int _y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::buttonrelease;
    if(check(interaction_type, _button, _mods, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.button           = _button;
        info.modifiers        = _mods;
        info.x                = _x;
        info.y                = _y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::button_press_event(mouse_button _button, modifier _mods, int _x, int _y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::buttonpress;
    if(check(interaction_type, _button, _mods, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.button           = _button;
        info.modifiers        = _mods;
        info.x                = _x;
        info.y                = _y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::button_double_press_event(mouse_button _button, modifier _mods, int _x, int _y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::buttondoublepress;
    if(check(interaction_type, _button, _mods, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.button           = _button;
        info.modifiers        = _mods;
        info.x                = _x;
        info.y                = _y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::key_press_event(int _key, modifier _mods, int _mouse_x, int _mouse_y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::keypress;
    if(check(interaction_type, {}, _mods, _key))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.key              = _key;
        info.modifiers        = _mods;
        info.x                = _mouse_x;
        info.y                = _mouse_y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::key_release_event(int _key, modifier _mods, int _mouse_x, int _mouse_y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::keyrelease;
    if(check(interaction_type, {}, _mods, _key))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.key              = _key;
        info.modifiers        = _mods;
        info.x                = _mouse_x;
        info.y                = _mouse_y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::resize_event(int _width, int _height)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::resize;
    if(check(interaction_type, {}, {}, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.x                = _width;
        info.y                = _height;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::pinch_gesture_event(double _scale_factor, int _center_x, int _center_y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::pinch_gesture;
    if(check(interaction_type, {}, {}, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.delta            = _scale_factor;
        info.x                = _center_x;
        info.y                = _center_y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::pan_gesture_move_event(int _x, int _y, int _dx, int _dy)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::pan_gesture_move;
    if(check(interaction_type, {}, {}, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.x                = _x;
        info.y                = _y;
        info.dx               = _dx;
        info.dy               = _dy;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::pan_gesture_release_event(int _x, int _y, int _dx, int _dy)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::pan_gesture_release;
    if(check(interaction_type, {}, {}, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.x                = _x;
        info.y                = _y;
        info.dx               = _dx;
        info.dy               = _dy;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::long_tap_gesture_event(int _x, int _y)
{
    interaction_info::interaction_enum interaction_type = interaction_info::interaction_enum::long_tap_gesture;
    if(check(interaction_type, {}, {}, {}))
    {
        interaction_info info;
        info.interaction_type = interaction_type;
        info.x                = _x;
        info.y                = _y;
        signal<triggered_signal_t>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

static interaction_info::interaction_enum string_to_interaction_enum(const std::string& _s)
{
    if(_s == "mouseMove")
    {
        return interaction_info::interaction_enum::mousemove;
    }

    if(_s == "wheel")
    {
        return interaction_info::interaction_enum::wheelmove;
    }

    if(_s == "buttonRelease")
    {
        return interaction_info::interaction_enum::buttonrelease;
    }

    if(_s == "buttonPress")
    {
        return interaction_info::interaction_enum::buttonpress;
    }

    if(_s == "buttonDoublePress")
    {
        return interaction_info::interaction_enum::buttondoublepress;
    }

    if(_s == "keyPress")
    {
        return interaction_info::interaction_enum::keypress;
    }

    if(_s == "keyRelease")
    {
        return interaction_info::interaction_enum::keyrelease;
    }

    if(_s == "resize")
    {
        return interaction_info::interaction_enum::resize;
    }

    if(_s == "pinchGesture")
    {
        return interaction_info::interaction_enum::pinch_gesture;
    }

    if(_s == "panGestureMove")
    {
        return interaction_info::interaction_enum::pan_gesture_move;
    }

    if(_s == "panGestureRelease")
    {
        return interaction_info::interaction_enum::pan_gesture_release;
    }

    SIGHT_THROW("Unknown interaction: " + _s);
}

//------------------------------------------------------------------------------

void event::configuring()
{
    configure_params();

    const config_t config = this->get_config();

    auto [begin, end] = config.equal_range("event");
    for(const auto& [_, event] : boost::make_iterator_range(begin, end))
    {
        filter filter;
        std::vector<std::string> types;
        boost::split(types, event.get<std::string>("<xmlattr>.type"), boost::is_any_of(" "));
        std::ranges::transform(types, std::back_inserter(filter.type), string_to_interaction_enum);
        if(boost::optional<std::string> buttons = event.get_optional<std::string>("buttons"))
        {
            std::vector<std::string> button_list;
            boost::split(button_list, *buttons, boost::is_any_of(" "));
            for(const std::string& button : button_list)
            {
                std::string lower_button = boost::to_lower_copy(button);
                if(lower_button == "left")
                {
                    filter.buttons |= mouse_buttons::left;
                }
                else if(lower_button == "middle")
                {
                    filter.buttons |= mouse_buttons::middle;
                }
                else if(lower_button == "right")
                {
                    filter.buttons |= mouse_buttons::right;
                }
                else
                {
                    SIGHT_THROW("Invalid mouse button name: " + button);
                }
            }
        }

        if(boost::optional<std::string> modifiers = event.get_optional<std::string>("<xmlattr>.modifiers"))
        {
            filter.modifiers = modifier::none;
            std::vector<std::string> modifier_list;
            boost::split(modifier_list, *modifiers, boost::is_any_of(" "));
            for(const std::string& modifier : modifier_list)
            {
                std::string lower_modifier = boost::to_lower_copy(modifier);
                if(lower_modifier == "shift")
                {
                    *filter.modifiers |= modifier::shift;
                }
                else if(lower_modifier == "ctrl")
                {
                    *filter.modifiers |= modifier::control;
                }
                else if(lower_modifier == "alt")
                {
                    *filter.modifiers |= modifier::alt;
                }
                else if(lower_modifier == "meta")
                {
                    *filter.modifiers |= modifier::meta;
                }
                else if(lower_modifier == "none")
                {
                    // do nothing
                }
                else
                {
                    SIGHT_THROW("Invalid keyboard modifier name: " + modifier);
                }
            }
        }

        if(boost::optional<std::string> keys = event.get_optional<std::string>("<xmlattr>.keys"))
        {
            std::vector<std::string> key_list;
            boost::split(key_list, *keys, boost::is_any_of(" "));
            for(const std::string& key : key_list)
            {
                filter.keys.push_back(std::stoi(key, nullptr, 0));
            }
        }

        m_filters.push_back(filter);
    }
}

//------------------------------------------------------------------------------

void event::starting()
{
    this->initialize();
    this->render_service()->make_current();

    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    this->layer()->add_interactor(interactor);
}

//------------------------------------------------------------------------------

void event::updating()
{
}

//------------------------------------------------------------------------------

void event::stopping()
{
    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    this->layer()->remove_interactor(interactor);
}

} // namespace sight::module::viz::scene3d::adaptor.
