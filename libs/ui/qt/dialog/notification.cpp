/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

#include "ui/qt/dialog/notification.hpp"

#include "ui/qt/container/widget.hpp"
#include "ui/qt/widget/slide_bar.hpp"

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

namespace sight::ui::qt::dialog
{

static constexpr auto SHOW_MORE = "...";

//------------------------------------------------------------------------------

void notification::show()
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

void notification::build()
{
    /// Retrieve the parent widget
    SIGHT_ASSERT("The dialog UI has been already built", m_parent.isNull());

    if(const auto& parent_container = std::dynamic_pointer_cast<const ui::qt::container::widget>(m_parent_container);
       parent_container)
    {
        m_parent = parent_container->get_qt_container();
    }
    else
    {
        // Checks if we have a Parent widget.
        m_parent = qApp->activeWindow();

        if(m_parent == nullptr)
        {
            // When the Parent widget is inactive
            if(auto widgets = qApp->allWidgets(); !widgets.empty())
            {
                m_parent = widgets.first()->topLevelWidget();
            }
        }

        if(const auto* slide_bar = qobject_cast<ui::qt::widget::slide_bar*>(m_parent); slide_bar != nullptr)
        {
            m_parent = slide_bar->nativeParentWidget();
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
    m_widget = new widget(this->compute_position(), m_parent);
    m_widget->setGraphicsEffect(effect);
    m_widget->setContentsMargins(0, 0, 0, 0);
    m_widget->setMinimumSize(m_notification.size[0], m_notification.size[1]);
    m_widget->setMaximumSize(m_notification.size[0], m_notification.size[1]);
    m_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // Creates container layout.
    auto* const container_layout = new QBoxLayout(QBoxLayout::LeftToRight, m_widget);
    container_layout->setSpacing(0);
    container_layout->setContentsMargins(0, 0, 0, 0);
    m_widget->setLayout(container_layout);

    // Moves the container when the main window is moved or is resized.
    // Find the real "root" mainwindow
    auto* root_widget = m_parent.data();
    while(root_widget->parentWidget() != nullptr)
    {
        root_widget = root_widget->parentWidget();
    }

    // Instal event filters
    root_widget->installEventFilter(m_widget);
    m_parent->installEventFilter(m_widget);

    // Creates an intermediate layer so we can add optional "show more" button
    m_sub_widget = new QWidget(m_widget);
    container_layout->addWidget(m_sub_widget);
    m_sub_widget->setAutoFillBackground(true);
    m_sub_widget->setAttribute(Qt::WA_StyledBackground, true);

    auto* const subcontainer_layout = new QBoxLayout(QBoxLayout::LeftToRight, m_sub_widget);
    subcontainer_layout->setSpacing(0);
    subcontainer_layout->setContentsMargins(0, 0, 0, 0);
    m_sub_widget->setLayout(subcontainer_layout);

    // Creates the clickable label.
    m_msg_box = new clickable_q_label(m_widget, m_sub_widget);
    m_msg_box->setWordWrap(true);
    m_msg_box->setScaledContents(true);
    m_msg_box->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    subcontainer_layout->addWidget(m_msg_box);

    // Execute the callback on close
    // Make an explicit copy so It will still be callable in the lambda when this has been destroyed.
    auto callable = m_closed_call_back;

    QObject::connect(
        m_msg_box,
        &clickable_q_label::faded,
        [callable]
        {
            if(callable)
            {
                callable();
            }
        });

    // Build the show more button
    m_show_more_button = new QToolButton(m_msg_box);
    m_show_more_button->setContentsMargins(0, 0, 0, 0);
    m_show_more_button->setMinimumSize(32, 24);
    m_show_more_button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_show_more_button->setText(SHOW_MORE);
    m_show_more_button->setStyleSheet("border-radius: 10px;border: 2px solid white;");
    subcontainer_layout->addWidget(m_show_more_button);

    // Build the show more dialog (icon, text, title will be set in update())
    m_show_more_box = new QMessageBox(
        QMessageBox::Information,
        QString(),
        QString(),
        QMessageBox::Ok,
        m_parent
    );

    // Show the show more dialog on button click
    QObject::connect(m_show_more_button, &QToolButton::clicked, m_show_more_box, &QMessageBox::exec);

    // Set text, icon, etc..
    update();

    // Displays it.
    m_widget->show();
}

//------------------------------------------------------------------------------

inline static bool check_message_length(
    const QFontMetrics& _metrics,
    const QRect& _available,
    const QString& _message,
    const bool _is_truncated
)
{
    const auto& bounding = _metrics.boundingRect(
        _available,
        int(Qt::TextWordWrap) | Qt::AlignHCenter | Qt::AlignVCenter,
        _message + (_is_truncated ? "..." : "")
    );

    return _available.width() >= bounding.width() && _available.height() >= bounding.height();
}

//------------------------------------------------------------------------------

void notification::update()
{
    // Check that the UI is built
    SIGHT_ASSERT("The dialog UI has not been built", !m_parent.isNull());

    // If we must reapply the style sheet or not
    const auto& old_object_name = m_sub_widget->objectName();

    static constexpr auto s_SUCCESS_NAME {"NotificationDialog_Success"};
    static constexpr auto s_FAILURE_NAME {"NotificationDialog_Failure"};
    static constexpr auto s_INFO_NAME {"NotificationDialog_Info"};

    // Set object names
    switch(m_notification.type)
    {
        case notification_base::type::success:
            m_sub_widget->setObjectName(s_SUCCESS_NAME);
            m_msg_box->setObjectName(s_SUCCESS_NAME);
            break;

        case notification_base::type::failure:
            m_sub_widget->setObjectName(s_FAILURE_NAME);
            m_msg_box->setObjectName(s_FAILURE_NAME);
            break;

        default:
            m_sub_widget->setObjectName(s_INFO_NAME);
            m_msg_box->setObjectName(s_INFO_NAME);
            break;
    }

    // If different, then we must (re)apply the style sheet
    if(old_object_name != m_sub_widget->objectName())
    {
        if(const auto& global_stylesheet = qApp->styleSheet(); !global_stylesheet.isEmpty())
        {
            m_sub_widget->setStyleSheet(global_stylesheet);
            m_msg_box->setStyleSheet(global_stylesheet);
        }
        else
        {
            static constexpr auto s_SUCCESS_STYLE {
                "background-color:#58D68D;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            };

            static constexpr auto s_FAILURE_STYLE {
                "background-color:#E74C3C;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            };

            static constexpr auto s_INFO_STYLE {
                "background-color:#5DADE2;color:white;font-weight: bold;font-size: 16px;border-radius: 10px"
            };

            switch(m_notification.type)
            {
                case notification_base::type::success:
                    m_sub_widget->setStyleSheet(s_SUCCESS_STYLE);
                    m_msg_box->setStyleSheet(s_SUCCESS_STYLE);
                    break;

                case notification_base::type::failure:
                    m_sub_widget->setStyleSheet(s_FAILURE_STYLE);
                    m_msg_box->setStyleSheet(s_FAILURE_STYLE);
                    break;

                default:
                    m_sub_widget->setStyleSheet(s_INFO_STYLE);
                    m_msg_box->setStyleSheet(s_INFO_STYLE);
                    break;
            }
        }

        // Force to reapply style
        m_sub_widget->adjustSize();
        m_sub_widget->update();
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

            const QFontMetrics metrics(m_msg_box->font());

            // Initial message
            auto truncated    = QString::fromStdString(m_notification.message);
            bool is_truncated = !check_message_length(metrics, available, truncated, false);

            if(is_truncated)
            {
                const auto& button_size = !m_show_more_button.isNull() ? m_show_more_button->sizeHint() : QSize(0, 0);
                const QRect new_available(
                    0,
                    0,
                    available.width() - button_size.width(),
                    available.height()
                );

                // Truncate
                for(auto step = truncated.length() / 2 ; step > 2 ; step = step / 2)
                {
                    // Truncate roughly
                    if(!check_message_length(metrics, new_available, truncated, true))
                    {
                        truncated.truncate(truncated.length() - step);
                    }

                    // Refine
                    if(check_message_length(metrics, new_available, truncated, true))
                    {
                        const auto previous = truncated.length() + step;
                        truncated = QString::fromStdString(m_notification.message);
                        truncated.truncate(previous);
                    }
                }

                // Last 1-2 steps
                while(!check_message_length(metrics, new_available, truncated, true))
                {
                    truncated.truncate(truncated.length() - 1);
                }
            }

            return std::make_pair(is_truncated, truncated);
        }();

    if(is_truncated)
    {
        const auto& truncated_message = message + SHOW_MORE;
        m_msg_box->setText(truncated_message);

        // Create a real message box with the full text
        auto icon = QMessageBox::NoIcon;
        if(m_notification.type == notification_base::type::failure)
        {
            icon = QMessageBox::Critical;
        }
        else if(m_notification.type == notification_base::type::info)
        {
            icon = QMessageBox::Information;
        }

        m_show_more_box->setIcon(icon);
        m_show_more_box->setWindowTitle(truncated_message);
        m_show_more_box->setText(QString::fromStdString(m_notification.message));
    }
    else
    {
        m_msg_box->setText(message);
    }

    // Only show the button for truncated text.
    m_show_more_button->setVisible(is_truncated);

    // Reapply the position
    auto position_fct = this->compute_position();
    m_widget->set_position(position_fct, m_parent);
    m_widget->set_position_fct(position_fct);

    // (re)Start / stop the fadeout timer
    m_msg_box->timed_fadeout(m_notification.duration);

    // If the notification has a closable attribute, use it, otherwise, make it closable if not permanent
    if((m_notification.closable && *m_notification.closable)
       || (!m_notification.closable && (m_notification.duration && m_notification.duration->count() != 0)))
    {
        // Fadeout when clicked.
        QObject::connect(m_msg_box, &clickable_q_label::clicked, m_msg_box, &clickable_q_label::fadeout);
    }
    else
    {
        // Not closable
        QObject::disconnect(m_msg_box, &clickable_q_label::clicked, m_msg_box, &clickable_q_label::fadeout);
    }
}

//------------------------------------------------------------------------------

bool notification::is_visible() const
{
    if(m_msg_box == nullptr)
    {
        return false;
    }

    return m_msg_box->isVisible();
}

//------------------------------------------------------------------------------

void notification::close() const
{
    if(m_msg_box != nullptr)
    {
        // Closing after a fade out effect.
        m_msg_box->fadeout();
    }
}

//------------------------------------------------------------------------------

void notification::move_down()
{
    if(!m_widget.isNull() && m_index > 0)
    {
        --m_index;

        // Use the Qt event loop to have a smoother animation, especially when moving many notifications at once, like
        // when closing permanent top level one.
        QTimer::singleShot(
            0,
            [&]
            {
                if(!m_widget.isNull())
                {
                    m_widget->set_position(compute_position(), m_parent);
                }
            });
    }
}

//------------------------------------------------------------------------------

void notification::set_size(std::array<int, 2> _size)
{
    notification_base::set_size(_size);

    // Apply the size change
    if(!m_widget.isNull())
    {
        m_widget->setMinimumSize(m_notification.size[0], m_notification.size[1]);
        m_widget->setMaximumSize(m_notification.size[0], m_notification.size[1]);
    }
}

//------------------------------------------------------------------------------

std::function<QPoint(QWidget*)> notification::compute_position()
{
    constexpr int margin = 5;

    switch(m_notification.position)
    {
        case position::centered:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parent_pos_center = _parent->rect().center();

                       return {
                           parent_pos_center.x() - static_cast<int>(m_notification.size[0] / 2),
                           parent_pos_center.y() - static_cast<int>(m_notification.size[1] / 2)
                       };
                   };

        case position::centered_top:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const int parent_x = _parent->rect().center().x();
                       const int parent_y = _parent->rect().topLeft().y();
                       const int height   = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parent_x - static_cast<int>(m_notification.size[0] / 2),
                           parent_y + margin + (height * static_cast<int>(m_index))
                       };
                   };

        case position::centered_bottom:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const int parent_x = _parent->rect().center().x();
                       const int parent_y = _parent->rect().bottomLeft().y();
                       const int height   = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parent_x - static_cast<int>(m_notification.size[0] / 2),
                           parent_y - margin - (height * (static_cast<int>(m_index) + 1))
                       };
                   };

        case position::top_left:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrent_top_left = _parent->rect().topLeft();
                       const int parent_x          = parrent_top_left.x();
                       const int parent_y          = parrent_top_left.y();
                       const int height            = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parent_x + margin,
                           parent_y + margin + (height * static_cast<int>(m_index))
                       };
                   };

        case position::top_right:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrent_top_right = _parent->rect().topRight();
                       const int parent_x           = parrent_top_right.x();
                       const int parent_y           = parrent_top_right.y();
                       const int height             = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parent_x - margin - static_cast<int>(m_notification.size[0]),
                           parent_y + margin + (height * static_cast<int>(m_index))
                       };
                   };

        case position::bottom_left:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrent_bottom_left = _parent->rect().bottomLeft();
                       const int parent_x             = parrent_bottom_left.x();
                       const int parent_y             = parrent_bottom_left.y();
                       const int height               = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parent_x + margin,
                           parent_y - (height * (static_cast<int>(m_index) + 1))
                       };
                   };

        case position::bottom_right:
            return [this](QWidget* _parent) -> QPoint
                   {
                       const auto parrent_bottom_right = _parent->rect().bottomRight();
                       const int parent_x              = parrent_bottom_right.x();
                       const int parent_y              = parrent_bottom_right.y();
                       const int height                = static_cast<int>(m_notification.size[1]) + margin;

                       return {
                           parent_x - margin - static_cast<int>(m_notification.size[0]),
                           parent_y - (height * (static_cast<int>(m_index) + 1))
                       };
                   };

        default:
            SIGHT_ASSERT("position '" + std::to_string(int(m_notification.position)) + "' is unknown.", false);

            return [](QWidget* _parent) -> QPoint
                   {
                       return _parent->pos();
                   };
    }
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
