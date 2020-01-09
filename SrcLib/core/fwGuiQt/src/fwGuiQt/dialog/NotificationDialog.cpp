/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <fwGui/registry/macros.hpp>

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
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
    QWidget* win    = qApp->activeWindow();
    QPoint position = computePosition();

    // Fade in effect.
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    QPropertyAnimation* a          = new QPropertyAnimation(effect, "opacity");
    a->setDuration(1000);  // in miliseconds
    a->setStartValue(0);
    a->setEndValue(0.9);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    ClickableQLabel* msgbox = new ClickableQLabel(win);
    msgbox->setMinimumSize(static_cast<int>(m_size[0]), static_cast<int>(m_size[1]));
    msgbox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    msgbox->setWordWrap(true);
    msgbox->setScaledContents(true);
    msgbox->setText(QString::fromStdString(m_message));

    msgbox->move(position);

    if(m_notificationType == INotificationDialog::Type::SUCCESS)
    {
        msgbox->setStyleSheet("background-color:#58D68D;color:white;font-weight: bold;font-size: 16px");
    }
    else if(m_notificationType == INotificationDialog::Type::FAILURE)
    {
        msgbox->setStyleSheet("background-color:#E74C3C;color:white;font-weight: bold;font-size: 16px");
    }
    else if(m_notificationType == INotificationDialog::Type::INFO)
    {
        msgbox->setStyleSheet("background-color:#5DADE2;color:white;font-weight: bold;font-size: 16px");
    }
    else
    {
        // Default style.
    }

    // fade in effect.
    msgbox->setGraphicsEffect(effect);
    // Align text at center.
    msgbox->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // Remove Title & Status bar, and display always on top.
    msgbox->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // Add rounded borders.
    QPainterPath path;
    path.addRoundedRect(msgbox->rect(), 10, 10);
    msgbox->setMask(path.toFillPolygon().toPolygon());

    // Fade out effect as a lambda.
    auto fadeout = [msgbox]()
                   {
                       QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
                       msgbox->setGraphicsEffect(effect);
                       QPropertyAnimation* a = new QPropertyAnimation(effect, "opacity");
                       a->setDuration(1000); // it will took 1000ms to face out
                       a->setStartValue(0.9);
                       a->setEndValue(0);
                       a->setEasingCurve(QEasingCurve::OutBack);
                       a->start(QPropertyAnimation::DeleteWhenStopped);
                       QObject::connect(a, &QPropertyAnimation::finished, msgbox, &ClickableQLabel::close);
                   };

    /// Close when clicked.
    QObject::connect(msgbox, &ClickableQLabel::clicked, msgbox, &ClickableQLabel::close);

    /// Display it.
    msgbox->show();

    /// Launch Timer and fadeout before closing.
    QTimer::singleShot(m_duration, fadeout);
}

//------------------------------------------------------------------------------

QPoint NotificationDialog::computePosition()
{
    QWidget* win = qApp->activeWindow();
    QPoint position;

    const QPoint margins(5, 5); // keep margins from border of active window.

    // Use active window as basis.
    if(win)
    {
        if(m_position == Position::CENTERED )
        {
            const auto parentPosCenter = win->rect().center();

            position = QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                              parentPosCenter.y() - static_cast<int>(m_size[1] / 2));

        }
        // Pop notification on center top of active window
        else if (m_position == Position::CENTERED_TOP)
        {
            const auto parentPosCenter = win->rect().center();
            const auto parentPosTop    = win->rect().top();

            position = QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                              parentPosTop + margins.y());

        }
        else if (m_position == Position::CENTERED_BOTTOM)
        {
            const auto parentPosCenter = win->rect().center();
            const auto parentPosBottom = win->rect().bottom();

            position = QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                              parentPosBottom - margins.y() - static_cast<int>(m_size[1]));
        }
        else if(m_position == Position::TOP_LEFT)
        {
            const auto parentPosTopLeft = win->rect().topLeft();

            position = QPoint((parentPosTopLeft.x() + margins.x()),
                              parentPosTopLeft.y() + margins.y());
        }
        else if(m_position == Position::TOP_RIGHT)
        {
            const auto parentPosTopRight = win->rect().topRight();

            position = QPoint((parentPosTopRight.x() - margins.x()) - static_cast<int>(m_size[0]),
                              parentPosTopRight.y() + margins.y());
        }
        else if(m_position == Position::BOTTOM_LEFT)
        {
            const auto parentPosBottomLeft = win->rect().bottomLeft();

            position = QPoint((parentPosBottomLeft.x() + margins.x()),
                              parentPosBottomLeft.y() - margins.y() - static_cast<int>(m_size[1]));
        }
        else if(m_position == Position::BOTTOM_RIGHT)
        {
            const auto parentPosBottomRight = win->rect().bottomRight();

            position = QPoint((parentPosBottomRight.x() - margins.x()) - static_cast<int>(m_size[0]),
                              parentPosBottomRight.y() - margins.y() - static_cast<int>(m_size[1]));
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
