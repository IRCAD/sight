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

#include "speed_dial.hpp"

#include <core/spy_log.hpp>

#include <QEvent>
#include <QLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

namespace sight::ui::qt::widget
{

speed_dial::speed_dial(Direction _direction, int _spacing, int _animation_duration, QWidget* _parent) :
    QPushButton(_parent),
    m_direction(_direction),
    m_spacing(_spacing),
    m_animationDuration(_animation_duration),
    m_animationGroup(new QParallelAnimationGroup)
{
    setEnabled(false);
    setCheckable(true);
    m_actionsContainer->setObjectName(objectName() + "Actions");
    infect(_parent);
    QObject::connect(this, &QPushButton::clicked, this, &speed_dial::toggleFold);
    QObject::connect(
        m_animationGroup,
        &QAnimationGroup::finished,
        [this]
        {
            if(m_folded)
            {
                m_actionsContainer->hide();
            }
        });
    QObject::connect(
        this,
        &QObject::objectNameChanged,
        [this](QString)
        {
            m_actionsContainer->setObjectName(objectName() + "Actions");
        });
}

//------------------------------------------------------------------------------

void speed_dial::setDirection(Direction _direction)
{
    m_direction = _direction;
}

//------------------------------------------------------------------------------

speed_dial::Direction speed_dial::direction() const
{
    return m_direction;
}

//------------------------------------------------------------------------------

void speed_dial::setSpacing(int _spacing)
{
    m_spacing = _spacing;
}

//------------------------------------------------------------------------------

int speed_dial::spacing() const
{
    return m_spacing;
}

//------------------------------------------------------------------------------

void speed_dial::setAnimationDuration(int _animation_duration)
{
    // It seems there is a bug when animationDuration=1; QAbstractAnimation::finished doesn't trigger, and as such the
    // buttons don't hide when folded.
    if(_animation_duration == 1)
    {
        m_animationDuration = 0;
    }
    else
    {
        m_animationDuration = _animation_duration;
    }
}

//------------------------------------------------------------------------------

int speed_dial::animationDuration() const
{
    return m_animationDuration;
}

//------------------------------------------------------------------------------

void speed_dial::fold()
{
    if(m_folded)
    {
        return;
    }

    m_folded = true;
    setChecked(false);
    if(m_actionsContainer == nullptr)
    {
        return;
    }

    m_animationGroup->setDirection(QAbstractAnimation::Backward);
    m_animationGroup->start();
}

//------------------------------------------------------------------------------

void speed_dial::unfold()
{
    if(!m_folded)
    {
        return;
    }

    if(m_actions.empty())
    {
        return;
    }

    m_folded = false;
    setChecked(true);
    m_animationGroup->setDirection(QAbstractAnimation::Forward);
    m_actionsContainer->show();
    m_animationGroup->start();
}

//------------------------------------------------------------------------------

void speed_dial::toggleFold()
{
    if(m_folded)
    {
        unfold();
    }
    else
    {
        fold();
    }
}

//------------------------------------------------------------------------------

bool speed_dial::isFolded() const
{
    return m_folded;
}

//------------------------------------------------------------------------------

void speed_dial::updateActions(std::vector<QWidget*> _actions)
{
    m_actions = std::move(_actions);
    setEnabled(!m_actions.empty());

    // Update the action container
    for(QObject* obj : m_actionsContainer->children())
    {
        if(obj->isWidgetType() && std::ranges::find(m_actions, obj) == m_actions.end())
        {
            static_cast<QWidget*>(obj)->hide();
            obj->setParent(nullptr);
        }
    }

    for(QWidget* w : m_actions)
    {
        w->setParent(m_actionsContainer);
        w->show();
    }

    // Update the animations
    int current_time = m_animationGroup->currentTime(); // Current time get resetted when calling QAnimationGroup::clear
    m_animationGroup->clear();
    auto* container_animation = new QPropertyAnimation(m_actionsContainer, "geometry");
    container_animation->setDuration(m_animationDuration);
    container_animation->setCurrentTime(current_time);
    m_animationGroup->addAnimation(container_animation);
    for(QWidget* w : m_actions)
    {
        auto* animation = new QPropertyAnimation(w, "pos");
        animation->setDuration(m_animationDuration);
        animation->setCurrentTime(current_time);
        m_animationGroup->addAnimation(animation);
    }

    // Ensure the actual size of the widgets are computed; this is needed by computePositions
    std::ranges::for_each(m_actions, &QWidget::adjustSize);
    if(parentWidget() != nullptr)
    {
        updateActionsPositions();
    }
}

//------------------------------------------------------------------------------

const std::vector<QWidget*>& speed_dial::actions() const
{
    return m_actions;
}

//------------------------------------------------------------------------------

bool speed_dial::eventFilter(QObject* _o, QEvent* _e)
{
    QWidget* w = qobject_cast<QWidget*>(_o);
    if(w == nullptr)
    {
        return false;
    }

    // The positions of the actions must be updated if:
    // - The size of one of the actions changes
    // - One of the ancestors of the speed dial is resized of moves
    if((std::ranges::find(m_actions, w) != m_actions.end() && _e->type() == QEvent::Resize)
       || (_o->findChildren<speed_dial*>(objectName()).contains(this)
           && (_e->type() == QEvent::Resize || _e->type() == QEvent::Move)))
    {
        updateActionsPositions();
    }

    return false;
}

//------------------------------------------------------------------------------

bool speed_dial::event(QEvent* _e)
{
    if(_e->type() == QEvent::ParentChange)
    {
        infect(parent());
        m_actionsContainer->setParent(window());
    }

    return QPushButton::event(_e);
}

//------------------------------------------------------------------------------

void speed_dial::moveEvent(QMoveEvent* _e)
{
    QPushButton::moveEvent(_e);
    updateActionsPositions();
}

//------------------------------------------------------------------------------

void speed_dial::resizeEvent(QResizeEvent* _e)
{
    QPushButton::resizeEvent(_e);
    updateActionsPositions();
    bool is_in_an_overlay = false;
    for(QWidget* child = this ; !is_in_an_overlay && child->parentWidget() != nullptr ; child = child->parentWidget())
    {
        QLayout* parent_layout = child->parentWidget()->layout();
        is_in_an_overlay = (parent_layout == nullptr || parent_layout->indexOf(child) == -1);
    }

    if(is_in_an_overlay)
    {
        setIconSize(size());
    }
}

//------------------------------------------------------------------------------

void speed_dial::infect(QObject* _parent)
{
    if(_parent == nullptr)
    {
        return;
    }

    _parent->installEventFilter(this);
    infect(_parent->parent());
}

//------------------------------------------------------------------------------

void speed_dial::updateActionsPositions()
{
    if(m_actionsContainer == nullptr || m_animationGroup == nullptr)
    {
        return;
    }

    // Either one of the actions was resized or one of the ancestor was resized or moved. Either way, the position and
    // size of the container is now invalid and must be changed.
    SIGHT_ASSERT(
        "There must be as much animations as there are actions (+ 1 animation because of the container)",
        static_cast<std::size_t>(m_animationGroup->animationCount()) == m_actions.size() + 1
    );
    // computePositions will give us the positions and sizes of the container and the positions of the actions. They are
    // provided as a pair, where the first element is the initial value and the second element is the final value.
    auto [containerRect, actionsPositions] = computePositions();
    // In the animation group, there must be precisely as much as animations as action, plus the animation of the
    // container, which is the first element. See updateActions, where these animations are created.
    auto* container_animation = qobject_cast<QPropertyAnimation*>(m_animationGroup->animationAt(0));
    SIGHT_ASSERT(
        "All animations inside the animation group must be QPropertyAnimation",
        container_animation != nullptr
    );
    container_animation->setStartValue(containerRect.first);
    container_animation->setEndValue(containerRect.second);
    m_actionsContainer->setGeometry(container_animation->currentValue().toRect());
    for(std::size_t i = 0 ; i < actionsPositions.size() ; i++)
    {
        auto* animation = qobject_cast<QPropertyAnimation*>(m_animationGroup->animationAt(static_cast<int>(i) + 1));
        SIGHT_ASSERT("All animations inside the animation group must be QPropertyAnimation", animation != nullptr);
        animation->setStartValue(actionsPositions[i].first);
        animation->setEndValue(actionsPositions[i].second);
        m_actions[i]->move(animation->currentValue().toPoint());
    }
}

//------------------------------------------------------------------------------

speed_dial::Animations speed_dial::computePositions() const
{
    std::pair<QRect, QRect> container_rect;
    QPoint speed_dial_position_on_window = mapTo(window(), parentWidget()->pos());
    QPoint container_position            = speed_dial_position_on_window;
    QSize container_final_size;
    // Firstly, we must compute what will be the initial container position and the  final container size. If the
    // direction is vertical (either up or down), the width is determined by the "fattest" action, and the height is the
    // sum of the height of all the actions including the spacing, and if the direction is horizontal, the width is the
    // sum of the width of all the actions including the spacing, and the height is the height of the biggest action.
    // In order for the container to be centered on the speed dial, its position is adapted. The container won't however
    // exceed the window.
    if(m_direction == Direction::UP || m_direction == Direction::DOWN)
    {
        int fattest_widget_width = 0;
        int total_height         = static_cast<int>(m_actions.size() - 1) * m_spacing;
        for(const QWidget* w : m_actions)
        {
            fattest_widget_width = std::max(fattest_widget_width, w->width());
            total_height        += w->height();
        }

        container_final_size.setWidth(fattest_widget_width);
        container_final_size.setHeight(total_height);
        container_position.setX(
            std::clamp(
                speed_dial_position_on_window.x() + (width() - fattest_widget_width) / 2,
                0,
                window()->width() - fattest_widget_width
            )
        );
    }
    else
    {
        int biggest_widget_height = 0;
        int total_width           = static_cast<int>(m_actions.size() - 1) * m_spacing;
        for(const QWidget* w : m_actions)
        {
            biggest_widget_height = std::max(biggest_widget_height, w->height());
            total_width          += w->width();
        }

        container_final_size.setWidth(total_width);
        container_final_size.setHeight(biggest_widget_height);
        container_position.setY(
            std::clamp(
                speed_dial_position_on_window.y() + (height() - biggest_widget_height) / 2,
                0,
                window()->height() - biggest_widget_height
            )
        );
    }

    container_rect.first = QRect(container_position, size());
    // Then, we compute the final position of the container. In the initial position, the container overlaps with the
    // speed dial, while in its final position, it must be aside. The spacing must be applied between the speed dial and
    // the container (it will effectively be the spacing between the speed dial and the first button). Since in Qt the
    // position of a widget is determined by its top left point, there are two main cases: we are either in the
    // "positive" world (down and right), where we must add the width/height of the speed dial to its position, or in
    // the "negative" world (up and left), where we must substract the final width/height of the container to its
    // position.
    QPoint delta;
    if(m_direction == Direction::UP)
    {
        delta = QPoint(0, -container_final_size.height() - m_spacing);
    }
    else if(m_direction == Direction::LEFT)
    {
        delta = QPoint(-container_final_size.width() - m_spacing, 0);
    }
    else if(m_direction == Direction::DOWN)
    {
        delta = QPoint(0, height() + m_spacing);
    }
    else
    {
        delta = QPoint(width() + m_spacing, 0);
    }

    container_rect.second = QRect(container_position + delta, container_final_size);
    // Finally, we compute the position of the actions within the container. The actions must be centered in the
    // container, and an offset must be calculated to place them evenly within the container. If the direction is
    // vertical (up or down), x is calculated so that the action is in the center of the container, which is the width
    // of the container minus its width. Its y (offset) will be the cumulated height of the previous widgets including
    // spacing. If the direction is horizontal (left or right), y is the height of the container minus its height, and
    // its x (offset) is the cumulated width of the previous widgets including spacing.
    std::vector<std::pair<QPoint, QPoint> > actions_positions;
    int cumulated_size = 0;
    for(int i = 0 ; const QWidget * w : m_actions)
    {
        std::pair<QPoint, QPoint> action_positions;
        int offset = cumulated_size + i * m_spacing;
        if(m_direction == Direction::UP || m_direction == Direction::DOWN)
        {
            int x = container_final_size.width() - w->width();
            action_positions.first  = QPoint(x, 0);
            action_positions.second = QPoint(x, offset);
        }
        else
        {
            int y = container_final_size.height() - w->height();
            action_positions.first  = QPoint(0, y);
            action_positions.second = QPoint(offset, y);
        }

        actions_positions.push_back(action_positions);

        cumulated_size += m_direction == Direction::UP || m_direction == Direction::DOWN ? w->height() : w->width();
        i++;
    }

    return {container_rect, actions_positions};
}

} // namespace sight::ui::qt::widget
