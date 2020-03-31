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

#pragma once

#include "fwGuiQt/config.hpp"

#include <fwCore/macros.hpp>

#include <fwGui/dialog/INotificationDialog.hpp>

#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QMouseEvent>
#include <QPointer>
#include <QPropertyAnimation>

#include <string>

namespace fwGuiQt
{

/// Creates an auto-movable container.
class FWGUIQT_CLASS_API Container : public QWidget
{

Q_OBJECT

public:

    /**
     * @brief Creates a Container.
     * @param _position a function that allows this container to compute its own position from its parent.
     * @param _parent the parent of this container.
     */
    explicit Container(std::function< QPoint(QWidget*) > _position, QWidget* _parent) :
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
    virtual bool eventFilter(QObject* _object, QEvent* _event) override
    {
        if(_event->type() == QEvent::Move || _event->type() == QEvent::Resize)
        {
            this->move(m_position(this->parentWidget()));
        }
        return QWidget::eventFilter(_object, _event);
    }

private:

    /// Defines a function that computes a position relatively to a parent widget.
    std::function< QPoint(QWidget*) > m_position;

};

/// Creates a clickable QLabel.
class FWGUIQT_CLASS_API ClickableQLabel : public QLabel
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

    /// Clicked signal, emited when mousePressEvent occurs.
    void clicked();

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
class FWGUIQT_CLASS_API NotificationDialog : public ::fwGui::dialog::INotificationDialog
{

public:

    fwCoreClassMacro(NotificationDialog, ::fwGui::dialog::INotificationDialog,
                     ::fwGui::factory::New< NotificationDialog > )

    /// Initializes members.
    FWGUIQT_API NotificationDialog(::fwGui::GuiBaseObject::Key key);

    /// Destroys the dialog.
    FWGUIQT_API virtual ~NotificationDialog() override;

    /// Shows the notification relative to the active window.
    FWGUIQT_API virtual void show() override;

    /**
     * @brief Returns whether the popup is displayed or not.
     * @return boolean (true is visible, false otherwise).
     */
    FWGUIQT_API virtual bool isVisible() const override;

    /// Closes the popup (use a fadeout effect).
    FWGUIQT_API virtual void close() const override;

private:

    /// Pointer to the Popup QLabel.
    QPointer< ClickableQLabel > m_msgBox { nullptr };

};

} // namespace dialog
} // namespace fwGuiQt
