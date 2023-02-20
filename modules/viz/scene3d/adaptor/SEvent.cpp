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

#include "SEvent.hpp"

#include <core/com/SignalBase.hpp>
#include <core/com/Signals.hpp>

#include <viz/scene3d/IWindowInteractor.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::Signals::SignalKeyType SEvent::s_TRIGGERED = "triggered";

using InteractionInfo = sight::viz::scene3d::IWindowInteractor::InteractionInfo;

enum class MouseButtons : std::uint8_t
{
    NONE   = 0,
    LEFT   = 0 << 1,
    MIDDLE = 0 << 2,
    RIGHT  = 0 << 3
};

//------------------------------------------------------------------------------

std::uint8_t& operator|=(std::uint8_t& a, MouseButtons b)
{
    return a |= static_cast<std::uint8_t>(b);
}

//------------------------------------------------------------------------------

std::uint8_t operator&(std::uint8_t a, MouseButtons b)
{
    return a & static_cast<std::uint8_t>(b);
}

SEvent::SEvent()
{
    newSignal<TriggeredSignal>(s_TRIGGERED);
}

//------------------------------------------------------------------------------

bool SEvent::check(
    InteractionInfo::InteractionEnum type,
    std::optional<MouseButton> button,
    std::optional<Modifier> modifiers,
    std::optional<int> key
)
{
    bool ok = m_filters.empty();
    for(const Filter& filter : m_filters)
    {
        if(std::ranges::find(filter.type, type) == filter.type.end())
        {
            continue;
        }

        if(button)
        {
            MouseButtons mouseButton = MouseButtons::NONE;
            if(*button == LEFT)
            {
                mouseButton = MouseButtons::LEFT;
            }
            else if(*button == MIDDLE)
            {
                mouseButton = MouseButtons::MIDDLE;
            }
            else if(*button == RIGHT)
            {
                mouseButton = MouseButtons::RIGHT;
            }

            if(!(filter.buttons == 0 || ((filter.buttons & mouseButton) != 0U)))
            {
                continue;
            }
        }
        else if(filter.buttons != 0)
        {
            continue;
        }

        if(!((!filter.modifiers && !modifiers) || (filter.modifiers && modifiers && *filter.modifiers == *modifiers)))
        {
            continue;
        }

        if(!((filter.keys.empty() && !key) || (key && std::ranges::find(filter.keys, key) != filter.keys.end())))
        {
            continue;
        }

        ok = true;
    }

    return ok;
}

//------------------------------------------------------------------------------

