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
    // Checks if we have a Parent widget.
    QWidget* parent = qApp->activeWindow();

    ::fwGuiQt::container::QtContainer::csptr parentContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(m_parentContainer);

    // Replaces the activeWindow by the parentContainer if exists.
    if(parentContainer)
    {
        parent = parentContainer->getQtContainer();
    }

    // Creates the clikable label.
    m_msgBox = new ClickableQLabel();
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
    a->setDuration(200);  // In milliseconds
    a->setStartValue(0); // Full transparent.
    a->setEndValue(0.9); // 90% of opacity, to see through the popup.
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    // Creates the getPosition function
    std::function< QPoint(QWidget*) > position;
    const int margin = 5;

    if(m_position == Position::CENTERED )
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parentPosCenter = _parent->mapToGlobal(_parent->rect().center());

                       return QPoint(parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                                     parentPosCenter.y() - static_cast<int>(m_size[1] / 2));
                   };

    }
    else if (m_position == Position::CENTERED_TOP)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
                       const int parentY = _parent->mapToGlobal(_parent->rect().topLeft()).y();
                       const int height  = static_cast<int>(m_size[0]/2) + margin;

                       return QPoint(parentX - static_cast<int>(m_size[0] / 2),
                                     parentY + margin + (height * m_index));
                   };

    }
    else if (m_position == Position::CENTERED_BOTTOM)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
                       const int parentY = _parent->mapToGlobal(_parent->rect().bottomLeft()).y();
                       const int height  = static_cast<int>(m_size[0]/2) + margin;

                       return QPoint(parentX - static_cast<int>(m_size[0] / 2),
                                     parentY - margin - (height * (m_index+1)));
                   };
    }
    else if(m_position == Position::TOP_LEFT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentTopLeft = _parent->mapToGlobal(_parent->rect().topLeft());
                       const int parentX         = parrentTopLeft.x();
                       const int parentY         = parrentTopLeft.y();
                       const int height          = static_cast<int>(m_size[0]/2) + margin;

                       return QPoint(parentX + margin,
                                     parentY + margin + (height * m_index));
                   };
    }
    else if(m_position == Position::TOP_RIGHT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentTopRight = _parent->mapToGlobal(_parent->rect().topRight());
                       const int parentX          = parrentTopRight.x();
                       const int parentY          = parrentTopRight.y();
                       const int height           = static_cast<int>(m_size[0]/2) + margin;

                       return QPoint(parentX - margin - static_cast<int>(m_size[0]),
                                     parentY + margin + (height * m_index));
                   };
    }
    else if(m_position == Position::BOTTOM_LEFT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentBottomLeft = _parent->mapToGlobal(_parent->rect().bottomLeft());
                       const int parentX            = parrentBottomLeft.x();
                       const int parentY            = parrentBottomLeft.y();
                       const int height             = static_cast<int>(m_size[0]/2) + margin;

                       return QPoint(parentX + margin,
                                     parentY - (height * (m_index+1)));
                   };
    }
    else if(m_position == Position::BOTTOM_RIGHT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentBottomRight = _parent->mapToGlobal(_parent->rect().bottomRight());
                       const int parentX             = parrentBottomRight.x();
                       const int parentY             = parrentBottomRight.y();
                       const int height              = static_cast<int>(m_size[0]/2) + margin;

                       return QPoint(parentX - margin - static_cast<int>(m_size[0]),
                                     parentY - (height * (m_index+1)));
                   };
    }

    // Creates the main translucent auto-movable container.
    Container* const container = new Container(position, parent);
    container->setGraphicsEffect(effect);
    container->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint| Qt::NoDropShadowWindowHint);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setContentsMargins(0, 0, 0, 0);
    container->setMinimumSize(static_cast<int>(m_size[0]), static_cast<int>(m_size[1]));
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    // Moves the container when the main window is moved or is resized.
    qApp->activeWindow()->installEventFilter(container);

    // Gives it a layout with the clickable label.
    QBoxLayout* const layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(layout);
    layout->addWidget(m_msgBox);

    // Fadeout when clicked.
    QObject::connect(m_msgBox, &ClickableQLabel::clicked, m_msgBox, &ClickableQLabel::fadeout);

    // Displays it.
    container->show();

    // Launchs a timer and fadeout before closing.
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

} // namespace dialog
} // namespace fwGuiQt
