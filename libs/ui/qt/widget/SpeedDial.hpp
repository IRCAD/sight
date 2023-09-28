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

#pragma once

#include "ui/qt/config.hpp"

#include <QAnimationGroup>
#include <QPointer>
#include <QPushButton>

#include <chrono>

namespace sight::ui::qt::widget
{

/// SpeedDial is a button which shows other buttons when clicked
class UI_QT_CLASS_API_QT SpeedDial : public QPushButton
{
Q_OBJECT
Q_PROPERTY(int spacing MEMBER m_spacing READ spacing WRITE setSpacing)
Q_PROPERTY(int animationDuration MEMBER m_animationDuration READ animationDuration WRITE setAnimationDuration)

public:

    enum class Direction
    {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    static inline constexpr int s_DEFAULT_SPACING            = 10;
    static inline constexpr int s_DEFAULT_ANIMATION_DURATION = 250;

    UI_QT_API_QT explicit SpeedDial(
        Direction direction   = Direction::DOWN,
        int spacing           = s_DEFAULT_SPACING,
        int animationDuration = s_DEFAULT_ANIMATION_DURATION,
        QWidget* parent       = nullptr
    );
    UI_QT_API_QT ~SpeedDial() override = default;

    UI_QT_API_QT void setDirection(Direction direction);
    [[nodiscard]] UI_QT_API_QT Direction direction() const;
    UI_QT_API_QT void setSpacing(int spacing);
    [[nodiscard]] UI_QT_API_QT int spacing() const;
    UI_QT_API_QT void setAnimationDuration(int duration);
    //------------------------------------------------------------------------------

    template<typename R, typename P>
    void setAnimationDuration(std::chrono::duration<R, P> animationDuration)
    {
        setAnimationDuration(std::chrono::milliseconds(animationDuration).count());
    }

    [[nodiscard]] UI_QT_API_QT int animationDuration() const;

    /// Make the button invisible
    UI_QT_API_QT void fold();
    /// Make the button visible
    UI_QT_API_QT void unfold();
    /// Make the button invisible if they are visible or visible if they are not
    UI_QT_API_QT void toggleFold();
    [[nodiscard]] UI_QT_API_QT bool isFolded() const;

    /// Provide a new list of actions to be managed by the speed dial.
    UI_QT_API_QT void updateActions(std::vector<QWidget*> actions);
    /// Get the list of the actions managed by the speed dial.
    [[nodiscard]] UI_QT_API_QT const std::vector<QWidget*>& actions() const;

    /// An event filter to listen to size changes of the actions and size changes and position changes of one of the
    /// ancestor of the speed dial. If either of these events happen, the positions of the actions must be updated.
    bool eventFilter(QObject* o, QEvent* e) override;

protected:

    /// Overridden to listen to the parent changes of the speed dial. If the parent change, the size changes and
    /// position
    /// changes of the parent and its ancestor must be listened, @see eventFilter.
    bool event(QEvent* e) override;
    /// Overridden to listen to the speed dial position changes. If the speed dial position changes, the positions of
    /// the actions must be updated.
    void moveEvent(QMoveEvent* e) override;
    /// Overridden to listen to the speed dial size changes. If the speed dial size changes, the positions of the
    /// actions must be updated.
    void resizeEvent(QResizeEvent* e) override;

private:

    /// A struct which contains the positions and sizes of the container and the positions of the actions during the
    /// animation. The first element in the pairs contains the initial value and the second element contains the final
    /// value.
    struct Animations
    {
        std::pair<QRect, QRect> containerRect;                    /// Rects of the container of actions
        std::vector<std::pair<QPoint, QPoint> > actionsPositions; /// Positions for each actions
    };

    /// Recursively "infect" the parent of the speed dial and its ancestors with the eventFilter. @see eventFilter.
    void infect(QObject* parent);
    /// Compute the new positions of the actions and then modify their associated animations
    void updateActionsPositions();
    /// Compute the new positions of the actions
    [[nodiscard]] Animations computePositions() const;

    Direction m_direction   = Direction::DOWN;
    int m_spacing           = s_DEFAULT_SPACING;
    int m_animationDuration = s_DEFAULT_ANIMATION_DURATION;
    bool m_folded           = true;
    std::vector<QWidget*> m_actions;
    QPointer<QWidget> m_actionsContainer = new QWidget;
    QAnimationGroup* m_animationGroup;
};

} // namespace sight::ui::qt::widget
