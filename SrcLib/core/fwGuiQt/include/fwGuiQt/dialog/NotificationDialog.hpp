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

/// Create a clickable QLabel.
class FWGUIQT_CLASS_API ClickableQLabel : public QLabel
{
Q_OBJECT

public:
    /// Constructor, construct a QLabel.
    explicit ClickableQLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
        QLabel(parent)
    {
        FwCoreNotUsedMacro(f);
    }

    /// Destructor, does nothing.
    virtual ~ClickableQLabel()
    {

    }

public Q_SLOTS:

    /// Fade out effect launched when closing the Widget.
    void fadeout()
    {
        QGraphicsOpacityEffect* const effect = new QGraphicsOpacityEffect();
        this->setGraphicsEffect(effect);
        QPropertyAnimation* a = new QPropertyAnimation(effect, "opacity");
        a->setDuration(500); // it will took 500ms to fade out
        a->setStartValue(0.9);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        QObject::connect(a, &QPropertyAnimation::finished, this, &ClickableQLabel::close);
    }

Q_SIGNALS:
    /// Clicked signal, emited when mousePressEvent occurs.
    void clicked();

protected:
    /// Emit clicked signal.
    void mousePressEvent(QMouseEvent* _event)
    {
        FwCoreNotUsedMacro(_event);
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
 #Notification-Success
    {
        background-color:#58D68D;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }

 #Notification-Failure
    {
        background-color:#E74C3C;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }

 #Notification-Info
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

    /// Constructor, does nothing.
    FWGUIQT_API NotificationDialog(::fwGui::GuiBaseObject::Key key);

    /// Destructor, does nothing.
    FWGUIQT_API virtual ~NotificationDialog() override final;

    /// Show the notification relative to the active window.
    FWGUIQT_API void show() override;

    /**
     * @brief Returns whether the popup is displayed or not.
     * @return boolean (true is visible, false otherwise).
     */
    FWGUIQT_API bool isVisible() const override final;

    /**
     * @brief Closes the popup (use a fadeout effect).
     */
    FWGUIQT_API void close() const override final;

private:

    /**
     * @brief Computes the position using the active window boundings.
     * @return the position in pixel as a QPoint.
     */
    QPoint computePosition(const QWidget* _parent) const;

    /// Pointer to the Popup QLabel.
    QPointer< ClickableQLabel > m_msgBox;

};

} // namespace dialog
} // namespace fwGuiQt
