/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "Interaction.hpp"

#include "Tester.hpp"

namespace sight::ui::test
{

//------------------------------------------------------------------------------

static std::string modifiersToString(Qt::KeyboardModifiers modifiers)
{
    std::vector<std::string> strings;
    if((modifiers& Qt::ShiftModifier) != 0U)
    {
        strings.emplace_back("Shift");
    }

    if((modifiers& Qt::ControlModifier) != 0U)
    {
        strings.emplace_back("Control");
    }

    if((modifiers& Qt::AltModifier) != 0U)
    {
        strings.emplace_back("Alt");
    }

    if((modifiers& Qt::MetaModifier) != 0U)
    {
        strings.emplace_back("Meta");
    }

    if((modifiers& Qt::KeypadModifier) != 0U)
    {
        strings.emplace_back("Keypad");
    }

    if((modifiers& Qt::GroupSwitchModifier) != 0U)
    {
        strings.emplace_back("GroupSwitch");
    }

    std::string res;
    if(!strings.empty())
    {
        for(size_t i = 0 ; i < strings.size() ; i++)
        {
            if(i != 0)
            {
                if(i == strings.size() - 1)
                {
                    res += " and ";
                }
                else
                {
                    res += ", ";
                }
            }
        }

        res = res + " key" + (strings.size() > 1 ? "s" : "");
    }

    return res;
}

//------------------------------------------------------------------------------

static std::string pointToString(const QPoint& p)
{
    return std::string("(") + std::to_string(p.x()) + ',' + std::to_string(p.y()) + ')';
}

MouseClick::MouseClick(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, const QPoint& pos) :
    m_button(button),
    m_modifiers(modifiers),
    m_pos(pos)
{
}

//------------------------------------------------------------------------------

void MouseClick::interactWith(QWidget* widget) const
{
    interactWith<>(widget);
}

//------------------------------------------------------------------------------

void MouseClick::interactWith(QWindow* window) const
{
    interactWith<>(window);
}

//------------------------------------------------------------------------------

template<typename T>
void MouseClick::interactWith(T thing) const
{
    qApp->postEvent(
        qApp,
        new TestEvent(
            [*this, thing]
        {
            QTest::mouseClick(thing, m_button, m_modifiers, m_pos);
        })
    );
}

//------------------------------------------------------------------------------

std::string MouseClick::toString() const
{
    std::string res;
    if(m_button == Qt::LeftButton)
    {
        res += "left click";
    }
    else if(m_button == Qt::RightButton)
    {
        res += "right click";
    }
    else if(m_button == Qt::MiddleButton)
    {
        res += "middle click";
    }
    else
    {
        res += "other click";
    }

    const std::string modifiers = modifiersToString(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

MouseDrag::MouseDrag(const QPoint& from, const QPoint& to, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) :
    m_from(from),
    m_to(to),
    m_button(button),
    m_modifiers(modifiers)
{
}

//------------------------------------------------------------------------------

void MouseDrag::interactWith(QWidget* widget) const
{
    interactWith<>(widget);
}

//------------------------------------------------------------------------------

void MouseDrag::interactWith(QWindow* window) const
{
    interactWith<>(window);
}

//------------------------------------------------------------------------------

template<typename T>
void MouseDrag::interactWith(T thing) const
{
    qApp->postEvent(
        qApp,
        new TestEvent(
            [*this, thing]
        {
            Tester::mouseMove(thing, m_from);
            QTest::mousePress(thing, m_button, m_modifiers, m_from);
            Tester::mouseMove(thing, m_to, -1, m_button, m_modifiers);
            QTest::mouseRelease(thing, m_button, m_modifiers, m_to);
        })
    );
}

//------------------------------------------------------------------------------

std::string MouseDrag::toString() const
{
    std::string res;
    res = res + "drag mouse from " + pointToString(m_from) + " to " + pointToString(m_to) + " while holding ";

    if(m_button == Qt::LeftButton)
    {
        res += "left";
    }
    else if(m_button == Qt::RightButton)
    {
        res += "right";
    }
    else if(m_button == Qt::MiddleButton)
    {
        res += "middle";
    }
    else
    {
        res += "other";
    }

    res += " button";
    const std::string modifiers = modifiersToString(m_modifiers);
    if(!modifiers.empty())
    {
        res += " and " + modifiers;
    }

    return res;
}

MouseWheel::MouseWheel(const QPoint& _angleDelta, Qt::KeyboardModifiers _modifiers, const QPoint& _position) :
    m_angleDelta(_angleDelta),
    m_modifiers(_modifiers),
    m_position(_position)
{
}

//------------------------------------------------------------------------------

void MouseWheel::interactWith(QWidget* widget) const
{
    interactWith<>(widget);
}

//------------------------------------------------------------------------------

void MouseWheel::interactWith(QWindow* window) const
{
    interactWith<>(window);
}

//------------------------------------------------------------------------------

template<typename T>
void MouseWheel::interactWith(T thing) const
{
    QPoint position = m_position;
    if(position.isNull())
    {
        position = QRect(0, 0, thing->width(), thing->height()).center();
    }

    auto* event = new QWheelEvent(
        position,
        thing->mapToGlobal(position),
        QPoint(),
        m_angleDelta,
        Qt::NoButton,
        m_modifiers,
        Qt::ScrollUpdate,
        false
    );
    qApp->postEvent(thing, event);
}

//------------------------------------------------------------------------------

std::string MouseWheel::toString() const
{
    std::string res       = "rotate the mouse wheel of " + pointToString(m_angleDelta) + " units";
    std::string modifiers = modifiersToString(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

KeyboardSequence::KeyboardSequence(std::string text, Qt::KeyboardModifiers modifiers) :
    m_text(std::move(text)),
    m_modifiers(modifiers)
{
}

//------------------------------------------------------------------------------

void KeyboardSequence::interactWith(QWidget* widget) const
{
    interactWith<>(widget);
}

//------------------------------------------------------------------------------

void KeyboardSequence::interactWith(QWindow* window) const
{
    interactWith<>(window);
}

//------------------------------------------------------------------------------

template<typename T>
void KeyboardSequence::interactWith(T thing) const
{
    qApp->postEvent(
        qApp,
        new TestEvent(
            [*this, thing]
        {
            for(char c : m_text)
            {
                QTest::keyClick(thing, c, m_modifiers);
            }
        })
    );
}

//------------------------------------------------------------------------------

std::string KeyboardSequence::toString() const
{
    std::string res             = "type \"" + m_text + '"';
    const std::string modifiers = modifiersToString(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

KeyboardClick::KeyboardClick(Qt::Key key, Qt::KeyboardModifiers modifiers) :
    m_key(key),
    m_modifiers(modifiers)
{
}

//------------------------------------------------------------------------------

void KeyboardClick::interactWith(QWidget* widget) const
{
    interactWith<>(widget);
}

//------------------------------------------------------------------------------

void KeyboardClick::interactWith(QWindow* window) const
{
    interactWith<>(window);
}

//------------------------------------------------------------------------------

template<typename T>
void KeyboardClick::interactWith(T thing) const
{
    qApp->postEvent(
        qApp,
        new TestEvent(
            [*this, thing]
        {
            QTest::keyClick(thing, m_key, m_modifiers);
        })
    );
}

//------------------------------------------------------------------------------

std::string KeyboardClick::toString() const
{
    std::string enumName = QMetaEnum::fromType<Qt::Key>().valueToKey(m_key);
    enumName = enumName.substr(enumName.find('_') + 1);
    std::string res             = "press " + enumName + " key";
    const std::string modifiers = modifiersToString(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

} // namespace sight::ui::test
