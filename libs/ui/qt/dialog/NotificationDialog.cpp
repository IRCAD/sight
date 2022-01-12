/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "ui/qt/dialog/NotificationDialog.hpp"

#include "ui/qt/container/QtContainer.hpp"

#include <ui/base/registry/macros.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QMessageBox>
#include <QTimer>

namespace sight::ui::qt
{

namespace dialog
{

//------------------------------------------------------------------------------

// Checks if bounding rectange of a message fit into _acceptable_size.
bool checkSizeOfMessage(const QString& _message, const QFontMetrics& _fm, const QSize _acceptable_size)
{
    const auto rect = _fm.boundingRect(
        QRect(0, 0, _acceptable_size.width(), _acceptable_size.height()),
        Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignVCenter,
        _message
    );

    if(rect.width() >= _acceptable_size.width() || rect.height() >= _acceptable_size.height())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

// Find where to trunc a message, to fit _acceptable_size. Calls checkSizeOfMessage.
size_t truncMessageToFit(const std::string& _message, const QFontMetrics& _fm, const QSize _acceptable_size)
{
    // if size is > 100, start trunc at 100 to be faster.
    const auto initial_size = _message.size() < 100 ? _message.size() : 100;

    bool fit = false;

    // initialize trunc_size to initial_size + 1
    size_t trunc_size = initial_size + 1;
    do
    {
        // first time == initial_size
        --trunc_size;
        // trunc _message
        const auto trunc_msg = _message.substr(0, trunc_size);
        // check if it fits.
        fit = checkSizeOfMessage(QString::fromStdString(trunc_msg), _fm, _acceptable_size);
    }
    while(!fit);

    return trunc_size;
}

//------------------------------------------------------------------------------

NotificationDialog::NotificationDialog(ui::base::GuiBaseObject::Key)
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
    m_parent = qApp->activeWindow();

    // If the activie window is a slide bar, we need to retrieve the native parent.
    if(m_parent && m_parent->objectName() == "SlideBar")
    {
        m_parent = m_parent->nativeParentWidget();
    }

    ui::qt::container::QtContainer::csptr parentContainer =
        ui::qt::container::QtContainer::dynamicCast(m_parentContainer);

    // Replaces the activeWindow by the parentContainer if exists.
    if(parentContainer)
    {
        m_parent = parentContainer->getQtContainer();
    }

    // If there is no parent here, we get the top one.
    if(!m_parent)
    {
        SIGHT_ERROR("Notification ignored, no Active Window are found(Focus may be lost).");
        return;
    }

    // Creates the clickable label.
    m_msgBox = new ClickableQLabel();
    m_msgBox->setWordWrap(true);
    m_msgBox->setScaledContents(true);
    m_msgBox->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_fullMessage = m_message;
    bool display_show_more = false;

    const int w = static_cast<int>(m_size[0]);
    const int h = static_cast<int>(m_size[1]);

    // Acceptable size is w, h with 10% margin removal.
    const QSize acceptable_size = {w - static_cast<int>(0.1 * w), h - static_cast<int>(0.1 * h)};

    const auto trunc_size = truncMessageToFit(m_fullMessage, QFontMetrics(m_msgBox->font()), acceptable_size);

    if(trunc_size != m_fullMessage.size())
    {
        display_show_more = true;
        const std::string read_more = "...";
        const std::string msg       = m_fullMessage.substr(0, trunc_size - read_more.size());
        m_message = msg + read_more;
    }

    m_msgBox->setText(QString::fromStdString(m_message));

    // If no styleSheet is used.
    if(qApp->styleSheet().isEmpty())
    {
        if(m_notificationType == INotificationDialog::Type::SUCCESS)
        {
            m_msgBox->setStyleSheet(
                "background-color:#58D68D;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            );
        }
        else if(m_notificationType == INotificationDialog::Type::FAILURE)
        {
            m_msgBox->setStyleSheet(
                "background-color:#E74C3C;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            );
        }
        else // INFO by default.
        {
            m_msgBox->setStyleSheet(
                "background-color:#5DADE2;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            );
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
    a->setDuration(200); // In milliseconds
    a->setStartValue(0); // Full transparent.
    a->setEndValue(0.9); // 90% of opacity, to see through the popup.
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    // Creates the getPosition function
    std::function<QPoint(QWidget*)> position;

    position = this->computePosition();

    // Creates the main translucent auto-movable container.
    m_msgContainer = new Container(position, m_parent);
    m_msgContainer->setGraphicsEffect(effect);
    m_msgContainer->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    m_msgContainer->setAttribute(Qt::WA_TranslucentBackground);
    m_msgContainer->setContentsMargins(0, 0, 0, 0);
    m_msgContainer->setMinimumSize(static_cast<int>(m_size[0]), static_cast<int>(m_size[1]));
    m_msgContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    // Moves the container when the main window is moved or is resized.
    m_parent->installEventFilter(m_msgContainer);

    // Gives it a layout with the clickable label.
    QBoxLayout* const layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_msgContainer->setLayout(layout);
    layout->addWidget(m_msgBox);

    if(display_show_more)
    {
        QMessageBox::Icon icon = QMessageBox::NoIcon;
        if(m_notificationType == INotificationDialog::Type::FAILURE)
        {
            icon = QMessageBox::Critical;
        }
        else if(m_notificationType == INotificationDialog::Type::INFO)
        {
            icon = QMessageBox::Information;
        }

        QMessageBox* full_message_box = new QMessageBox(
            icon,
            "Read more ...",
            QString::fromStdString(m_fullMessage),
            QMessageBox::Ok,
            qApp->activeWindow()
        );

        QObject::connect(m_msgBox, &ClickableQLabel::clicked, full_message_box, &QMessageBox::exec);
    }

    // Fadeout when  double clicked.
    QObject::connect(m_msgBox, &ClickableQLabel::doubleClicked, m_msgBox, &ClickableQLabel::fadeout);
    QObject::connect(
        m_msgBox,
        &ClickableQLabel::destroyed,
        [ = ]()
            {
                if(m_closedCallBack)
                {
                    m_closedCallBack();
                }
            });

    // Displays it.
    m_msgContainer->show();

    if(m_duration > 0)
    {
        // Launchs a timer and fadeout before closing.
        QTimer::singleShot(m_duration, m_msgBox, &ClickableQLabel::fadeout);
    }
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

void NotificationDialog::moveDown()
{
    if(m_msgContainer && m_index > 0)
    {
        --m_index;
        auto position = this->computePosition();
        m_msgContainer->setPosition(position, m_parent);
    }
}

//------------------------------------------------------------------------------

std::function<QPoint(QWidget*)> NotificationDialog::computePosition()
{
    std::function<QPoint(QWidget*)> position;
    const int margin = 5;
    if(m_position == Position::CENTERED)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parentPosCenter = _parent->mapToGlobal(_parent->rect().center());

                       return QPoint(
                           parentPosCenter.x() - static_cast<int>(m_size[0] / 2),
                           parentPosCenter.y() - static_cast<int>(m_size[1] / 2)
                       );
                   };
    }
    else if(m_position == Position::CENTERED_TOP)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
                       const int parentY = _parent->mapToGlobal(_parent->rect().topLeft()).y();
                       const int height  = static_cast<int>(m_size[1]) + margin;

                       return QPoint(
                           parentX - static_cast<int>(m_size[0] / 2),
                           parentY + margin + (height * static_cast<int>(m_index))
                       );
                   };
    }
    else if(m_position == Position::CENTERED_BOTTOM)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
                       const int parentY = _parent->mapToGlobal(_parent->rect().bottomLeft()).y();
                       const int height  = static_cast<int>(m_size[1]) + margin;

                       return QPoint(
                           parentX - static_cast<int>(m_size[0] / 2),
                           parentY - margin - (height * (static_cast<int>(m_index) + 1))
                       );
                   };
    }
    else if(m_position == Position::TOP_LEFT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentTopLeft = _parent->mapToGlobal(_parent->rect().topLeft());
                       const int parentX         = parrentTopLeft.x();
                       const int parentY         = parrentTopLeft.y();
                       const int height          = static_cast<int>(m_size[1]) + margin;

                       return QPoint(
                           parentX + margin,
                           parentY + margin + (height * static_cast<int>(m_index))
                       );
                   };
    }
    else if(m_position == Position::TOP_RIGHT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentTopRight = _parent->mapToGlobal(_parent->rect().topRight());
                       const int parentX          = parrentTopRight.x();
                       const int parentY          = parrentTopRight.y();
                       const int height           = static_cast<int>(m_size[1]) + margin;

                       return QPoint(
                           parentX - margin - static_cast<int>(m_size[0]),
                           parentY + margin + (height * static_cast<int>(m_index))
                       );
                   };
    }
    else if(m_position == Position::BOTTOM_LEFT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentBottomLeft = _parent->mapToGlobal(_parent->rect().bottomLeft());
                       const int parentX            = parrentBottomLeft.x();
                       const int parentY            = parrentBottomLeft.y();
                       const int height             = static_cast<int>(m_size[1]) + margin;

                       return QPoint(
                           parentX + margin,
                           parentY - (height * (static_cast<int>(m_index) + 1))
                       );
                   };
    }
    else if(m_position == Position::BOTTOM_RIGHT)
    {
        position = [ = ](QWidget* _parent) -> QPoint
                   {
                       const auto parrentBottomRight = _parent->mapToGlobal(_parent->rect().bottomRight());
                       const int parentX             = parrentBottomRight.x();
                       const int parentY             = parrentBottomRight.y();
                       const int height              = static_cast<int>(m_size[1]) + margin;

                       return QPoint(
                           parentX - margin - static_cast<int>(m_size[0]),
                           parentY - (height * (static_cast<int>(m_index) + 1))
                       );
                   };
    }

    return position;
}

//------------------------------------------------------------------------------

} // namespace dialog.

} // namespace sight::ui::qt.
