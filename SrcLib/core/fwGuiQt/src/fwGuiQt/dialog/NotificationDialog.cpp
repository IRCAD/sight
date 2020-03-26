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
#include <QBoxLayout>
#include <QTimer>

fwGuiRegisterMacro(::fwGuiQt::dialog::NotificationDialog, ::fwGui::dialog::INotificationDialog::REGISTRY_KEY);

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

    ::fwGuiQt::container::QtContainer::csptr parentContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(m_parentContainer);

    // Replace the activeWindow by the parentContainer if exists.
    if(parentContainer)
    {
        parent = parentContainer->getQtContainer();
    }

    // Create the clikable label.
    m_msgBox = new ClickableQLabel();
    m_msgBox->setMinimumSize(static_cast<int>(m_size[0]), static_cast<int>(m_size[1]));
    m_msgBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    m_msgBox->setWordWrap(true);
    m_msgBox->setScaledContents(true);
    m_msgBox->setText(QString::fromStdString(m_message));
    m_msgBox->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // If no styleSheet is used.
    if(qApp->styleSheet().isEmpty())
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

    // Fade in effect.
    QGraphicsOpacityEffect* const effect = new QGraphicsOpacityEffect();
    QPropertyAnimation* const a          = new QPropertyAnimation(effect, "opacity");
    a->setDuration(500);  // in miliseconds
    a->setStartValue(0); // Full transparent.
    a->setEndValue(0.9); // 90% of opacity, to see through the popup.
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    // Create tye main translucent container.
    QWidget* const container = new QWidget(parent);
    container->setGraphicsEffect(effect);
    container->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint| Qt::NoDropShadowWindowHint);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setContentsMargins(0, 0, 0, 0);

    // Set it at the right position.
    const QPoint position = computePosition(parent);
    container->move(position);

    // Give it a layout with the clickable label.
    QBoxLayout* const layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(layout);
    layout->addWidget(m_msgBox);

    // Close when clicked.
    QObject::connect(m_msgBox, &ClickableQLabel::clicked, container, &ClickableQLabel::close);

    // Display it.
    container->show();

    // Launch Timer and fadeout before closing.
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
    // keep margins from border of active window.
    const int margin = 5;

    // FIXME: if no active window where to pop the notification ?
    QPoint position(margin, margin);

    // Use _parent containter as basis.
    if(_parent)
    {
        if(m_position == Position::CENTERED )
        {
            const auto parentPosCenter = _parent->mapToGlobal(_parent->rect().center());

            position = QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                              parentPosCenter.y() - static_cast<int>(m_size[1] / 2));

        }
        else if (m_position == Position::CENTERED_TOP)
        {
            const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
            const int parentY = _parent->mapToGlobal(_parent->rect().topLeft()).y();
            const int height  = static_cast<int>(m_size[0]/2) + margin;

            position = QPoint(parentX - static_cast<int>(m_size[0] / 2),
                              parentY + margin + (height * m_index));

        }
        else if (m_position == Position::CENTERED_BOTTOM)
        {
            const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
            const int parentY = _parent->mapToGlobal(_parent->rect().bottomLeft()).y();
            const int height  = static_cast<int>(m_size[0]/2) + margin;

            position = QPoint(parentX - static_cast<int>(m_size[0] / 2),
                              parentY - margin - (height * (m_index+1)));
        }
        else if(m_position == Position::TOP_LEFT)
        {
            const auto parrentTopLeft = _parent->mapToGlobal(_parent->rect().topLeft());
            const int parentX         = parrentTopLeft.x();
            const int parentY         = parrentTopLeft.y();
            const int height          = static_cast<int>(m_size[0]/2) + margin;

            position = QPoint(parentX + margin,
                              parentY + margin + (height * m_index));
        }
        else if(m_position == Position::TOP_RIGHT)
        {
            const auto parrentTopRight = _parent->mapToGlobal(_parent->rect().topRight());
            const int parentX          = parrentTopRight.x();
            const int parentY          = parrentTopRight.y();
            const int height           = static_cast<int>(m_size[0]/2) + margin;

            position = QPoint(parentX - margin - static_cast<int>(m_size[0]),
                              parentY + margin + (height * m_index));
        }
        else if(m_position == Position::BOTTOM_LEFT)
        {
            const auto parrentBottomLeft = _parent->mapToGlobal(_parent->rect().bottomLeft());
            const int parentX            = parrentBottomLeft.x();
            const int parentY            = parrentBottomLeft.y();
            const int height             = static_cast<int>(m_size[0]/2) + margin;

            position = QPoint(parentX + margin,
                              parentY - margin - (height * (m_index+1)));
        }
        else if(m_position == Position::BOTTOM_RIGHT)
        {
            const auto parrentBottomRight = _parent->mapToGlobal(_parent->rect().bottomRight());
            const int parentX             = parrentBottomRight.x();
            const int parentY             = parrentBottomRight.y();
            const int height              = static_cast<int>(m_size[0]/2) + margin;

            position = QPoint(parentX - margin - static_cast<int>(m_size[0]),
                              parentY - margin - (height * (m_index+1)));
        }
    }

    return position;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQt
