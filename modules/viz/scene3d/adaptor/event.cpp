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

using InteractionInfo = sight::viz::scene3d::window_interactor::InteractionInfo;

enum class MouseButtons : std::uint8_t
{
    NONE   = 0,
    LEFT   = 0 << 1,
    MIDDLE = 0 << 2,
    RIGHT  = 0 << 3
};

//------------------------------------------------------------------------------

std::uint8_t& operator|=(std::uint8_t& _a, MouseButtons _b)
{
    return _a |= static_cast<std::uint8_t>(_b);
}

//------------------------------------------------------------------------------

std::uint8_t operator&(std::uint8_t _a, MouseButtons _b)
{
    return _a & static_cast<std::uint8_t>(_b);
}

event::event()
{
    new_signal<TriggeredSignal>(TRIGGERED);
}

//------------------------------------------------------------------------------

bool event::check(
    InteractionInfo::InteractionEnum _type,
    std::optional<MouseButton> _button,
    std::optional<Modifier> _modifiers,
    std::optional<int> _key
)
{
    bool ok = m_filters.empty();
    for(const Filter& filter : m_filters)
    {
        if(std::ranges::find(filter.type, _type) == filter.type.end())
        {
            continue;
        }

        if(_button)
        {
            MouseButtons mouse_button = MouseButtons::NONE;
            if(*_button == LEFT)
            {
                mouse_button = MouseButtons::LEFT;
            }
            else if(*_button == MIDDLE)
            {
                mouse_button = MouseButtons::MIDDLE;
            }
            else if(*_button == RIGHT)
            {
                mouse_button = MouseButtons::RIGHT;
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

void event::mouseMoveEvent(MouseButton _button, Modifier _mods, int _x, int _y, int _dx, int _dy)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::MOUSEMOVE;
    if(check(interaction_type, _button, _mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.button          = _button;
        info.modifiers       = _mods;
        info.x               = _x;
        info.y               = _y;
        info.dx              = _dx;
        info.dy              = _dy;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::wheelEvent(Modifier _mods, double _angle_delta, int _x, int _y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::WHEELMOVE;
    if(check(interaction_type, {}, _mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.modifiers       = _mods;
        info.delta           = _angle_delta;
        info.x               = _x;
        info.y               = _y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::buttonReleaseEvent(MouseButton _button, Modifier _mods, int _x, int _y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::BUTTONRELEASE;
    if(check(interaction_type, _button, _mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.button          = _button;
        info.modifiers       = _mods;
        info.x               = _x;
        info.y               = _y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::buttonPressEvent(MouseButton _button, Modifier _mods, int _x, int _y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::BUTTONPRESS;
    if(check(interaction_type, _button, _mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.button          = _button;
        info.modifiers       = _mods;
        info.x               = _x;
        info.y               = _y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::buttonDoublePressEvent(MouseButton _button, Modifier _mods, int _x, int _y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::BUTTONDOUBLEPRESS;
    if(check(interaction_type, _button, _mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.button          = _button;
        info.modifiers       = _mods;
        info.x               = _x;
        info.y               = _y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::keyPressEvent(int _key, Modifier _mods, int _mouse_x, int _mouse_y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::KEYPRESS;
    if(check(interaction_type, {}, _mods, _key))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.key             = _key;
        info.modifiers       = _mods;
        info.x               = _mouse_x;
        info.y               = _mouse_y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::keyReleaseEvent(int _key, Modifier _mods, int _mouse_x, int _mouse_y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::KEYRELEASE;
    if(check(interaction_type, {}, _mods, _key))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.key             = _key;
        info.modifiers       = _mods;
        info.x               = _mouse_x;
        info.y               = _mouse_y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::resizeEvent(int _width, int _height)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::RESIZE;
    if(check(interaction_type, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.x               = _width;
        info.y               = _height;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::pinchGestureEvent(double _scale_factor, int _center_x, int _center_y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::PINCH_GESTURE;
    if(check(interaction_type, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.delta           = _scale_factor;
        info.x               = _center_x;
        info.y               = _center_y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::panGestureMoveEvent(int _x, int _y, int _dx, int _dy)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::PAN_GESTURE_MOVE;
    if(check(interaction_type, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.x               = _x;
        info.y               = _y;
        info.dx              = _dx;
        info.dy              = _dy;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::panGestureReleaseEvent(int _x, int _y, int _dx, int _dy)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::PAN_GESTURE_RELEASE;
    if(check(interaction_type, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.x               = _x;
        info.y               = _y;
        info.dx              = _dx;
        info.dy              = _dy;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

void event::longTapGestureEvent(int _x, int _y)
{
    InteractionInfo::InteractionEnum interaction_type = InteractionInfo::InteractionEnum::LONG_TAP_GESTURE;
    if(check(interaction_type, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interaction_type;
        info.x               = _x;
        info.y               = _y;
        signal<TriggeredSignal>(TRIGGERED)->async_emit(info);
    }
}

//------------------------------------------------------------------------------

static InteractionInfo::InteractionEnum string_to_interaction_enum(const std::string& _s)
{
    if(_s == "mouseMove")
    {
        return InteractionInfo::InteractionEnum::MOUSEMOVE;
    }

    if(_s == "wheel")
    {
        return InteractionInfo::InteractionEnum::WHEELMOVE;
    }

    if(_s == "buttonRelease")
    {
        return InteractionInfo::InteractionEnum::BUTTONRELEASE;
    }

    if(_s == "buttonPress")
    {
        return InteractionInfo::InteractionEnum::BUTTONPRESS;
    }

    if(_s == "buttonDoublePress")
    {
        return InteractionInfo::InteractionEnum::BUTTONDOUBLEPRESS;
    }

    if(_s == "keyPress")
    {
        return InteractionInfo::InteractionEnum::KEYPRESS;
    }

    if(_s == "keyRelease")
    {
        return InteractionInfo::InteractionEnum::KEYRELEASE;
    }

    if(_s == "resize")
    {
        return InteractionInfo::InteractionEnum::RESIZE;
    }

    if(_s == "pinchGesture")
    {
        return InteractionInfo::InteractionEnum::PINCH_GESTURE;
    }

    if(_s == "panGestureMove")
    {
        return InteractionInfo::InteractionEnum::PAN_GESTURE_MOVE;
    }

    if(_s == "panGestureRelease")
    {
        return InteractionInfo::InteractionEnum::PAN_GESTURE_RELEASE;
    }

    SIGHT_THROW("Unknown interaction: " + _s);
}

//------------------------------------------------------------------------------

void event::configuring()
{
    configureParams();

    const config_t config = this->get_config();

    auto [begin, end] = config.equal_range("event");
    for(const auto& [_, event] : boost::make_iterator_range(begin, end))
    {
        Filter filter;
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
                    filter.buttons |= MouseButtons::LEFT;
                }
                else if(lower_button == "middle")
                {
                    filter.buttons |= MouseButtons::MIDDLE;
                }
                else if(lower_button == "right")
                {
                    filter.buttons |= MouseButtons::RIGHT;
                }
                else
                {
                    SIGHT_THROW("Invalid mouse button name: " + button);
                }
            }
        }

        if(boost::optional<std::string> modifiers = event.get_optional<std::string>("<xmlattr>.modifiers"))
        {
            filter.modifiers = Modifier::NONE;
            std::vector<std::string> modifier_list;
            boost::split(modifier_list, *modifiers, boost::is_any_of(" "));
            for(const std::string& modifier : modifier_list)
            {
                std::string lower_modifier = boost::to_lower_copy(modifier);
                if(lower_modifier == "shift")
                {
                    *filter.modifiers |= Modifier::SHIFT;
                }
                else if(lower_modifier == "ctrl")
                {
                    *filter.modifiers |= Modifier::CONTROL;
                }
                else if(lower_modifier == "alt")
                {
                    *filter.modifiers |= Modifier::ALT;
                }
                else if(lower_modifier == "meta")
                {
                    *filter.modifiers |= Modifier::META;
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
    this->getRenderService()->makeCurrent();

    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    this->getLayer()->addInteractor(interactor);
}

//------------------------------------------------------------------------------

void event::updating()
{
}

//------------------------------------------------------------------------------

void event::stopping()
{
    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    this->getLayer()->removeInteractor(interactor);
}

} // namespace sight::module::viz::scene3d::adaptor.
