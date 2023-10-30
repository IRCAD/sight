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

#include "interaction.hpp"

#include "tester.hpp"

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

mouse_click::mouse_click(Qt::MouseButton _button, Qt::KeyboardModifiers _modifiers, const QPoint& _pos) :
    m_button(_button),
    m_modifiers(_modifiers),
    m_pos(_pos)
{
}

//------------------------------------------------------------------------------

void mouse_click::interact_with(QWidget* _widget) const
{
    interact_with<>(_widget);
}

//------------------------------------------------------------------------------

void mouse_click::interact_with(QWindow* _window) const
{
    interact_with<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void mouse_click::interact_with(T _thing) const
{
    qApp->postEvent(
        qApp,
        new test_event(
            [*this, _thing]
        {
            if(auto* push_button = qobject_cast<QAbstractButton*>(_thing);
               m_modifiers == Qt::NoModifier && push_button != nullptr)
            {
                push_button->click();
            }
            else
            {
                QTest::mouseClick(_thing, m_button, m_modifiers, m_pos);
            }
        })
    );
}

//------------------------------------------------------------------------------

std::string mouse_click::to_string() const
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

mouse_double_click::mouse_double_click(Qt::MouseButton _button, Qt::KeyboardModifiers _modifiers, const QPoint& _pos) :
    m_button(_button),
    m_modifiers(_modifiers),
    m_pos(_pos)
{
}

//------------------------------------------------------------------------------

void mouse_double_click::interact_with(QWidget* _widget) const
{
    interact_with<>(_widget);
}

//------------------------------------------------------------------------------

void mouse_double_click::interact_with(QWindow* _window) const
{
    interact_with<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void mouse_double_click::interact_with(T _thing) const
{
    qApp->postEvent(
        qApp,
        new test_event(
            [*this, _thing]
        {
            QTest::mouseDClick(_thing, m_button, m_modifiers, m_pos);
        })
    );
}

//------------------------------------------------------------------------------

std::string mouse_double_click::to_string() const
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

mouse_drag::mouse_drag(
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

void mouse_drag::interact_with(QWidget* _widget) const
{
    interact_with<>(_widget);
}

//------------------------------------------------------------------------------

void mouse_drag::interact_with(QWindow* _window) const
{
    interact_with<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void mouse_drag::interact_with(T _thing) const
{
    qApp->postEvent(
        qApp,
        new test_event(
            [*this, _thing]
        {
            tester::mouse_move(_thing, m_from);
            QTest::mousePress(_thing, m_button, m_modifiers, m_from);
            tester::mouse_move(_thing, m_to, -1, m_button, m_modifiers);
            QTest::mouseRelease(_thing, m_button, m_modifiers, m_to);
        })
    );
}

//------------------------------------------------------------------------------

std::string mouse_drag::to_string() const
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

mouse_wheel::mouse_wheel(const QPoint& _angle_delta, Qt::KeyboardModifiers _modifiers, const QPoint& _position) :
    m_angle_delta(_angle_delta),
    m_modifiers(_modifiers),
    m_position(_position)
{
}

//------------------------------------------------------------------------------

void mouse_wheel::interact_with(QWidget* _widget) const
{
    interact_with<>(_widget);
}

//------------------------------------------------------------------------------

void mouse_wheel::interact_with(QWindow* _window) const
{
    interact_with<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void mouse_wheel::interact_with(T _thing) const
{
    QPoint position = m_position;
    if(position.isNull())
    {
        position = QRect(0, 0, _thing->width(), _thing->height()).center();
    }

    auto* event = new QWheelEvent(
        position,
        _thing->mapToGlobal(position),
        QPoint(),
        m_angle_delta,
        Qt::NoButton,
        m_modifiers,
        Qt::ScrollUpdate,
        false
    );
    qApp->postEvent(_thing, event);
}

//------------------------------------------------------------------------------

std::string mouse_wheel::to_string() const
{
    std::string res       = "rotate the mouse wheel of " + point_to_string(m_angle_delta) + " units";
    std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

keyboard_sequence::keyboard_sequence(std::string _text, Qt::KeyboardModifiers _modifiers) :
    m_text(std::move(_text)),
    m_modifiers(_modifiers)
{
}

//------------------------------------------------------------------------------

void keyboard_sequence::interact_with(QWidget* _widget) const
{
    interact_with<>(_widget);
}

//------------------------------------------------------------------------------

void keyboard_sequence::interact_with(QWindow* _window) const
{
    interact_with<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void keyboard_sequence::interact_with(T _thing) const
{
    qApp->postEvent(
        qApp,
        new test_event(
            [*this, _thing]
        {
            for(char c : m_text)
            {
                QTest::keyClick(_thing, c, m_modifiers);
            }
        })
    );
}

//------------------------------------------------------------------------------

std::string keyboard_sequence::to_string() const
{
    std::string res             = "type \"" + m_text + '"';
    const std::string modifiers = modifiers_to_string(m_modifiers);
    if(!modifiers.empty())
    {
        res += " while holding " + modifiers;
    }

    return res;
}

keyboard_click::keyboard_click(Qt::Key _key, Qt::KeyboardModifiers _modifiers) :
    m_key(_key),
    m_modifiers(_modifiers)
{
}

//------------------------------------------------------------------------------

void keyboard_click::interact_with(QWidget* _widget) const
{
    interact_with<>(_widget);
}

//------------------------------------------------------------------------------

void keyboard_click::interact_with(QWindow* _window) const
{
    interact_with<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void keyboard_click::interact_with(T _thing) const
{
    qApp->postEvent(
        qApp,
        new test_event(
            [*this, _thing]
        {
            QTest::keyClick(_thing, m_key, m_modifiers);
        })
    );
}

//------------------------------------------------------------------------------

std::string keyboard_click::to_string() const
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

pinch_gesture::pinch_gesture(
    std::pair<QPoint, QPoint> _first_finger_pos,
    std::pair<QPoint, QPoint> _second_finger_pos
) :
    m_first_finger_pos(std::move(_first_finger_pos)),
    m_second_finger_pos(std::move(_second_finger_pos))
{
    SIGHT_ASSERT(
        "Two fingers can't be at the same place at the same time",
        m_first_finger_pos.first != m_second_finger_pos.first && m_first_finger_pos.second != m_second_finger_pos.second
    );
}

//------------------------------------------------------------------------------

void pinch_gesture::interact_with(QWidget* _widget) const
{
    interact_with<>(_widget);
}

//------------------------------------------------------------------------------

void pinch_gesture::interact_with(QWindow* _window) const
{
    interact_with<>(_window);
}

//------------------------------------------------------------------------------

template<typename T>
void pinch_gesture::interact_with(T _thing) const
{
    static constexpr T s_NULL = nullptr; // Required to avoid ambiguous overload compiler error
    QTest::touchEvent(_thing, tester::get_dummy_touch_screen())
    .press(0, m_first_finger_pos.first, s_NULL)
    .press(1, m_second_finger_pos.first, s_NULL);
    QTest::touchEvent(_thing, tester::get_dummy_touch_screen())
    .move(0, m_first_finger_pos.first, s_NULL)
    .move(1, m_second_finger_pos.first, s_NULL);

    // If the two fingers are too far in one go, Qt will ignore it as it will consider them as spurious. We must
    // therefore divide the moves in multiple steps.
    QLineF first_finger_line(m_first_finger_pos.first, m_first_finger_pos.second);
    QLineF second_finger_line(m_second_finger_pos.first, m_second_finger_pos.second);
    for(int i = 0 ; i < 100 ; i++)
    {
        QTest::touchEvent(_thing, tester::get_dummy_touch_screen())
        .move(0, first_finger_line.pointAt(i / 100.).toPoint(), s_NULL)
        .move(1, second_finger_line.pointAt(i / 100.).toPoint(), s_NULL);
    }

    QTest::touchEvent(_thing, tester::get_dummy_touch_screen())
    .move(0, m_first_finger_pos.second, s_NULL)
    .move(1, m_second_finger_pos.second, s_NULL);
    QTest::touchEvent(_thing, tester::get_dummy_touch_screen())
    .release(0, m_first_finger_pos.second, s_NULL)
    .release(1, m_second_finger_pos.second, s_NULL);
}

//------------------------------------------------------------------------------

std::string pinch_gesture::to_string() const
{
    return "pinch gesture with first finger going from " + point_to_string(m_first_finger_pos.first) + " to "
           + point_to_string(m_first_finger_pos.second) + " and second finger going from " + point_to_string(
        m_second_finger_pos.first
           ) + " to " + point_to_string(m_second_finger_pos.second);
}

} // namespace sight::ui::test_core
