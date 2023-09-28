/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qt/config.hpp"

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

namespace sight::ui::qt
{

/// Creates an auto-movable container.
class UI_QT_CLASS_API_QT Container : public QWidget
{
Q_OBJECT

public:

    /**
     * @brief Creates a Container.
     * @param _position a function that allows this container to compute its own position from its parent.
     * @param _parent the parent of this container.
     */
    explicit Container(std::function<QPoint(QWidget*)> _position, QWidget* _parent) :
        QWidget(_parent),
        m_position(_position)
    {
        this->move(m_position(this->parentWidget()));
    }

    /// Destroys the Container.
    ~Container() override = default;

    //------------------------------------------------------------------------------

    void setPosition(std::function<QPoint(QWidget*)> _position, QWidget* _parent)
    {
        auto* a = new QPropertyAnimation(this, "pos");
        a->setDuration(500);
        a->setEndValue(_position(_parent));
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
    }

    //------------------------------------------------------------------------------

    void setPositionFct(std::function<QPoint(QWidget*)> _position)
    {
        m_position = _position;
    }

    /**
     * @brief Moves the container according to the parent movement.
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
class UI_QT_CLASS_API_QT ClickableQLabel : public QLabel
{
Q_OBJECT

public:

    /// Creates a clickable QLabel.
    explicit ClickableQLabel(Container* root, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) :
        QLabel(parent, f),
        m_root(root)
    {
    }

    /// Destroys the clickable QLabel.
    ~ClickableQLabel() override
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
        QObject::connect(a, &QPropertyAnimation::finished, this, &ClickableQLabel::faded);
        QObject::connect(a, &QPropertyAnimation::finished, parent_to_kill, &QWidget::deleteLater);
    }

    //------------------------------------------------------------------------------

    void timedFadeout(std::optional<std::chrono::milliseconds> duration)
    {
        // Kill the timer if already started or null duration
        if(m_timer_id != 0 || !duration || duration->count() == 0)
        {
            killTimer(m_timer_id);
        }

        if(duration && duration->count() > 0)
        {
            m_timer_id = startTimer(*duration);
        }
    }

Q_SIGNALS:

    /// Clicked signal, emitted when a mouse press event (simple click) occurs.
    void clicked();

    /// Double clicked signal, emitted when a mouse double click event occurs.
    void doubleClicked();

    /// Emited when the widget will be closed
    void faded();

protected:

    //------------------------------------------------------------------------------

    void timerEvent(QTimerEvent* event) override
    {
        if(event->timerId() == m_timer_id)
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
        Q_EMIT doubleClicked();
    }

private:

    int m_timer_id {0};

    /// Contains the root container which will be destroyed on close
    const QPointer<Container> m_root;
};

namespace dialog
{

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
class UI_QT_CLASS_API notification : public ui::dialog::notification_base
{
public:

    SIGHT_DECLARE_CLASS(notification, ui::dialog::notification_base, ui::factory::make<notification>)

    /// Destroys the dialog.
    UI_QT_API ~notification() override = default;

    /// Shows the notification relative to the active window.
    UI_QT_API void show() override;

    /**
     * @brief Returns whether the popup is displayed or not.
     * @return boolean (true is visible, false otherwise).
     */
    UI_QT_API bool isVisible() const override;

    /// Closes the popup (use a fadeout effect).
    UI_QT_API void close() const override;

    /// Move the notification to the lower index
    UI_QT_API void moveDown() override;

    /// Resize the dialog
    UI_QT_API void setSize(std::array<int, 2> _size) override;

private:

    std::function<QPoint(QWidget*)> computePosition();

    /// Build the UI
    void build();

    /// Update the UI
    void update();

    /// Pointer to the Popup QLabel.
    QPointer<ClickableQLabel> m_msgBox {nullptr};
    QPointer<Container> m_container {nullptr};
    QPointer<QWidget> m_subContainer {nullptr};
    QPointer<QWidget> m_parent {nullptr};
    QPointer<QMessageBox> m_showMoreBox {nullptr};
    QPointer<QToolButton> m_showMoreButton {nullptr};
};

} // namespace dialog.

} // namespace sight::ui::qt.
