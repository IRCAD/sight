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

#include "SpeedDial.hpp"

#include <core/spy_log.hpp>

#include <QEvent>
#include <QLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

namespace sight::ui::qt::widget
{

SpeedDial::SpeedDial(Direction direction, int spacing, int animationDuration, QWidget* parent) :
    QPushButton(parent),
    m_direction(direction),
    m_spacing(spacing),
    m_animationDuration(animationDuration),
    m_animationGroup(new QParallelAnimationGroup)
{
    setEnabled(false);
    setCheckable(true);
    m_actionsContainer->setObjectName(objectName() + "Actions");
    infect(parent);
    QObject::connect(this, &QPushButton::clicked, this, &SpeedDial::toggleFold);
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

void SpeedDial::setDirection(Direction direction)
{
    m_direction = direction;
}

//------------------------------------------------------------------------------

SpeedDial::Direction SpeedDial::direction() const
{
    return m_direction;
}

//------------------------------------------------------------------------------

void SpeedDial::setSpacing(int spacing)
{
    m_spacing = spacing;
}

//------------------------------------------------------------------------------

int SpeedDial::spacing() const
{
    return m_spacing;
}

//------------------------------------------------------------------------------

void SpeedDial::setAnimationDuration(int animationDuration)
{
    // It seems there is a bug when animationDuration=1; QAbstractAnimation::finished doesn't trigger, and as such the
    // buttons don't hide when folded.
    if(animationDuration == 1)
    {
        m_animationDuration = 0;
    }
    else
    {
        m_animationDuration = animationDuration;
    }
}

//------------------------------------------------------------------------------

int SpeedDial::animationDuration() const
{
    return m_animationDuration;
}

//------------------------------------------------------------------------------

void SpeedDial::fold()
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

void SpeedDial::unfold()
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

void SpeedDial::toggleFold()
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

bool SpeedDial::isFolded() const
{
    return m_folded;
}

//------------------------------------------------------------------------------

void SpeedDial::updateActions(std::vector<QWidget*> actions)
{
    m_actions = std::move(actions);
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
    int currentTime = m_animationGroup->currentTime(); // Current time get resetted when calling QAnimationGroup::clear
    m_animationGroup->clear();
    auto* containerAnimation = new QPropertyAnimation(m_actionsContainer, "geometry");
    containerAnimation->setDuration(m_animationDuration);
    containerAnimation->setCurrentTime(currentTime);
    m_animationGroup->addAnimation(containerAnimation);
    for(QWidget* w : m_actions)
    {
        auto* animation = new QPropertyAnimation(w, "pos");
        animation->setDuration(m_animationDuration);
        animation->setCurrentTime(currentTime);
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

const std::vector<QWidget*>& SpeedDial::actions() const
{
    return m_actions;
}

//------------------------------------------------------------------------------

bool SpeedDial::eventFilter(QObject* o, QEvent* e)
{
    QWidget* w = qobject_cast<QWidget*>(o);
    if(w == nullptr)
    {
        return false;
    }

    // The positions of the actions must be updated if:
    // - The size of one of the actions changes
    // - One of the ancestors of the speed dial is resized of moves
    if((std::ranges::find(m_actions, w) != m_actions.end() && e->type() == QEvent::Resize)
       || (o->findChildren<SpeedDial*>(objectName()).contains(this)
           && (e->type() == QEvent::Resize || e->type() == QEvent::Move)))
    {
        updateActionsPositions();
    }

    return false;
}

//------------------------------------------------------------------------------

bool SpeedDial::event(QEvent* e)
{
    if(e->type() == QEvent::ParentChange)
    {
        infect(parent());
        m_actionsContainer->setParent(window());
    }

    return QPushButton::event(e);
}

//------------------------------------------------------------------------------

void SpeedDial::moveEvent(QMoveEvent* e)
{
    QPushButton::moveEvent(e);
    updateActionsPositions();
}

//------------------------------------------------------------------------------

void SpeedDial::resizeEvent(QResizeEvent* e)
{
    QPushButton::resizeEvent(e);
    updateActionsPositions();
    bool isInAnOverlay = false;
    for(QWidget* child = this ; !isInAnOverlay && child->parentWidget() != nullptr ; child = child->parentWidget())
    {
        QLayout* parentLayout = child->parentWidget()->layout();
        isInAnOverlay = (parentLayout == nullptr || parentLayout->indexOf(child) == -1);
    }

    if(isInAnOverlay)
    {
        setIconSize(size());
    }
}

//------------------------------------------------------------------------------

void SpeedDial::infect(QObject* parent)
{
    if(parent == nullptr)
    {
        return;
    }

    parent->installEventFilter(this);
    infect(parent->parent());
}

//------------------------------------------------------------------------------

void SpeedDial::updateActionsPositions()
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
    auto* containerAnimation = qobject_cast<QPropertyAnimation*>(m_animationGroup->animationAt(0));
    SIGHT_ASSERT("All animations inside the animation group must be QPropertyAnimation", containerAnimation != nullptr);
    containerAnimation->setStartValue(containerRect.first);
    containerAnimation->setEndValue(containerRect.second);
    m_actionsContainer->setGeometry(containerAnimation->currentValue().toRect());
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

SpeedDial::Animations SpeedDial::computePositions() const
{
    std::pair<QRect, QRect> containerRect;
    QPoint speedDialPositionOnWindow = mapTo(window(), parentWidget()->pos());
    QPoint containerPosition         = speedDialPositionOnWindow;
    QSize containerFinalSize;
    // Firstly, we must compute what will be the initial container position and the  final container size. If the
    // direction is vertical (either up or down), the width is determined by the "fattest" action, and the height is the
    // sum of the height of all the actions including the spacing, and if the direction is horizontal, the width is the
    // sum of the width of all the actions including the spacing, and the height is the height of the biggest action.
    // In order for the container to be centered on the speed dial, its position is adapted. The container won't however
    // exceed the window.
    if(m_direction == Direction::UP || m_direction == Direction::DOWN)
    {
        int fattestWidgetWidth = 0;
        int totalHeight        = static_cast<int>(m_actions.size() - 1) * m_spacing;
        for(const QWidget* w : m_actions)
        {
            fattestWidgetWidth = std::max(fattestWidgetWidth, w->width());
            totalHeight       += w->height();
        }

        containerFinalSize.setWidth(fattestWidgetWidth);
        containerFinalSize.setHeight(totalHeight);
        containerPosition.setX(
            std::clamp(
                speedDialPositionOnWindow.x() + (width() - fattestWidgetWidth) / 2,
                0,
                window()->width() - fattestWidgetWidth
            )
        );
    }
    else
    {
        int biggestWidgetHeight = 0;
        int totalWidth          = static_cast<int>(m_actions.size() - 1) * m_spacing;
        for(const QWidget* w : m_actions)
        {
            biggestWidgetHeight = std::max(biggestWidgetHeight, w->height());
            totalWidth         += w->width();
        }

        containerFinalSize.setWidth(totalWidth);
        containerFinalSize.setHeight(biggestWidgetHeight);
        containerPosition.setY(
            std::clamp(
                speedDialPositionOnWindow.y() + (height() - biggestWidgetHeight) / 2,
                0,
                window()->height() - biggestWidgetHeight
            )
        );
    }

    containerRect.first = QRect(containerPosition, size());
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
        delta = QPoint(0, -containerFinalSize.height() - m_spacing);
    }
    else if(m_direction == Direction::LEFT)
    {
        delta = QPoint(-containerFinalSize.width() - m_spacing, 0);
    }
    else if(m_direction == Direction::DOWN)
    {
        delta = QPoint(0, height() + m_spacing);
    }
    else
    {
        delta = QPoint(width() + m_spacing, 0);
    }

    containerRect.second = QRect(containerPosition + delta, containerFinalSize);
    // Finally, we compute the position of the actions within the container. The actions must be centered in the
    // container, and an offset must be calculated to place them evenly within the container. If the direction is
    // vertical (up or down), x is calculated so that the action is in the center of the container, which is the width
    // of the container minus its width. Its y (offset) will be the cumulated height of the previous widgets including
    // spacing. If the direction is horizontal (left or right), y is the height of the container minus its height, and
    // its x (offset) is the cumulated width of the previous widgets including spacing.
    std::vector<std::pair<QPoint, QPoint> > actionsPositions;
    int cumulatedSize = 0;
    for(int i = 0 ; const QWidget * w : m_actions)
    {
        std::pair<QPoint, QPoint> actionPositions;
        int offset = cumulatedSize + i * m_spacing;
        if(m_direction == Direction::UP || m_direction == Direction::DOWN)
        {
            int x = containerFinalSize.width() - w->width();
            actionPositions.first  = QPoint(x, 0);
            actionPositions.second = QPoint(x, offset);
        }
        else
        {
            int y = containerFinalSize.height() - w->height();
            actionPositions.first  = QPoint(0, y);
            actionPositions.second = QPoint(offset, y);
        }

        actionsPositions.push_back(actionPositions);

        cumulatedSize += m_direction == Direction::UP || m_direction == Direction::DOWN ? w->height() : w->width();
        i++;
    }

    return {containerRect, actionsPositions};
}

} // namespace sight::ui::qt::widget
