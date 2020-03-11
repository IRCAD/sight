/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwGuiQt/dialog/NotificationDialog.hpp"

#include "fwGuiQt/container/QtContainer.hpp"

#include <fwGui/registry/macros.hpp>

#include <QApplication>
#include <QTimer>

fwGuiRegisterMacro( ::fwGuiQt::dialog::NotificationDialog, ::fwGui::dialog::INotificationDialog::REGISTRY_KEY );

namespace fwGuiQt
{
namespace dialog
{
NotificationDialog::NotificationDialog(::fwGui::GuiBaseObject::Key )
{
}

//------------------------------------------------------------------------------

NotificationDialog::~NotificationDialog()
{
}

//------------------------------------------------------------------------------

void NotificationDialog::show()
{
    // Check if we have a Parent widget.
    QWidget* parent = qApp->activeWindow();

    ::fwGuiQt::container::QtContainer::csptr parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        m_parentContainer);

    // Replace the activeWindow by the parentContainer if exists.
    if(parentContainer)
    {
        parent = parentContainer->getQtContainer();
    }

    const QPoint position = computePosition(parent);

    QTimer fadeOutTime;
    fadeOutTime.setSingleShot(true);
    fadeOutTime.setInterval(m_duration);

    // Fade in effect.
    QGraphicsOpacityEffect* const effect = new QGraphicsOpacityEffect();
    QPropertyAnimation* const a          = new QPropertyAnimation(effect, "opacity");
    a->setDuration(500);  // in miliseconds
    a->setStartValue(0); // Full transparent.
    a->setEndValue(0.9); // 90% of opacity, to see through the popup.
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    m_msgBox = new ClickableQLabel(parent);
    m_msgBox->setMinimumSize(static_cast<int>(m_size[0]), static_cast<int>(m_size[1]));
    m_msgBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    m_msgBox->setWordWrap(true);
    m_msgBox->setScaledContents(true);
    m_msgBox->setText(QString::fromStdString(m_message));

    m_msgBox->move(position);

    if(qApp->styleSheet().isEmpty()) // If no styleSheet is used.
    {
        if(m_notificationType == INotificationDialog::Type::SUCCESS)
        {
            m_msgBox->setStyleSheet(
                "background-color:#58D68D;color:white;font-weight: bold;font-size: 16px;border-radius: 10px");
        }
        else if(m_notificationType == INotificationDialog::Type::FAILURE)
        {
            m_msgBox->setStyleSheet(
                "background-color:#E74C3C;color:white;font-weight: bold;font-size: 16px;border-radius: 10px");
        }
        else // INFO by default.
        {
            m_msgBox->setStyleSheet(
                "background-color:#5DADE2;color:white;font-weight: bold;font-size: 16px;border-radius: 10px");
        }
    }
    else
    {
        if(m_notificationType == INotificationDialog::Type::SUCCESS)
        {
            m_msgBox->setObjectName("NotificationDialog_Success");
        }
        else if(m_notificationType == INotificationDialog::Type::FAILURE)
        {
            m_msgBox->setObjectName("NotificationDialog_Failure");
        }
        else // INFO by default.
        {
            m_msgBox->setObjectName("NotificationDialog_Info");
        }
    }

    // fade in effect.
    m_msgBox->setGraphicsEffect(effect);
    // Align text at center.
    m_msgBox->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // Remove Title & Status bar, and display always on top.
    m_msgBox->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    /// Close when clicked.
    QObject::connect(m_msgBox, &ClickableQLabel::clicked, m_msgBox, &ClickableQLabel::close);

    /// Display it.
    m_msgBox->show();

    /// Launch Timer and fadeout before closing.
    QTimer::singleShot(m_duration, m_msgBox, &ClickableQLabel::fadeout);
}

//------------------------------------------------------------------------------

bool NotificationDialog::isVisible() const
{
    if(!m_msgBox)
    {
        return false;
    }

    return m_msgBox->isVisible();
}

//------------------------------------------------------------------------------

void NotificationDialog::close() const
{
    if(m_msgBox)
    {
        // Closing after a fade out effect.
        m_msgBox->fadeout();
    }
}

//------------------------------------------------------------------------------

QPoint NotificationDialog::computePosition(const QWidget* _parent) const
{
    QPoint position;

    const QPoint margins(5, 5); // keep margins from border of active window.

    // Use _parent containter as basis.
    if(_parent)
    {
        if(m_position == Position::CENTERED )
        {
            const auto parentPosCenter = _parent->rect().center();

            position = QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                              parentPosCenter.y() - static_cast<int>(m_size[1] / 2));

        }
        else if (m_position == Position::CENTERED_TOP)
        {
            const auto parentPosCenter = _parent->rect().center();
            const auto parentPosTop    = _parent->rect().top();

            position = QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                              parentPosTop + margins.y() +
                              ((parentPosTop + margins.y() + static_cast<int>(m_size[1])) *
                               static_cast<int>(m_index)));

        }
        else if (m_position == Position::CENTERED_BOTTOM)
        {
            const auto parentPosCenter = _parent->rect().center();
            const auto parentPosBottom = _parent->rect().bottom();

            position = QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                              parentPosBottom - margins.y()- static_cast<int>(m_size[1]));

            position.setY( position.y() - (static_cast<int>(m_index) * (static_cast<int>(m_size[1]) + margins.y())) );
        }
        else if(m_position == Position::TOP_LEFT)
        {
            const auto parentPosTopLeft = _parent->rect().topLeft();

            position = QPoint((parentPosTopLeft.x() + margins.x()),
                              parentPosTopLeft.y() + margins.y() +
                              (  (parentPosTopLeft.y() + margins.y() + static_cast<int>(m_size[1])) *
                                 static_cast<int>(m_index)));
        }
        else if(m_position == Position::TOP_RIGHT)
        {
            const auto parentPosTopRight = _parent->rect().topRight();

            position = QPoint((parentPosTopRight.x() - margins.x()) - static_cast<int>(m_size[0]),
                              parentPosTopRight.y() + margins.y() +
                              (  (parentPosTopRight.y() + margins.y() + static_cast<int>(m_size[1])) *
                                 static_cast<int>(m_index)));
        }
        else if(m_position == Position::BOTTOM_LEFT)
        {
            const auto parentPosBottomLeft = _parent->rect().bottomLeft();

            position = QPoint(parentPosBottomLeft.x() + margins.x(),
                              parentPosBottomLeft.y() - margins.y() - static_cast<int>(m_size[1]));

            position.setY( position.y() - (static_cast<int>(m_index) * (static_cast<int>(m_size[1]) + margins.y())) );
        }
        else if(m_position == Position::BOTTOM_RIGHT)
        {
            const auto parentPosBottomRight = _parent->rect().bottomRight();

            position = QPoint((parentPosBottomRight.x() - margins.x()) - static_cast<int>(m_size[0]),
                              parentPosBottomRight.y() - margins.y() - static_cast<int>(m_size[1]));

            position.setY( position.y() - (static_cast<int>(m_index) * (static_cast<int>(m_size[1]) + margins.y())) );
        }
    }
    else // FIXME: if no active window where to pop the notification ?
    {
        position = margins;
    }

    return position;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQt
