/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <sight/ui/qt/config.hpp>

#include <QAnimationGroup>
#include <QPointer>
#include <QPushButton>

#include <chrono>

namespace sight::ui::qt::widget
{

/// speed_dial is a button which shows other buttons when clicked
class SIGHT_UI_QT_CLASS_API_QT speed_dial : public QPushButton
{
Q_OBJECT
Q_PROPERTY(int spacing MEMBER m_spacing READ spacing WRITE set_spacing)
Q_PROPERTY(int animation_duration MEMBER m_animation_duration READ animation_duration WRITE set_animation_duration)

public:

    enum class direction
    {
        up,
        right,
        down,
        left
    };

    static inline constexpr int DEFAULT_SPACING            = 10;
    static inline constexpr int DEFAULT_ANIMATION_DURATION = 250;

    SIGHT_UI_QT_API_QT explicit speed_dial(
        direction _direction    = direction::down,
        int _spacing            = DEFAULT_SPACING,
        int _animation_duration = DEFAULT_ANIMATION_DURATION,
        QWidget* _parent        = nullptr
    );
    SIGHT_UI_QT_API_QT ~speed_dial() override = default;

    SIGHT_UI_QT_API_QT void set_direction(direction _direction);
    [[nodiscard]] SIGHT_UI_QT_API_QT direction direction() const;
    SIGHT_UI_QT_API_QT void set_spacing(int _spacing);
    [[nodiscard]] SIGHT_UI_QT_API_QT int spacing() const;
    SIGHT_UI_QT_API_QT void set_animation_duration(int _duration);
    //------------------------------------------------------------------------------

    template<typename R, typename P>
    void set_animation_duration(std::chrono::duration<R, P> _animation_duration)
    {
        set_animation_duration(std::chrono::milliseconds(_animation_duration).count());
    }

    [[nodiscard]] SIGHT_UI_QT_API_QT int animation_duration() const;

    /// Make the button invisible
    SIGHT_UI_QT_API_QT void fold();
    /// Make the button visible
    SIGHT_UI_QT_API_QT void unfold();
    /// Make the button invisible if they are visible or visible if they are not
    SIGHT_UI_QT_API_QT void toggle_fold();
    [[nodiscard]] SIGHT_UI_QT_API_QT bool is_folded() const;

    /// Provide a new list of actions to be managed by the speed dial.
    SIGHT_UI_QT_API_QT void update_actions(std::vector<QWidget*> _actions);
    /// Get the list of the actions managed by the speed dial.
    [[nodiscard]] SIGHT_UI_QT_API_QT const std::vector<QWidget*>& actions() const;

    /// An event filter to listen to size changes of the actions and size changes and position changes of one of the
    /// ancestor of the speed dial. If either of these events happen, the positions of the actions must be updated.
    bool eventFilter(QObject* _o, QEvent* _e) override;

protected:

    /// Overridden to listen to the parent changes of the speed dial. If the parent change, the size changes and
    /// position
    /// changes of the parent and its ancestor must be listened, @see eventFilter.
    bool event(QEvent* _e) override;
    /// Overridden to listen to the speed dial position changes. If the speed dial position changes, the positions of
    /// the actions must be updated.
    void moveEvent(QMoveEvent* _e) override;
    /// Overridden to listen to the speed dial size changes. If the speed dial size changes, the positions of the
    /// actions must be updated.
    void resizeEvent(QResizeEvent* _e) override;

private:

    /// A struct which contains the positions and sizes of the container and the positions of the actions during the
    /// animation. The first element in the pairs contains the initial value and the second element contains the final
    /// value.
    struct animations
    {
        std::pair<QRect, QRect> container_rect;                    /// Rects of the container of actions
        std::vector<std::pair<QPoint, QPoint> > actions_positions; /// Positions for each actions
    };

    /// Recursively "infect" the parent of the speed dial and its ancestors with the eventFilter. @see eventFilter.
    void infect(QObject* _parent);
    /// Compute the new positions of the actions and then modify their associated animations
    void update_actions_positions();
    /// Compute the new positions of the actions
    [[nodiscard]] animations compute_positions() const;

    enum direction m_direction = direction::down;
    int m_spacing              = DEFAULT_SPACING;
    int m_animation_duration   = DEFAULT_ANIMATION_DURATION;
    bool m_folded              = true;
    std::vector<QWidget*> m_actions;
    QPointer<QWidget> m_actions_container = new QWidget;
    QAnimationGroup* m_animation_group;
};

} // namespace sight::ui::qt::widget