void SEvent::mouseMoveEvent(MouseButton button, Modifier mods, int x, int y, int dx, int dy)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::MOUSEMOVE;
    if(check(interactionType, button, mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.button          = button;
        info.modifiers       = mods;
        info.x               = x;
        info.y               = y;
        info.dx              = dx;
        info.dy              = dy;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::wheelEvent(Modifier mods, double angleDelta, int x, int y)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::WHEELMOVE;
    if(check(interactionType, {}, mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.modifiers       = mods;
        info.delta           = angleDelta;
        info.x               = x;
        info.y               = y;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::buttonReleaseEvent(MouseButton button, Modifier mods, int x, int y)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::BUTTONRELEASE;
    if(check(interactionType, button, mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.button          = button;
        info.modifiers       = mods;
        info.x               = x;
        info.y               = y;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::buttonPressEvent(MouseButton button, Modifier mods, int x, int y)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::BUTTONPRESS;
    if(check(interactionType, button, mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.button          = button;
        info.modifiers       = mods;
        info.x               = x;
        info.y               = y;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::buttonDoublePressEvent(MouseButton button, Modifier mods, int x, int y)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::BUTTONDOUBLEPRESS;
    if(check(interactionType, button, mods, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.button          = button;
        info.modifiers       = mods;
        info.x               = x;
        info.y               = y;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::keyPressEvent(int key, Modifier mods, int mouseX, int mouseY)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::KEYPRESS;
    if(check(interactionType, {}, mods, key))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.key             = key;
        info.modifiers       = mods;
        info.x               = mouseX;
        info.y               = mouseY;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::keyReleaseEvent(int key, Modifier mods, int mouseX, int mouseY)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::KEYRELEASE;
    if(check(interactionType, {}, mods, key))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.key             = key;
        info.modifiers       = mods;
        info.x               = mouseX;
        info.y               = mouseY;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::resizeEvent(int width, int height)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::RESIZE;
    if(check(interactionType, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.x               = width;
        info.y               = height;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::pinchGestureEvent(double scaleFactor, int centerX, int centerY)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::PINCH_GESTURE;
    if(check(interactionType, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.delta           = scaleFactor;
        info.x               = centerX;
        info.y               = centerY;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::panGestureMoveEvent(int x, int y, int dx, int dy)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::PAN_GESTURE_MOVE;
    if(check(interactionType, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.x               = x;
        info.y               = y;
        info.dx              = dx;
        info.dy              = dy;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::panGestureReleaseEvent(int x, int y, int dx, int dy)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::PAN_GESTURE_RELEASE;
    if(check(interactionType, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.x               = x;
        info.y               = y;
        info.dx              = dx;
        info.dy              = dy;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::longTapGestureEvent(int x, int y)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::LONG_TAP_GESTURE;
    if(check(interactionType, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.x               = x;
        info.y               = y;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::pan2GestureMoveEvent(int x, int y, int dx, int dy)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::PAN2_GESTURE_MOVE;
    if(check(interactionType, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.x               = x;
        info.y               = y;
        info.dx              = dx;
        info.dy              = dy;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

void SEvent::pan2GestureReleaseEvent(int x, int y, int dx, int dy)
{
    InteractionInfo::InteractionEnum interactionType = InteractionInfo::InteractionEnum::PAN2_GESTURE_RELEASE;
    if(check(interactionType, {}, {}, {}))
    {
        InteractionInfo info;
        info.interactionType = interactionType;
        info.x               = x;
        info.y               = y;
        info.dx              = dx;
        info.dy              = dy;
        signal<TriggeredSignal>(s_TRIGGERED)->asyncEmit(info);
    }
}

//------------------------------------------------------------------------------

static InteractionInfo::InteractionEnum stringToInteractionEnum(const std::string& s)
{
    if(s == "mouseMove")
    {
        return InteractionInfo::InteractionEnum::MOUSEMOVE;
    }

    if(s == "wheel")
    {
        return InteractionInfo::InteractionEnum::WHEELMOVE;
    }

    if(s == "buttonRelease")
    {
        return InteractionInfo::InteractionEnum::BUTTONRELEASE;
    }

    if(s == "buttonPress")
    {
        return InteractionInfo::InteractionEnum::BUTTONPRESS;
    }

    if(s == "buttonDoublePress")
    {
        return InteractionInfo::InteractionEnum::BUTTONDOUBLEPRESS;
    }

    if(s == "keyPress")
    {
        return InteractionInfo::InteractionEnum::KEYPRESS;
    }

    if(s == "keyRelease")
    {
        return InteractionInfo::InteractionEnum::KEYRELEASE;
    }

    if(s == "resize")
    {
        return InteractionInfo::InteractionEnum::RESIZE;
    }

    if(s == "pinchGesture")
    {
        return InteractionInfo::InteractionEnum::PINCH_GESTURE;
    }

    if(s == "panGestureMove")
    {
        return InteractionInfo::InteractionEnum::PAN_GESTURE_MOVE;
    }

    if(s == "panGestureRelease")
    {
        return InteractionInfo::InteractionEnum::PAN_GESTURE_RELEASE;
    }

    if(s == "longTapGesture")
    {
        return InteractionInfo::InteractionEnum::LONG_TAP_GESTURE;
    }

    if(s == "pan2GestureMove")
    {
        return InteractionInfo::InteractionEnum::PAN2_GESTURE_MOVE;
    }

    if(s == "pan2GestureRelease")
    {
        return InteractionInfo::InteractionEnum::PAN2_GESTURE_RELEASE;
    }

    SIGHT_THROW("Unknown interaction: " + s);
}

//------------------------------------------------------------------------------

void SEvent::configuring()
{
    configureParams();

    const ConfigType config = getConfiguration();

    auto [begin, end] = config.equal_range("event");
    for(const auto& [_, event] : boost::make_iterator_range(begin, end))
    {
        Filter filter;
        std::vector<std::string> types;
        boost::split(types, event.get<std::string>("<xmlattr>.type"), boost::is_any_of(" "));
        std::ranges::transform(types, std::back_inserter(filter.type), stringToInteractionEnum);
        if(boost::optional<std::string> buttons = event.get_optional<std::string>("buttons"))
        {
            std::vector<std::string> buttonList;
            boost::split(buttonList, *buttons, boost::is_any_of(" "));
            for(const std::string& button : buttonList)
            {
                std::string lowerButton = boost::to_lower_copy(button);
                if(lowerButton == "left")
                {
                    filter.buttons |= MouseButtons::LEFT;
                }
                else if(lowerButton == "middle")
                {
                    filter.buttons |= MouseButtons::MIDDLE;
                }
                else if(lowerButton == "right")
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
            std::vector<std::string> modifierList;
            boost::split(modifierList, *modifiers, boost::is_any_of(" "));
            for(const std::string& modifier : modifierList)
            {
                std::string lowerModifier = boost::to_lower_copy(modifier);
                if(lowerModifier == "shift")
                {
                    *filter.modifiers |= Modifier::SHIFT;
                }
                else if(lowerModifier == "ctrl")
                {
                    *filter.modifiers |= Modifier::CONTROL;
                }
                else if(lowerModifier == "alt")
                {
                    *filter.modifiers |= Modifier::ALT;
                }
                else if(lowerModifier == "meta")
                {
                    *filter.modifiers |= Modifier::META;
                }
                else if(lowerModifier == "none")
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
            std::vector<std::string> keyList;
            boost::split(keyList, *keys, boost::is_any_of(" "));
            for(const std::string& key : keyList)
            {
                filter.keys.push_back(std::stoi(key, nullptr, 0));
            }
        }

        m_filters.push_back(filter);
    }
}

//------------------------------------------------------------------------------

void SEvent::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();

    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
    this->getLayer()->addInteractor(interactor);
}

//------------------------------------------------------------------------------

void SEvent::updating()
{
}

//------------------------------------------------------------------------------

void SEvent::stopping()
{
    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
    this->getLayer()->removeInteractor(interactor);
}

} // namespace sight::module::viz::scene3d::adaptor.
