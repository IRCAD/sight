/************************************************************************
 *
 * Copyright (C) 2016-2018 IRCAD France
 * Copyright (C) 2016-2018 IHU Strasbourg
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

#include "fwGuiQt/widget/SlideBar.hpp"

#include "fwGuiQt/QtMainFrame.hpp"

#include <fwCore/spyLog.hpp>

#include <QHBoxLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>

namespace fwGuiQt
{

namespace widget
{

//-----------------------------------------------------------------------------

SlideBar::SlideBar(QWidget* parent, Aligment align, int buttonSize, double opacity) :
    QGroupBox(),
    m_buttonSize(buttonSize),
    m_opacity(opacity),
    m_isShown(false),
    m_align(align)
{
    this->setParent(parent);
    this->init();
}

//-----------------------------------------------------------------------------

void SlideBar::init()
{
    this->setObjectName("SlideBar");

    // Set the widget position
    this->updatePosition();
    this->setGeometry(m_hiddenPosition);

    // window flags to have a frameless dialog that can be displayed over an openGL widget
    this->setWindowFlags(Qt::Tool
                         | Qt::FramelessWindowHint
                         | Qt::NoDropShadowWindowHint
                         );

    // Set the transparent background
    this->setAttribute(Qt::WA_TranslucentBackground);

    // Listen parent and activeWindow 'Resize' and 'Move' event to update widget position
    // (just listening parent event is not enough because the Move event is only send on activeWindow).
    this->parent()->installEventFilter(this);

    auto activeWindow = qApp->activeWindow();
    if(activeWindow)
    {
        activeWindow->installEventFilter(this);
    }
}

//-----------------------------------------------------------------------------

SlideBar::~SlideBar()
{
}

//-----------------------------------------------------------------------------

void SlideBar::updatePosition()
{
    int height = this->parentWidget()->height();
    int width  = this->parentWidget()->width();

    switch (m_align)
    {
        case LEFT:
        case RIGHT:
            width = std::min(width, m_buttonSize);
            this->setFixedWidth(width);
            this->setFixedHeight(height);
            break;
        case TOP:
        case BOTTOM:
            height = std::min(height, m_buttonSize);
            this->setFixedHeight(height);
            this->setFixedWidth(width);
            break;
    }

    if (m_align == LEFT)
    {
        QPoint pos = this->parentWidget()->rect().topLeft();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition  = QRect(pos.x(), pos.y(), width, height);
        m_hiddenPosition = QRect(pos.x()-width, pos.y(), 0, height);
    }
    else if (m_align == RIGHT)
    {
        QPoint pos = this->parentWidget()->rect().topRight();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition  = QRect(pos.x()-width, pos.y(), width, height);
        m_hiddenPosition = QRect(pos.x(), pos.y(), 0, height);
    }
    else if (m_align == TOP)
    {
        QPoint pos = this->parentWidget()->rect().topLeft();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition  = QRect(pos.x(), pos.y(), width, height);
        m_hiddenPosition = QRect(pos.x(), pos.y()-height, width, 0);
    }
    else // m_align == BOTTOM
    {
        QPoint pos = this->parentWidget()->rect().bottomLeft();
        pos = this->parentWidget()->mapToGlobal(pos);

        m_shownPosition  = QRect(pos.x(), pos.y()-height, width, height);
        m_hiddenPosition = QRect(pos.x(), pos.y(), width, 0);
    }

    if (m_isShown)
    {
        this->setGeometry(m_shownPosition);
    }
    else
    {
        this->setGeometry(m_hiddenPosition);
    }
}

//-----------------------------------------------------------------------------

void SlideBar::setSide(Aligment align)
{
    m_align = align;
    this->updatePosition();
}
//-----------------------------------------------------------------------------

void SlideBar::setVisible(bool visible)
{
    this->slide(visible);
}

//-----------------------------------------------------------------------------

void SlideBar::forceHide()
{
    this->QGroupBox::setVisible(false);
}

//-----------------------------------------------------------------------------

void SlideBar::forceShow()
{
    this->QGroupBox::setVisible(true);
}

//-----------------------------------------------------------------------------

void SlideBar::slide(bool visible)
{
    // Show the widget with the previous opacity. It must be hidden after the slide(false) because if opacity == 0, the
    // widget is still clickable.
    this->forceShow();
    this->setWindowOpacity(m_isShown ? m_opacity : 0);

    // Set animation to slide the widget and update the opacity
    QParallelAnimationGroup* animations = new QParallelAnimationGroup();

    // slide animation
    QPropertyAnimation* geomAnimation = new QPropertyAnimation(this, "geometry");
    geomAnimation->setDuration(500);
    geomAnimation->setEasingCurve(QEasingCurve::InBack);
    geomAnimation->setStartValue(this->geometry());

    if(visible == true)
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
    opacityAnimation->setEndValue(visible ? m_opacity : 0);

    animations->addAnimation(geomAnimation);
    animations->addAnimation(opacityAnimation);

    animations->start(QPropertyAnimation::DeleteWhenStopped);
    m_isShown = visible;
}

//-----------------------------------------------------------------------------

bool SlideBar::eventFilter(QObject* obj, QEvent* event)
{
    // Update the widget position when the parent is moved or resized
    if (event->type() == QEvent::Resize
        || event->type() == QEvent::Move)
    {
        this->updatePosition();
    }
    else if (event->type() == QEvent::WindowActivate)
    {
        auto activeWindow = qApp->activeWindow();
        SLM_ASSERT("No active window", activeWindow);
        activeWindow->installEventFilter(this);
        this->updatePosition();
    }
    else if (event->type() == QEvent::WindowDeactivate)
    {
        auto mainFrame = dynamic_cast< ::fwGuiQt::QtMainFrame*>(obj);
        if(mainFrame)
        {
            mainFrame->removeEventFilter(this);
        }
    }
    return QObject::eventFilter(obj, event);
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace fwGuiQt
