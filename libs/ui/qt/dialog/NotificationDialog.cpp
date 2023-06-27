/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
#include "ui/qt/widget/SlideBar.hpp"

#include <ui/base/registry/macros.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

namespace sight::ui::qt::dialog
{

static constexpr auto s_SHOW_MORE = "...";

//------------------------------------------------------------------------------

NotificationDialog::NotificationDialog(ui::base::GuiBaseObject::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

void NotificationDialog::show()
{
    // Check if the widget is already created and has already been shown
    if(m_parent.isNull())
    {
        build();
    }
    else
    {
        update();
    }
}

//------------------------------------------------------------------------------

void NotificationDialog::build()
{
    /// Retrieve the parent widget
    SIGHT_ASSERT("The dialog UI has been already built", m_parent.isNull());

    if(const auto& parentContainer = ui::qt::container::QtContainer::dynamicCast(m_parentContainer); parentContainer)
    {
        m_parent = parentContainer->getQtContainer();
    }
    else
    {
        // Checks if we have a Parent widget.
        m_parent = qApp->activeWindow();

        if(const auto* slideBar = qobject_cast<widget::SlideBar*>(m_parent); slideBar != nullptr)
        {
            m_parent = slideBar->nativeParentWidget();
        }
    }

    if(m_parent == nullptr)
    {
        SIGHT_ERROR("Notification ignored, no Active Window are found (focus may be lost).");
        return;
    }

    // Fade in effect.
    auto* const effect = new QGraphicsOpacityEffect();
    auto* const a      = new QPropertyAnimation(effect, "opacity");
    a->setDuration(200); // In milliseconds
    a->setStartValue(0); // Full transparent.
    a->setEndValue(0.9); // 90% of opacity, to see through the popup.
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    // Creates the main translucent auto-movable container.
    m_container = new Container(this->computePosition(), m_parent);
    m_container->setGraphicsEffect(effect);
    m_container->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    m_container->setAttribute(Qt::WA_TranslucentBackground);
    m_container->setContentsMargins(0, 0, 0, 0);
    m_container->setMinimumSize(m_notification.size[0], m_notification.size[1]);
    m_container->setMaximumSize(m_notification.size[0], m_notification.size[1]);
    m_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // Creates container layout.
    auto* const containerLayout = new QBoxLayout(QBoxLayout::LeftToRight, m_container);
    containerLayout->setSpacing(0);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    m_container->setLayout(containerLayout);

    // Moves the container when the main window is moved or is resized.
    // Find the real "root" mainwindow
    auto* root_widget = m_parent.data();
    while(root_widget->parentWidget() != nullptr)
    {
        root_widget = root_widget->parentWidget();
    }

    // Instal event filters
    root_widget->installEventFilter(m_container);
    m_parent->installEventFilter(m_container);

    // Creates an intermediate layer so we can add optional "show more" button
    m_subContainer = new QWidget(m_container);
    containerLayout->addWidget(m_subContainer);
    m_subContainer->setAutoFillBackground(true);
    m_subContainer->setAttribute(Qt::WA_StyledBackground, true);

    auto* const subcontainerLayout = new QBoxLayout(QBoxLayout::LeftToRight, m_subContainer);
    subcontainerLayout->setSpacing(0);
    subcontainerLayout->setContentsMargins(0, 0, 0, 0);
    m_subContainer->setLayout(subcontainerLayout);

    // Creates the clickable label.
    m_msgBox = new ClickableQLabel(m_container, m_subContainer);
    m_msgBox->setWordWrap(true);
    m_msgBox->setScaledContents(true);
    m_msgBox->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    subcontainerLayout->addWidget(m_msgBox);

    // Execute the callback on close
    // Make an explicit copy so It will still be callable in the lambda when this has been destroyed.
    auto callable = m_closedCallBack;

    QObject::connect(
        m_msgBox,
        &ClickableQLabel::faded,
        [callable]
        {
            if(callable)
            {
                callable();
            }
        });

    // Build the show more button
    m_showMoreButton = new QToolButton(m_msgBox);
    m_showMoreButton->setContentsMargins(0, 0, 0, 0);
    m_showMoreButton->setMinimumSize(32, 24);
    m_showMoreButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_showMoreButton->setText(s_SHOW_MORE);
    m_showMoreButton->setStyleSheet("border-radius: 10px;border: 2px solid white;");
    subcontainerLayout->addWidget(m_showMoreButton);

    // Build the show more dialog (icon, text, title will be set in update())
    m_showMoreBox = new QMessageBox(
        QMessageBox::Information,
        QString(),
        QString(),
        QMessageBox::Ok,
        m_parent
    );

    // Show the show more dialog on button click
    QObject::connect(m_showMoreButton, &QToolButton::clicked, m_showMoreBox, &QMessageBox::exec);

    // Set text, icon, etc..
    update();

    // Displays it.
    m_container->show();
}

//------------------------------------------------------------------------------

inline static bool checkMessageLength(
    const QFontMetrics& metrics,
    const QRect& available,
    const QString& message,
    const bool is_truncated
)
{
    const auto& bounding = metrics.boundingRect(
        available,
        int(Qt::TextWordWrap) | Qt::AlignHCenter | Qt::AlignVCenter,
        message + (is_truncated ? "..." : "")
    );

    return available.width() >= bounding.width() && available.height() >= bounding.height();
}

//------------------------------------------------------------------------------

void NotificationDialog::update()
{
    // Check that the UI is built
    SIGHT_ASSERT("The dialog UI has not been built", !m_parent.isNull());

    // If we must reapply the style sheet or not
    const auto& old_object_name = m_subContainer->objectName();

    static constexpr auto success_name {"NotificationDialog_Success"};
    static constexpr auto failure_name {"NotificationDialog_Failure"};
    static constexpr auto info_name {"NotificationDialog_Info"};

    // Set object names
    switch(m_notification.type)
    {
        case INotificationDialog::Type::SUCCESS:
            m_subContainer->setObjectName(success_name);
            m_msgBox->setObjectName(success_name);
            break;

        case INotificationDialog::Type::FAILURE:
            m_subContainer->setObjectName(failure_name);
            m_msgBox->setObjectName(failure_name);
            break;

        default:
            m_subContainer->setObjectName(info_name);
            m_msgBox->setObjectName(info_name);
            break;
    }

    // If different, then we must (re)apply the style sheet
    if(old_object_name != m_subContainer->objectName())
    {
        if(const auto& global_stylesheet = qApp->styleSheet(); !global_stylesheet.isEmpty())
        {
            m_subContainer->setStyleSheet(global_stylesheet);
            m_msgBox->setStyleSheet(global_stylesheet);
        }
        else
        {
            static constexpr auto success_style {
                "background-color:#58D68D;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            };

            static constexpr auto failure_style {
                "background-color:#E74C3C;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            };

            static constexpr auto info_style {
                "background-color:#5DADE2;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            };

            switch(m_notification.type)
            {
                case INotificationDialog::Type::SUCCESS:
                    m_subContainer->setStyleSheet(success_style);
                    m_msgBox->setStyleSheet(success_style);
                    break;

                case INotificationDialog::Type::FAILURE:
                    m_subContainer->setStyleSheet(failure_style);
                    m_msgBox->setStyleSheet(failure_style);
                    break;

                default:
                    m_subContainer->setStyleSheet(info_style);
                    m_msgBox->setStyleSheet(info_style);
                    break;
            }
        }

        // Force to reapply style
        m_subContainer->adjustSize();
        m_subContainer->update();
    }

    const auto& [is_truncated, message] =
        [&]
        {
            SIGHT_ASSERT("Width is invalid", m_notification.size[0] > 0);
            SIGHT_ASSERT("Height is invalid", m_notification.size[1] > 0);

            const QRect available(
                0,
                0,
                int(m_notification.size[0] - 0.1 * m_notification.size[0]),
                int(m_notification.size[1] - 0.1 * m_notification.size[1])
            );

            const QFontMetrics metrics(m_msgBox->font());

            // Initial message
            auto truncated    = QString::fromStdString(m_notification.message);
            bool is_truncated = !checkMessageLength(metrics, available, truncated, false);

            if(is_truncated)
            {
                const auto& button_size = !m_showMoreButton.isNull() ? m_showMoreButton->sizeHint() : QSize(0, 0);
                const QRect new_available(
                    0,
                    0,
                    available.width() - int(button_size.width()),
                    available.height()
                );

                // Truncate
                for(int step = truncated.length() / 2 ; step > 2 ; step = step / 2)
                {
                    // Truncate roughly
                    if(!checkMessageLength(metrics, new_available, truncated, true))
                    {
                        truncated.truncate(truncated.length() - step);
                    }

                    // Refine
                    if(checkMessageLength(metrics, new_available, truncated, true))
                    {
                        const int previous = truncated.length() + step;
                        truncated = QString::fromStdString(m_notification.message);
                        truncated.truncate(previous);
                    }
                }

                // Last 1-2 steps
                while(!checkMessageLength(metrics, new_available, truncated, true))
                {
                    truncated.truncate(truncated.length() - 1);
                }
            }

            return std::make_pair(is_truncated, truncated);
        }();

    if(is_truncated)
    {
        const auto& truncated_message = message + s_SHOW_MORE;
        m_msgBox->setText(truncated_message);

        // Create a real message box with the full text
        auto icon = QMessageBox::NoIcon;
        if(m_notification.type == INotificationDialog::Type::FAILURE)
        {
            icon = QMessageBox::Critical;
        }
        else if(m_notification.type == INotificationDialog::Type::INFO)
        {
            icon = QMessageBox::Information;
        }

        m_showMoreBox->setIcon(icon);
        m_showMoreBox->setWindowTitle(truncated_message);
        m_showMoreBox->setText(QString::fromStdString(m_notification.message));
    }
    else
    {
        m_msgBox->setText(message);
    }

    // Only show the button for truncated text.
    m_showMoreButton->setVisible(is_truncated);

    // Reapply the position
    auto positionFct = this->computePosition();
    m_container->setPosition(positionFct, m_parent);
    m_container->setPositionFct(positionFct);

    // (re)Start / stop the fadeout timer
    m_msgBox->timedFadeout(m_notification.duration);

    // If the notification has a closable attribute, use it, otherwise, make it closable if not permanent
    if((m_notification.closable && *m_notification.closable)
       || (!m_notification.closable && (m_notification.duration && m_notification.duration->count() != 0)))
    {
        // Fadeout when clicked.
        QObject::connect(m_msgBox, &ClickableQLabel::clicked, m_msgBox, &ClickableQLabel::fadeout);
    }
    else
    {
        // Not closable
        QObject::disconnect(m_msgBox, &ClickableQLabel::clicked, m_msgBox, &ClickableQLabel::fadeout);
    }
}

//------------------------------------------------------------------------------

bool NotificationDialog::isVisible() const
{
    if(m_msgBox == nullptr)
    {
        return false;
    }

    return m_msgBox->isVisible();
}

//------------------------------------------------------------------------------

void NotificationDialog::close() const
{
    if(m_msgBox != nullptr)
    {
        // Closing after a fade out effect.
        m_msgBox->fadeout();
    }
}

//------------------------------------------------------------------------------

void NotificationDialog::moveDown()
{
    if(!m_container.isNull() && m_index > 0)
    {
        --m_index;

        // Use the Qt event loop to have a smoother animation, especially when moving many notifications at once, like
        // when closing permanent top level one.
        QTimer::singleShot(
            0,
            [&]
            {
                if(!m_container.isNull())
                {
                    m_container->setPosition(computePosition(), m_parent);
                }
            });
    }
}

//------------------------------------------------------------------------------

void NotificationDialog::setSize(std::array<int, 2> _size)
{
    INotificationDialog::setSize(_size);

    // Apply the size change
    if(!m_container.isNull())
    {
        m_container->setMinimumSize(m_notification.size[0], m_notification.size[1]);
        m_container->setMaximumSize(m_notification.size[0], m_notification.size[1]);
    }
}

//------------------------------------------------------------------------------

std::function<QPoint(QWidget*)> NotificationDialog::computePosition()
{
    constexpr int margin = 5;

    switch(m_notification.position)
    {
        case Position::CENTERED:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parentPosCenter = _parent->mapToGlobal(_parent->rect().center());

                       return {
                           parentPosCenter.x() - static_cast<int>(m_notification.size[0] / 2),
                           parentPosCenter.y() - static_cast<int>(m_notification.size[1] / 2)
                       };
                   };

        case Position::CENTERED_TOP:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
                       const int parentY = _parent->mapToGlobal(_parent->rect().topLeft()).y();
                       const int height  = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parentX - static_cast<int>(m_notification.size[0] / 2),
                           parentY + margin + (height * static_cast<int>(m_index))
                       };
                   };

        case Position::CENTERED_BOTTOM:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const int parentX = _parent->mapToGlobal(_parent->rect().center()).x();
                       const int parentY = _parent->mapToGlobal(_parent->rect().bottomLeft()).y();
                       const int height  = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parentX - static_cast<int>(m_notification.size[0] / 2),
                           parentY - margin - (height * (static_cast<int>(m_index) + 1))
                       };
                   };

        case Position::TOP_LEFT:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrentTopLeft = _parent->mapToGlobal(_parent->rect().topLeft());
                       const int parentX         = parrentTopLeft.x();
                       const int parentY         = parrentTopLeft.y();
                       const int height          = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parentX + margin,
                           parentY + margin + (height * static_cast<int>(m_index))
                       };
                   };

        case Position::TOP_RIGHT:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrentTopRight = _parent->mapToGlobal(_parent->rect().topRight());
                       const int parentX          = parrentTopRight.x();
                       const int parentY          = parrentTopRight.y();
                       const int height           = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parentX - margin - static_cast<int>(m_notification.size[0]),
                           parentY + margin + (height * static_cast<int>(m_index))
                       };
                   };

        case Position::BOTTOM_LEFT:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrentBottomLeft = _parent->mapToGlobal(_parent->rect().bottomLeft());
                       const int parentX            = parrentBottomLeft.x();
                       const int parentY            = parrentBottomLeft.y();
                       const int height             = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parentX + margin,
                           parentY - (height * (static_cast<int>(m_index) + 1))
                       };
                   };

        case Position::BOTTOM_RIGHT:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrentBottomRight = _parent->mapToGlobal(_parent->rect().bottomRight());
                       const int parentX             = parrentBottomRight.x();
                       const int parentY             = parrentBottomRight.y();
                       const int height              = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parentX - margin - static_cast<int>(m_notification.size[0]),
                           parentY - (height * (static_cast<int>(m_index) + 1))
                       };
                   };

        default:
            SIGHT_ASSERT("Position '" + std::to_string(int(m_notification.position)) + "' is unknown.", false);

            return [](QWidget* _parent) -> QPoint
                   {
                       return _parent->pos();
                   };
    }
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
