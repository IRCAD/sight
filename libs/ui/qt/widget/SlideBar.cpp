/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "ui/qt/widget/SlideBar.hpp"

#include "ui/qt/QtMainFrame.hpp"

#include <core/spyLog.hpp>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWindow>

namespace sight::ui::qt
{

namespace widget
{

//-----------------------------------------------------------------------------

SlideBar::SlideBar(
    QWidget* _parent,
    HAlignment _hAlign,
    VAlignment _vAlign,
    int _width,
    bool _percentWidth,
    int _height,
    bool _percentHeight,
    int _hOffset,
    bool _percentHOffset,
    int _vOffset,
    bool _percentVOffset,
    double _opacity,
    bool _animatable,
    AnimatableAlignment _animatableAlignment
) :
    QWidget(_parent),
    m_hAlignment(_hAlign),
    m_vAlignment(_vAlign),
    m_width(_width),
    m_percentWidth(_percentWidth),
    m_height(_height),
    m_percentHeight(_percentHeight),
    m_hOffset(_hOffset),
    m_percentHOffset(_percentHOffset),
    m_vOffset(_vOffset),
    m_percentVOffset(_percentVOffset),
    m_opacity(_opacity),
    m_animatable(_animatable),
    m_animatableAlignment(_animatableAlignment)
{
    this->init();
}

//-----------------------------------------------------------------------------

void SlideBar::init()
{
    this->setObjectName("SlideBar");

    // Set the widget position
    this->updatePosition();

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

SlideBar::~SlideBar()
{
}

//-----------------------------------------------------------------------------

void SlideBar::updatePosition()
{
    // Computes the size.
    int width  = this->parentWidget()->width();
    int height = this->parentWidget()->height();

    if(!m_percentWidth)
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

    if(!m_percentHeight)
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
    int hOffset = 0;
    int vOffset = 0;

    if(!m_percentHOffset)
    {
        hOffset = m_hOffset;
    }
    else
    {
        hOffset = static_cast<int>(m_hOffset / 100. * this->parentWidget()->width());
    }

    if(!m_percentVOffset)
    {
        vOffset = m_vOffset;
    }
    else
    {
        vOffset = static_cast<int>(m_vOffset / 100. * this->parentWidget()->height());
    }

    // Compute the shown and hidden position.
    if(m_hAlignment == LEFT && m_vAlignment == TOP)
    {
        QPoint pos = this->parentWidget()->rect().topLeft();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition = QRect(pos.x() + hOffset, pos.y() + vOffset, width, height);
    }
    else if(m_hAlignment == RIGHT && m_vAlignment == TOP)
    {
        QPoint pos = this->parentWidget()->rect().topRight();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition = QRect(pos.x() - width - hOffset, pos.y() + vOffset, width, height);
    }
    else if(m_hAlignment == LEFT && m_vAlignment == BOTTOM)
    {
        QPoint pos = this->parentWidget()->rect().bottomLeft();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition = QRect(pos.x() + hOffset, pos.y() - height - vOffset, width, height);
    }
    else if(m_hAlignment == RIGHT && m_vAlignment == BOTTOM)
    {
        QPoint pos = this->parentWidget()->rect().bottomRight();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition = QRect(pos.x() - width - hOffset, pos.y() - height - vOffset, width, height);
    }

    switch(m_animatableAlignment)
    {
        case TOP_ANIMATION:
        {
            QPoint pos = this->parentWidget()->rect().topLeft();
            pos              = this->parentWidget()->mapToGlobal(pos);
            m_hiddenPosition = QRect(m_shownPosition.x(), pos.y() - height + vOffset, width, 0);
            break;
        }

        case BOTTOM_ANIMATION:
        {
            QPoint pos = this->parentWidget()->rect().bottomLeft();
            pos              = this->parentWidget()->mapToGlobal(pos);
            m_hiddenPosition = QRect(m_shownPosition.x(), pos.y() + vOffset, width, 0);
            break;
        }

        case LEFT_ANIMATION:
        {
            QPoint pos = this->parentWidget()->rect().topLeft();
            pos              = this->parentWidget()->mapToGlobal(pos);
            m_hiddenPosition = QRect(pos.x() - width + hOffset, m_shownPosition.y(), 0, height);
            break;
        }

        case RIGHT_ANIMATION:
        {
            QPoint pos = this->parentWidget()->rect().bottomRight();
            pos              = this->parentWidget()->mapToGlobal(pos);
            m_hiddenPosition = QRect(pos.x() + hOffset, m_shownPosition.y(), 0, height);
            break;
        }
    }

    if(!m_animatable || m_isShown)
    {
        this->setGeometry(m_shownPosition);
    }
    else
    {
        this->setGeometry(m_hiddenPosition);
    }
}

//-----------------------------------------------------------------------------

void SlideBar::setVisible(bool _visible)
{
    this->slide(_visible);
}

//-----------------------------------------------------------------------------

void SlideBar::forceHide()
{
    this->QWidget::setVisible(false);
}

//-----------------------------------------------------------------------------

void SlideBar::forceShow()
{
    this->QWidget::setVisible(true);
}

//-----------------------------------------------------------------------------

void SlideBar::slide(bool _visible)
{
    if(m_animatable)
    {
        // Show the widget with the previous opacity. It must be hidden after the slide(false) because if opacity == 0,
        // the widget is still clickable.
        this->forceShow();
        this->setWindowOpacity(m_isShown ? m_opacity : 0);

        // Set animation to slide the widget and update the opacity
        QParallelAnimationGroup* animations = new QParallelAnimationGroup();

        // slide animation
        QPropertyAnimation* geomAnimation = new QPropertyAnimation(this, "geometry");
        geomAnimation->setDuration(500);
        geomAnimation->setEasingCurve(QEasingCurve::InBack);
        geomAnimation->setStartValue(this->geometry());

        if(_visible == true)
        {
            geomAnimation->setEndValue(m_shownPosition);
        }
        else
        {
            geomAnimation->setEndValue(m_hiddenPosition);

            // hide the widget when the animation is finished (if opacity == 0, widget is still clickable)
            QObject::connect(animations, &QAbstractAnimation::finished, this, &SlideBar::forceHide);
        }

        // opacity animation
        QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "windowOpacity");
        opacityAnimation->setDuration(500);
        opacityAnimation->setEndValue(_visible ? m_opacity : 0);

        animations->addAnimation(geomAnimation);
        animations->addAnimation(opacityAnimation);

        animations->start(QPropertyAnimation::DeleteWhenStopped);
    }
    else if(_visible)
    {
        this->forceShow();
    }
    else
    {
        this->forceHide();
    }

    m_isShown = _visible;
}

//-----------------------------------------------------------------------------

bool SlideBar::eventFilter(QObject* _obj, QEvent* _event)
{
    // Update the widget position when the parent is moved or resized
    if(_event->type() == QEvent::Resize
       || _event->type() == QEvent::Move)
    {
        this->updatePosition();
    }
    else if(_event->type() == QEvent::Show)
    {
        if(m_isShown)
        {
            this->forceShow();
        }
    }
    else if(_event->type() == QEvent::Hide)
    {
        this->forceHide();
    }

    return QObject::eventFilter(_obj, _event);
}

//-----------------------------------------------------------------------------

} // namespace widget

} // namespace sight::ui::qt
