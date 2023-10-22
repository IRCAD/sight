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

#pragma once

#include "ui/testCore/config.hpp"

#include <QPoint>
#include <QWidget>
#include <QWindow>

#include <string>

namespace sight::ui::test_core
{

/// Represents a user interaction on the interace, such as a mouse click, keyboard tap...
class GUITEST_CLASS_API Interaction
{
public:

    /// Destructor. Does nothing.
    virtual ~Interaction() = default;

    /// Use this interaction to interact with a graphic component.
    /// @{
    virtual void interactWith(QWidget* _widget) const = 0;
    virtual void interactWith(QWindow* _window) const = 0;
    /// @}

    /// Returns a string representation of the interaction.
    [[nodiscard]] virtual std::string toString() const = 0;
};

/// Represents a mouse click.
class GUITEST_CLASS_API MouseClick : public Interaction
{
public:

    GUITEST_API MouseClick(
        Qt::MouseButton _button          = Qt::MouseButton::LeftButton,
        Qt::KeyboardModifiers _modifiers = Qt::NoModifier,
        const QPoint& _pos               = QPoint()
    );

    void interactWith(QWidget* _widget) const override;
    void interactWith(QWindow* _window) const override;

    [[nodiscard]] std::string toString() const override;

private:

    template<typename T>
    void interactWith(T _thing) const;

    Qt::MouseButton m_button;
    Qt::KeyboardModifiers m_modifiers;
    QPoint m_pos;
};

/// Represents a mouse double click.
class GUITEST_CLASS_API MouseDoubleClick : public Interaction
{
public:

    GUITEST_API MouseDoubleClick(
        Qt::MouseButton _button          = Qt::MouseButton::LeftButton,
        Qt::KeyboardModifiers _modifiers = Qt::NoModifier,
        const QPoint& _pos               = QPoint()
    );

    void interactWith(QWidget* _widget) const override;
    void interactWith(QWindow* _window) const override;

    [[nodiscard]] std::string toString() const override;

private:

    template<typename T>
    void interactWith(T _thing) const;

    Qt::MouseButton m_button;
    Qt::KeyboardModifiers m_modifiers;
    QPoint m_pos;
};

/// Represents a dragging of the mouse over a component.
class GUITEST_CLASS_API MouseDrag : public Interaction
{
public:

    GUITEST_API MouseDrag(
        const QPoint& _from,
        const QPoint& _to,
        Qt::MouseButton _button          = Qt::MouseButton::LeftButton,
        Qt::KeyboardModifiers _modifiers = Qt::NoModifier
    );

    void interactWith(QWidget* _widget) const override;
    void interactWith(QWindow* _window) const override;

    [[nodiscard]] std::string toString() const override;

private:

    template<typename T>
    void interactWith(T _thing) const;

    QPoint m_from;
    QPoint m_to;
    Qt::MouseButton m_button;
    Qt::KeyboardModifiers m_modifiers;
};

/// Represents the use of the mouse wheel over a component.
class GUITEST_CLASS_API MouseWheel : public Interaction
{
public:

    /**
     * @param angleDelta relative amount that the wheel is to be rotated, in eighths of a degree. A positive value means
     * that the wheel is rotated forwards away from the user.
     * The x part of the QPoint represents a horizontal wheel move, while the y part is the classical top vertical wheel
     * move.
     * Most mouse types work in steps of 15 degrees, in which case the delta value is a multiple of 120;
     *  i.e., 120 units * 1/8 = 15 degrees.
     * For a classical test this value can be set to QPoint(0, 1200) for a 150° turn of the top wheel.
     * see @ref https://doc.qt.io/qt-5/qwheelevent.html#angleDelta for more information
     * @param modifiers the keyboard modifier to be held when the mouse wheel is used
     * @param position the position of the widget where the mouse wheel is to be rotated
     */
    GUITEST_API MouseWheel(
        const QPoint& _angle_delta,
        Qt::KeyboardModifiers _modifiers = Qt::NoModifier,
        const QPoint& _position          = QPoint()
    );

    void interactWith(QWidget* _widget) const override;
    void interactWith(QWindow* _window) const override;

    [[nodiscard]] std::string toString() const override;

private:

    template<typename T>
    void interactWith(T _thing) const;

    QPoint m_angleDelta;
    Qt::KeyboardModifiers m_modifiers;
    QPoint m_position;
};

/// Represents the typing of a sequence of printable caracters.
class GUITEST_CLASS_API KeyboardSequence : public Interaction
{
public:

    GUITEST_API KeyboardSequence(std::string _text, Qt::KeyboardModifiers _modifiers = Qt::NoModifier);

    void interactWith(QWidget* _widget) const override;
    void interactWith(QWindow* _window) const override;
    [[nodiscard]] std::string toString() const override;

private:

    template<typename T>
    void interactWith(T _thing) const;

    std::string m_text;
    Qt::KeyboardModifiers m_modifiers;
};

/// Represents a key tap.
class GUITEST_CLASS_API KeyboardClick : public Interaction
{
public:

    GUITEST_API KeyboardClick(Qt::Key _key, Qt::KeyboardModifiers _modifiers = Qt::NoModifier);

    void interactWith(QWidget* _widget) const override;
    void interactWith(QWindow* _window) const override;
    [[nodiscard]] std::string toString() const override;

private:

    template<typename T>
    void interactWith(T _thing) const;

    Qt::Key m_key;
    Qt::KeyboardModifiers m_modifiers;
};

/// Represents a pinch gesture with two fingers on a component.
class GUITEST_CLASS_API PinchGesture : public Interaction
{
public:

    /**
     * @param firstFingerPos the starting position and ending position of the first finger
     * @param secondFingerPos the starting position and ending position of the second finger
     */
    GUITEST_API PinchGesture(std::pair<QPoint, QPoint> _first_finger_pos, std::pair<QPoint, QPoint> _second_finger_pos);

    void interactWith(QWidget* _widget) const override;
    void interactWith(QWindow* _window) const override;
    [[nodiscard]] std::string toString() const override;

private:

    template<typename T>
    void interactWith(T _thing) const;

    std::pair<QPoint, QPoint> m_firstFingerPos;
    std::pair<QPoint, QPoint> m_secondFingerPos;
};

} // namespace sight::ui::testCore
