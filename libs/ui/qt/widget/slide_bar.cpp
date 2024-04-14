/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "ui/qt/widget/slide_bar.hpp"

#include "ui/qt/qt_main_frame.hpp"

#include <core/spy_log.hpp>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWindow>

namespace sight::ui::qt::widget
{

//-----------------------------------------------------------------------------

slide_bar::slide_bar(
    QWidget* _parent,
    h_alignment _h_align,
    v_alignment _v_align,
    int _width,
    bool _percent_width,
    int _height,
    bool _percent_height,
    int _h_offset,
    bool _percent_h_offset,
    int _v_offset,
    bool _percent_v_offset,
    double _opacity,
    bool _animatable,
    animatable_alignment _animatable_alignment
) :
    QWidget(_parent),
    m_h_alignment(_h_align),
    m_v_alignment(_v_align),
    m_width(_width),
    m_percent_width(_percent_width),
    m_height(_height),
    m_percent_height(_percent_height),
    m_h_offset(_h_offset),
    m_percent_h_offset(_percent_h_offset),
    m_v_offset(_v_offset),
    m_percent_v_offset(_percent_v_offset),
    m_opacity(_opacity),
    m_animatable(_animatable),
    m_animatable_alignment(_animatable_alignment)
{
    this->init();
}

//-----------------------------------------------------------------------------

void slide_bar::init()
{
    this->setObjectName("slide_bar");

    // Set the widget position
    this->update_position();

    // window flags to have a frameless dialog that can be displayed over an openGL widget
    this->setWindowFlags(
        Qt::Tool
        | Qt::FramelessWindowHint
        | Qt::NoDropShadowWindowHint
    );

    // Set the transparent background
    this->setAttribute(Qt::WA_TranslucentBackground);

    // Listen parent and activeWindow 'Resize' and 'Move' event to update widget position
    // (just listening parent event is not enough because the Move event is only send on activeWindow).
    this->parent()->installEventFilter(this);

    // Find the real root (when using opengl/ogre, there are intermediate "QWindow")
    QWidget* ancestor = this->parentWidget();
    while(ancestor->parentWidget() != nullptr)
    {
        ancestor = ancestor->parentWidget();
    }

    ancestor = ancestor->topLevelWidget();
    ancestor->installEventFilter(this);
}

//-----------------------------------------------------------------------------

slide_bar::~slide_bar()
= default;

//-----------------------------------------------------------------------------

void slide_bar::update_position()
{
    // Computes the size.
    int width  = this->parentWidget()->width();
    int height = this->parentWidget()->height();

    if(!m_percent_width)
    {
        if(m_width > -1)
        {
            width = std::min(width, m_width);
        }
    }
    else
    {
        width = std::min(width, static_cast<int>(m_width / 100. * width));
    }

    if(!m_percent_height)
    {
        if(m_height > -1)
        {
            height = std::min(height, m_height);
        }
    }
    else
    {
        height = std::min(height, static_cast<int>(m_height / 100. * height));
    }

    this->setFixedWidth(width);
    this->setFixedHeight(height);

    // Computes the offset.
    int h_offset = 0;
    int v_offset = 0;

    if(!m_percent_h_offset)
    {
        h_offset = m_h_offset;
    }
    else
    {
        h_offset = static_cast<int>(m_h_offset / 100. * this->parentWidget()->width());
    }

    if(!m_percent_v_offset)
    {
        v_offset = m_v_offset;
    }
    else
    {
        v_offset = static_cast<int>(m_v_offset / 100. * this->parentWidget()->height());
    }

    // Compute the shown and hidden position.
    if(m_h_alignment == left && m_v_alignment == top)
    {
        QPoint pos = this->parentWidget()->rect().topLeft();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shown_position = QRect(pos.x() + h_offset, pos.y() + v_offset, width, height);
    }
    else if(m_h_alignment == right && m_v_alignment == top)
    {
        QPoint pos = this->parentWidget()->rect().topRight();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shown_position = QRect(pos.x() - width - h_offset, pos.y() + v_offset, width, height);
    }
    else if(m_h_alignment == left && m_v_alignment == bottom)
    {
        QPoint pos = this->parentWidget()->rect().bottomLeft();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shown_position = QRect(pos.x() + h_offset, pos.y() - height - v_offset, width, height);
    }
    else if(m_h_alignment == right && m_v_alignment == bottom)
    {
        QPoint pos = this->parentWidget()->rect().bottomRight();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shown_position = QRect(pos.x() - width - h_offset, pos.y() - height - v_offset, width, height);
    }

    switch(m_animatable_alignment)
    {
        case top_animation:
        {
            QPoint pos = this->parentWidget()->rect().topLeft();
            pos               = this->parentWidget()->mapToGlobal(pos);
            m_hidden_position = QRect(m_shown_position.x(), pos.y() - height + v_offset, width, 0);
            break;
        }

        case bottom_animation:
        {
            QPoint pos = this->parentWidget()->rect().bottomLeft();
            pos               = this->parentWidget()->mapToGlobal(pos);
            m_hidden_position = QRect(m_shown_position.x(), pos.y() + v_offset, width, 0);
            break;
        }

        case left_animation:
        {
            QPoint pos = this->parentWidget()->rect().topLeft();
            pos               = this->parentWidget()->mapToGlobal(pos);
            m_hidden_position = QRect(pos.x() - width + h_offset, m_shown_position.y(), 0, height);
            break;
        }

        case right_animation:
        {
            QPoint pos = this->parentWidget()->rect().bottomRight();
            pos               = this->parentWidget()->mapToGlobal(pos);
            m_hidden_position = QRect(pos.x() + h_offset, m_shown_position.y(), 0, height);
            break;
        }
    }

    if(!m_animatable || m_is_shown)
    {
        this->setGeometry(m_shown_position);
    }
    else
    {
        this->setGeometry(m_hidden_position);
    }
}

//-----------------------------------------------------------------------------

void slide_bar::setVisible(bool _visible)
{
    this->slide(_visible);
}

//-----------------------------------------------------------------------------

void slide_bar::force_hide()
{
    this->QWidget::setVisible(false);
}

//-----------------------------------------------------------------------------

void slide_bar::force_show()
{
    this->QWidget::setVisible(true);
}

//-----------------------------------------------------------------------------

void slide_bar::slide(bool _visible)
{
    if(m_animatable)
    {
        // Show the widget with the previous opacity. It must be hidden after the slide(false) because if opacity == 0,
        // the widget is still clickable.
        this->force_show();
        this->setWindowOpacity(m_is_shown ? m_opacity : 0);

        // Set animation to slide the widget and update the opacity
        auto* animations = new QParallelAnimationGroup();

        // slide animation
        auto* geom_animation = new QPropertyAnimation(this, "geometry");
        geom_animation->setDuration(500);
        geom_animation->setEasingCurve(QEasingCurve::InBack);
        geom_animation->setStartValue(this->geometry());

        if(_visible)
        {
            geom_animation->setEndValue(m_shown_position);
        }
        else
        {
            geom_animation->setEndValue(m_hidden_position);

            // hide the widget when the animation is finished (if opacity == 0, widget is still clickable)
            QObject::connect(animations, &QAbstractAnimation::finished, this, &slide_bar::force_hide);
        }

        // opacity animation
        auto* opacity_animation = new QPropertyAnimation(this, "windowOpacity");
        opacity_animation->setDuration(500);
        opacity_animation->setEndValue(_visible ? m_opacity : 0);

        animations->addAnimation(geom_animation);
        animations->addAnimation(opacity_animation);

        animations->start(QPropertyAnimation::DeleteWhenStopped);
    }
    else if(_visible)
    {
        this->force_show();
    }
    else
    {
        this->force_hide();
    }

    m_is_shown = _visible;
}

//-----------------------------------------------------------------------------

bool slide_bar::eventFilter(QObject* _obj, QEvent* _event)
{
    // Update the widget position when the parent is moved or resized
    if(_event->type() == QEvent::Resize
       || _event->type() == QEvent::Move)
    {
        this->update_position();
    }
    else if(_event->type() == QEvent::Show)
    {
        if(m_is_shown)
        {
            this->force_show();
        }
    }
    else if(_event->type() == QEvent::Hide)
    {
        this->force_hide();
    }

    return QObject::eventFilter(_obj, _event);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::widget
