/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#pragma once

#include <sight/ui/qt/config.hpp>

#include <core/macros.hpp>

#include <ui/__/dialog/notification_base.hpp>

#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPointer>
#include <QPropertyAnimation>
#include <QTimerEvent>
#include <QToolButton>

namespace sight::ui::qt::dialog
{

/// Creates an auto-movable container.
class SIGHT_UI_QT_CLASS_API_QT widget : public QWidget
{
Q_OBJECT

public:

    /**
     * @brief Creates a Container.
     * @param _position a function that allows this widget to compute its own position from its parent.
     * @param _parent the parent of this widget.
     */
    explicit widget(std::function<QPoint(QWidget*)> _position, QWidget* _parent) :
        QWidget(_parent),
        m_position(_position)
    {
        this->move(m_position(this->parentWidget()));
    }

    /// Destroys the Container.
    ~widget() override = default;

    //------------------------------------------------------------------------------

    void set_position(std::function<QPoint(QWidget*)> _position, QWidget* _parent)
    {
        auto* a = new QPropertyAnimation(this, "pos");
        a->setDuration(500);
        a->setEndValue(_position(_parent));
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
    }

    //------------------------------------------------------------------------------

    void set_position_fct(std::function<QPoint(QWidget*)> _position)
    {
        m_position = _position;
    }

    /**
     * @brief Moves the widget according to the parent movement.
     * @param _object updated widget.
     * @param _event event type, only care about QEvent::Move and QEvent::Resize
     * @return
     */
    bool eventFilter(QObject* _object, QEvent* _event) override
    {
        if(const auto* const widget = qobject_cast<QWidget*>(_object); widget != nullptr)
        {
            const bool is_root = widget->parentWidget() == nullptr;

            if((!is_root && _event->type() == QEvent::Resize)
               || (is_root && (_event->type() == QEvent::Move
                               || _event->type() == QEvent::Resize
                               || _event->type() == QEvent::WindowActivate)))
            {
                this->move(m_position(this->parentWidget()));
            }
        }

        return false;
    }

private:

    /// Defines a function that computes a position relatively to a parent widget.
    std::function<QPoint(QWidget*)> m_position;
};

/// Creates a clickable QLabel.
class SIGHT_UI_QT_CLASS_API_QT clickable_q_label : public QLabel
{
Q_OBJECT

public:

    /// Creates a clickable QLabel.
    explicit clickable_q_label(widget* _root, QWidget* _parent = nullptr, Qt::WindowFlags _f = Qt::WindowFlags()) :
        QLabel(_parent, _f),
        m_root(_root)
    {
    }

    /// Destroys the clickable QLabel.
    ~clickable_q_label() override
    {
        if(m_timer_id != 0)
        {
            killTimer(m_timer_id);
        }
    }

public Q_SLOTS:

    /// Creates a fade out effect then closes the parent widget (Must be the Container).
    void fadeout()
    {
        if(m_timer_id != 0)
        {
            killTimer(m_timer_id);
            m_timer_id = 0;
        }

        QWidget* const parent_to_kill = !m_root.isNull() ? m_root : this->parentWidget() ? this->parentWidget() : this;

        auto* const effect = new QGraphicsOpacityEffect();
        auto* a            = new QPropertyAnimation(effect, "opacity");
        a->setDuration(500);
        a->setStartValue(0.9);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        parent_to_kill->setGraphicsEffect(effect);

        // When the animation is finished, we kill the parent since it must be the Container (the main widget).
        QObject::connect(a, &QPropertyAnimation::finished, this, &clickable_q_label::faded);
        QObject::connect(a, &QPropertyAnimation::finished, parent_to_kill, &QWidget::deleteLater);
    }

    //------------------------------------------------------------------------------

    void timed_fadeout(std::optional<std::chrono::milliseconds> _duration)
    {
        // Kill the timer if already started or null duration
        if(m_timer_id != 0 || !_duration || _duration->count() == 0)
        {
            killTimer(m_timer_id);
        }

        if(_duration && _duration->count() > 0)
        {
            m_timer_id = startTimer(*_duration);
        }
    }

Q_SIGNALS:

    /// Clicked signal, emitted when a mouse press event (simple click) occurs.
    void clicked();

    /// Double clicked signal, emitted when a mouse double click event occurs.
    void double_clicked();

    /// Emited when the widget will be closed
    void faded();

protected:

    //------------------------------------------------------------------------------

    void timerEvent(QTimerEvent* _event) override
    {
        if(_event->timerId() == m_timer_id)
        {
            fadeout();
        }
    }

    /**
     * @brief Handles event related to the mouse buttons.
     *
     * Send the signal @ref clicked().
     */
    void mousePressEvent(QMouseEvent* /*ev*/) override
    {
        Q_EMIT clicked();
    }

    /**
     * @brief Handles event related to the mouse buttons.
     *
     * Send the signal @ref doubleClicked().
     */
    void mouseDoubleClickEvent(QMouseEvent* /*event*/) override
    {
        Q_EMIT double_clicked();
    }

private:

    int m_timer_id {0};

    /// Contains the root widget which will be destroyed on close
    const QPointer<widget> m_root;
};

/**
 * @brief Defines a notification popup.
 *
 * Example of how to customize the style of the popups (You need to refers to their names):
 *
 * @code{.qss}
 #NotificationDialog_Success
    {
        background-color:#58D68D;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }

 #NotificationDialog_Failure
    {
        background-color:#E74C3C;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }

 #NotificationDialog_Info
    {
        background-color:#5DADE2;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }
   @endcode
 *
 */
class SIGHT_UI_QT_CLASS_API notification : public ui::dialog::notification_base
{
public:

    SIGHT_DECLARE_CLASS(notification, ui::dialog::notification_base, ui::factory::make<notification>)

    /// Destroys the dialog.
    SIGHT_UI_QT_API ~notification() override = default;

    /// Shows the notification relative to the active window.
    SIGHT_UI_QT_API void show() override;

    /**
     * @brief Returns whether the popup is displayed or not.
     * @return boolean (true is visible, false otherwise).
     */
    SIGHT_UI_QT_API bool is_visible() const override;

    /// Closes the popup (use a fadeout effect).
    SIGHT_UI_QT_API void close() const override;

    /// Move the notification to the lower index
    SIGHT_UI_QT_API void move_down() override;

    /// Resize the dialog
    SIGHT_UI_QT_API void set_size(std::array<int, 2> _size) override;

private:

    std::function<QPoint(QWidget*)> compute_position();

    /// Build the UI
    void build();

    /// Update the UI
    void update();

    /// Pointer to the Popup QLabel.
    QPointer<clickable_q_label> m_msg_box {nullptr};
    QPointer<widget> m_widget {nullptr};
    QPointer<QWidget> m_sub_widget {nullptr};
    QPointer<QWidget> m_parent {nullptr};
    QPointer<QMessageBox> m_show_more_box {nullptr};
    QPointer<QToolButton> m_show_more_button {nullptr};
};

} // namespace sight::ui::qt.
