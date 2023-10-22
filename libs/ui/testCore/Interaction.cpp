/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <core/spy_log.hpp>

#include <QAbstractButton>
#include <QTimer>

namespace sight::ui::test_core
{

//------------------------------------------------------------------------------

static std::string modifiers_to_string(Qt::KeyboardModifiers _modifiers)
{
    std::vector<std::string> strings;
    if((_modifiers& Qt::ShiftModifier) != 0U)
    {
        strings.emplace_back("Shift");
    }

    if((_modifiers& Qt::ControlModifier) != 0U)
    {
        strings.emplace_back("Control");
    }

    if((_modifiers& Qt::AltModifier) != 0U)
    {
        strings.emplace_back("Alt");
    }

    if((_modifiers& Qt::MetaModifier) != 0U)
    {
        strings.emplace_back("Meta");
    }

    if((_modifiers& Qt::KeypadModifier) != 0U)
    {
        strings.emplace_back("Keypad");
    }

    if((_modifiers& Qt::GroupSwitchModifier) != 0U)
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

static std::string point_to_string(const QPoint& _p)
{
    return std::string("(") + std::to_string(_p.x()) + ',' + std::to_string(_p.y()) + ')';
}

MouseClick::MouseClick(Qt::MouseButton _button, Qt::KeyboardModifiers _modifiers, const QPoint& _pos) :
    m_button(_button),
    m_modifiers(_modifiers),
    m_pos(_pos)
{
}

//------------------------------------------------------------------------------

void MouseClick::interactWith(QWidget* _widget) const
{
    interactWith<>(_widget);
}

//------------------------------------------------------------------------------

void MouseClick::interactWith(QWindow* _window) const
{
    interactWith<>(_window);
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
            if(auto* push_button = qobject_cast<QAbstractButton*>(thing);
               m_modifiers == Qt::NoModifier && push_button != nullptr)
            {
                push_button->click();
            }
            else
            {
                QTest::mouseClick(thing, m_button, m_modifiers, m_pos);
            }
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

    const std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

MouseDoubleClick::MouseDoubleClick(Qt::MouseButton _button, Qt::KeyboardModifiers _modifiers, const QPoint& _pos) :
    m_button(_button),
    m_modifiers(_modifiers),
    m_pos(_pos)
{
}

//------------------------------------------------------------------------------

void MouseDoubleClick::interactWith(QWidget* _widget) const
{
    interactWith<>(_widget);
}

//------------------------------------------------------------------------------

void MouseDoubleClick::interactWith(QWindow* _window) const
{
    interactWith<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void MouseDoubleClick::interactWith(T thing) const
{
    qApp->postEvent(
        qApp,
        new TestEvent(
            [*this, thing]
        {
            QTest::mouseDClick(thing, m_button, m_modifiers, m_pos);
        })
    );
}

//------------------------------------------------------------------------------

std::string MouseDoubleClick::toString() const
{
    std::string res;
    if(m_button == Qt::LeftButton)
    {
        res += "left double click";
    }
    else if(m_button == Qt::RightButton)
    {
        res += "right double click";
    }
    else if(m_button == Qt::MiddleButton)
    {
        res += "middle double click";
    }
    else
    {
        res += "other double click";
    }

    const std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

MouseDrag::MouseDrag(
    const QPoint& _from,
    const QPoint& _to,
    Qt::MouseButton _button,
    Qt::KeyboardModifiers _modifiers
) :
    m_from(_from),
    m_to(_to),
    m_button(_button),
    m_modifiers(_modifiers)
{
}

//------------------------------------------------------------------------------

void MouseDrag::interactWith(QWidget* _widget) const
{
    interactWith<>(_widget);
}

//------------------------------------------------------------------------------

void MouseDrag::interactWith(QWindow* _window) const
{
    interactWith<>(_window);
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
    res = res + "drag mouse from " + point_to_string(m_from) + " to " + point_to_string(m_to) + " while holding ";

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
    const std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " and " + modifiers;
    }

    return res;
}

MouseWheel::MouseWheel(const QPoint& _angle_delta, Qt::KeyboardModifiers _modifiers, const QPoint& _position) :
    m_angleDelta(_angle_delta),
    m_modifiers(_modifiers),
    m_position(_position)
{
}

//------------------------------------------------------------------------------

void MouseWheel::interactWith(QWidget* _widget) const
{
    interactWith<>(_widget);
}

//------------------------------------------------------------------------------

void MouseWheel::interactWith(QWindow* _window) const
{
    interactWith<>(_window);
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
    std::string res       = "rotate the mouse wheel of " + point_to_string(m_angleDelta) + " units";
    std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

KeyboardSequence::KeyboardSequence(std::string _text, Qt::KeyboardModifiers _modifiers) :
    m_text(std::move(_text)),
    m_modifiers(_modifiers)
{
}

//------------------------------------------------------------------------------

void KeyboardSequence::interactWith(QWidget* _widget) const
{
    interactWith<>(_widget);
}

//------------------------------------------------------------------------------

void KeyboardSequence::interactWith(QWindow* _window) const
{
    interactWith<>(_window);
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
    const std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

KeyboardClick::KeyboardClick(Qt::Key _key, Qt::KeyboardModifiers _modifiers) :
    m_key(_key),
    m_modifiers(_modifiers)
{
}

//------------------------------------------------------------------------------

void KeyboardClick::interactWith(QWidget* _widget) const
{
    interactWith<>(_widget);
}

//------------------------------------------------------------------------------

void KeyboardClick::interactWith(QWindow* _window) const
{
    interactWith<>(_window);
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
    std::string enum_name = QMetaEnum::fromType<Qt::Key>().valueToKey(m_key);
    enum_name = enum_name.substr(enum_name.find('_') + 1);
    std::string res             = "press " + enum_name + " key";
    const std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

PinchGesture::PinchGesture(std::pair<QPoint, QPoint> _first_finger_pos, std::pair<QPoint, QPoint> _second_finger_pos) :
    m_firstFingerPos(std::move(_first_finger_pos)),
    m_secondFingerPos(std::move(_second_finger_pos))
{
    SIGHT_ASSERT(
        "Two fingers can't be at the same place at the same time",
        m_firstFingerPos.first != m_secondFingerPos.first && m_firstFingerPos.second != m_secondFingerPos.second
    );
}

//------------------------------------------------------------------------------

void PinchGesture::interactWith(QWidget* _widget) const
{
    interactWith<>(_widget);
}

//------------------------------------------------------------------------------

void PinchGesture::interactWith(QWindow* _window) const
{
    interactWith<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void PinchGesture::interactWith(T thing) const
{
    static constexpr T null = nullptr; // Required to avoid ambiguous overload compiler error
    QTest::touchEvent(thing, Tester::getDummyTouchScreen())
    .press(0, m_firstFingerPos.first, null)
    .press(1, m_secondFingerPos.first, null);
    QTest::touchEvent(thing, Tester::getDummyTouchScreen())
    .move(0, m_firstFingerPos.first, null)
    .move(1, m_secondFingerPos.first, null);

    // If the two fingers are too far in one go, Qt will ignore it as it will consider them as spurious. We must
    // therefore divide the moves in multiple steps.
    QLineF first_finger_line(m_firstFingerPos.first, m_firstFingerPos.second);
    QLineF second_finger_line(m_secondFingerPos.first, m_secondFingerPos.second);
    for(int i = 0 ; i < 100 ; i++)
    {
        QTest::touchEvent(thing, Tester::getDummyTouchScreen())
        .move(0, first_finger_line.pointAt(i / 100.).toPoint(), null)
        .move(1, second_finger_line.pointAt(i / 100.).toPoint(), null);
    }

    QTest::touchEvent(thing, Tester::getDummyTouchScreen())
    .move(0, m_firstFingerPos.second, null)
    .move(1, m_secondFingerPos.second, null);
    QTest::touchEvent(thing, Tester::getDummyTouchScreen())
    .release(0, m_firstFingerPos.second, null)
    .release(1, m_secondFingerPos.second, null);
}

//------------------------------------------------------------------------------

std::string PinchGesture::toString() const
{
    return "pinch gesture with first finger going from " + point_to_string(m_firstFingerPos.first) + " to "
           + point_to_string(m_firstFingerPos.second) + " and second finger going from " + point_to_string(
        m_secondFingerPos.first
           ) + " to " + point_to_string(m_secondFingerPos.second);
}

} // namespace sight::ui::test_core
