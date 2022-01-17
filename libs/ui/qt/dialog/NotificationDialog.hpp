/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#pragma once

#include "ui/qt/config.hpp"

#include <core/macros.hpp>

#include <ui/base/dialog/INotificationDialog.hpp>

#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QMouseEvent>
#include <QPointer>
#include <QPropertyAnimation>

#include <string>

namespace sight::ui::qt
{

/// Creates an auto-movable container.
class UI_QT_CLASS_API Container : public QWidget
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
    virtual ~Container()
    {
    }

    /**
     * @brief Moves the container according to the parent movement.
     * @param _object updated widget.
     * @param _event event type, only care about QEvent::Move and QEvent::Resize
     * @return
     */
    bool eventFilter(QObject* _object, QEvent* _event) override
    {
        if(_event->type() == QEvent::Move || _event->type() == QEvent::Resize)
        {
            this->move(m_position(this->parentWidget()));
        }

        return QWidget::eventFilter(_object, _event);
    }

private:

    /// Defines a function that computes a position relatively to a parent widget.
    std::function<QPoint(QWidget*)> m_position;
};

/// Creates a clickable QLabel.
class UI_QT_CLASS_API ClickableQLabel : public QLabel
{
Q_OBJECT

public:

    /// Creates a clickable QLabel.
    explicit ClickableQLabel() :
        QLabel(nullptr)
    {
    }

    /// Destroys the clickable QLabel.
    virtual ~ClickableQLabel()
    {
    }

public Q_SLOTS:

    /// Creates a fade out effect then closes the parent widget (Must be the Container).
    void fadeout()
    {
        QGraphicsOpacityEffect* const effect = new QGraphicsOpacityEffect();
        this->setGraphicsEffect(effect);
        QPropertyAnimation* a = new QPropertyAnimation(effect, "opacity");
        a->setDuration(500);
        a->setStartValue(0.9);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        // When the animation is finished, we kill the parent since it must be the Container (the main widget).
        QObject::connect(a, &QPropertyAnimation::finished, this->parentWidget(), &QWidget::deleteLater);
    }

Q_SIGNALS:

    /// Clicked signal, emitted when a mouse press event (simple click) occurs.
    void clicked();

    /// Double clicked signal, emitted when a mouse double click event occurs.
    void doubleClicked();

private:

    /**
     * @brief Handles event related to the mouse buttons.
     *
     * Send the signal @ref clicked().
     */
    void mousePressEvent(QMouseEvent*) override
    {
        Q_EMIT clicked();
    }

    /**
     * @brief Handles event related to the mouse buttons.
     *
     * Send the signal @ref doubleClicked().
     */
    void mouseDoubleClickEvent(QMouseEvent*) override
    {
        Q_EMIT doubleClicked();
    }
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
class UI_QT_CLASS_API NotificationDialog : public ui::base::dialog::INotificationDialog
{
public:

    SIGHT_DECLARE_CLASS(
        NotificationDialog,
        ui::base::dialog::INotificationDialog,
        ui::base::factory::New<NotificationDialog>
    )

    /// Initializes members.
    UI_QT_API NotificationDialog(ui::base::GuiBaseObject::Key key);

    /// Destroys the dialog.
    UI_QT_API ~NotificationDialog() override;

    /// Shows the notification relative to the active window.
    UI_QT_API void show() override;

    /**
     * @brief Returns whether the popup is displayed or not.
     * @return boolean (true is visible, false otherwise).
     */
    UI_QT_API bool isVisible() const override;

    /// Closes the popup (use a fadeout effect).
    UI_QT_API void close() const override;

private:

    /// Pointer to the Popup QLabel.
    QPointer<ClickableQLabel> m_msgBox {nullptr};
};

} // namespace dialog.

} // namespace sight::ui::qt.
